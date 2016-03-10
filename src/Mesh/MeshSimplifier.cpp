#include "MeshSimplifier.h"
#include <algorithm>
#include <unordered_set>
#include "Logger.h"


MeshSimplifier::MeshSimplifier(Mesh* _mesh) {
	mesh = _mesh;
}

MeshSimplifier::~MeshSimplifier() {

};

IndexType MeshSimplifier::pushEdgeCollapse(IndexType v1, IndexType v2, bool updateVbo) {
	// These values are floats but shouldn't have any arthimetic being performed on them
	// i.e: v = 1 or 0
	// Check if these edges are elligible for collapse
	if (!mesh->buffer[v1].v || !mesh->buffer[v2].v || v1 >= 2*mesh->buffer.size() || v2 >= 2*mesh->buffer.size()) {
		LogError("Verticies are unelligble for collapse\n");
		return NULL_INDEX;
	}
	VSet* v1Adj = &(mesh->vAdjs[v1]);
	VSet* v2Adj = &(mesh->vAdjs[v2]);

	// Removed Faces = v1 intersect v2
	// O(2*(v1Adj.size+v2Adj.size)-1) ; Linear in # Adj
	VSet removedFaces;
	set_intersection(v1Adj->begin(), v1Adj->end(), 
					 v2Adj->begin(), v2Adj->end(),
					 std::inserter(removedFaces, removedFaces.begin()));

	// vUnion = (v1 union v2)
	// O(v1Adj.size + v2Adj.size) ; Linear in # Adj
	VSet vUnion(*v1Adj);
	vUnion.insert(v2Adj->begin(), v2Adj->end());
	// New Vertex = vUnion - (v1 intersect v2)
	// O(2*(vUnion.size + removedFaces.size - 1)) ; Linear in # Adj
	VSet vN;
	std::set_difference(vUnion.begin(), vUnion.end(), 
						removedFaces.begin(), removedFaces.end(),
    					std::inserter(vN, vN.end()));

	// New vertex index = end of buffer
	IndexType vNindex = mesh->buffer.size();
	// Add new vertex to face adjacency list
	mesh->vAdjs.push_back(vN);

	// Add edge collapse event to history
	EdgeDelta delta;
	delta.before[0] = v1;
	delta.before[1] = v2;
	delta.after = vNindex;
	collapseHistory.push(delta);
	
	// New vertex is the midpoint of it's predecessors
	Datum newData;
	newData.p = collapseVertices(v1, v2);
	newData.n = (mesh->buffer[v1].n + mesh->buffer[v2].n)/2.0f;
	newData.c = (mesh->buffer[v1].c + mesh->buffer[v2].c)/2.0f;
	newData.v = 1;
	mesh->buffer.push_back(newData);

	// Set old verticies to invisible so faces get culled by Geometry shader
	mesh->buffer[v1].v = 0;
	mesh->buffer[v2].v = 0;

	// Iterate through all faces that use the new vertex
	for (IndexType faceInd : mesh->vAdjs[vNindex]) {
		Triangle& face = mesh->faces[faceInd];
		// Replace old vertices with new
		for (int i = 0; i < 3; i++) {
			if (mesh->faces[faceInd].v[i] == v1 || mesh->faces[faceInd].v[i] == v2) {
				mesh->faces[faceInd].v[i] = vNindex;
				mesh->indices[3*faceInd+i] = vNindex;
			}
		}
		// Fix face adj
		bool b = false;
		// Iterate through the current face's adjacent faces
		for (IndexType& face_adj : face.f) {
			if (face_adj == NULL_INDEX) {
				continue;
			}
			// For each removed face...
			for (IndexType face_i : removedFaces) {
				// if current face is adjacent to a removed face
				if (face_adj == face_i) {
					IndexType faceToRemove = face_adj;
					// Iterate through the removed face's adjacent faces
					for (IndexType& face_i_adj : mesh->faces[faceToRemove].f) {
						if (face_i_adj == NULL_INDEX) {
							continue;
						}
						// Current face can't be adjacent to itself
						if (face_i_adj != faceInd) {
							// Also can't be adjacent to another removed face
							for (IndexType face_k : removedFaces) {
								if (face_i_adj != face_k) {
									// We've found a face adjacent to the removed
									// face that our current face must now be adjacent
									// to. Update current faces adjaceny and move on
									// to next face.
									for (int i = 0; i < 3; i++) {
										if (mesh->faces[faceInd].f[i] == faceToRemove) {
											mesh->faces[faceInd].f[i] = face_i_adj;
											b = true; break;
										}
									}
									if (b) break;
								}
							}
							if (b) break;
						}
					}
				}
				if (b) break;
			}
			if (b) break;
		}
	}

	// Upload the mesh data to the graphics driver
	if (updateVbo) {
		mesh->updateVBO();
	}

	return vNindex;
}

bool MeshSimplifier::popEdgeCollapse(bool updateVbo) {
	if (collapseHistory.size() == 0) {
		return false;
	}

	EdgeDelta delta = collapseHistory.top();

	// Set old verticies as visible
	mesh->buffer[delta.before[0]].v = 1;
	mesh->buffer[delta.before[1]].v = 1;
	// Set collapsed verticies to invisible
	mesh->buffer[delta.after].v = 0;

	// Iterate through the previous verticies
	for (IndexType beforeInd : delta.before) {
		// Iterate through this previous verticies face adjacencies
		for (IndexType faceInd : mesh->vAdjs[beforeInd]) {
			for (int i = 0; i < 3; i++) {
				if (mesh->faces[faceInd].v[i] == delta.after) {
					mesh->faces[faceInd].v[i] = beforeInd;
					mesh->indices[3*faceInd+i] = beforeInd;
					break;
				}
			}
		}
	}
	
	// Upload the mesh data to the graphics driver
	if (updateVbo) {
		mesh->updateVBO();
	}
	collapseHistory.pop();
	return true;
}

void MeshSimplifier::initializeQuadricError(float threshold) {
	
	std::vector<mat4> faceK;
	faceK.reserve(mesh->faces.size());
	vertexQ.reserve(2*mesh->buffer.size());
	vpeLinks.resize(2*mesh->buffer.size());

	//LogInfo("Calculating face errors\n");
	// Find the fundamental error quadric of every face
	for (IndexType i = 0; i < mesh->faces.size(); i++) {
		Triangle& t = mesh->faces[i];
		vec3 v0 = mesh->buffer[t.v[0]].p;
		vec3 ab = mesh->buffer[t.v[1]].p - v0;
		vec3 ac = mesh->buffer[t.v[2]].p - v0;
		vec3 norm = cross(ab, ac);
		float d = -dot(norm, v0);
		vec4 p = vec4(norm.x, norm.y, norm.z, d);
		mat4 K = outerProduct(p, p);

		faceK.push_back(K);
		//LogInfo("Facek[%d]= %s\n",i,to_string(K).c_str());
	}

	//LogInfo("Computing Vertex Q's\n");
	// Compute initial Q for each vertex
	for (IndexType i = 0; i < mesh->vAdjs.size(); i++) {
		VSet& adjFaces = mesh->vAdjs[i];
		mat4 Q = mat4(0);
		for (auto face : adjFaces) {
			Q = Q + faceK[face];
		}
		vertexQ.push_back(Q);
		//LogInfo("VertexQ[%d]= %s\n",i,to_string(Q).c_str());
	}

	//LogInfo("Finding all valid pairs.\n");
	// Find all Valid Pairs
	using ValidPairs = std::set<IndexType>;
	// Combine hashes into a new unique hash - Thanks Boost
	struct ValidPairsHasher {
	    size_t operator()(const ValidPairs& v) const {
	        std::hash<IndexType> hasher;
	        size_t seed = 0;
	        for (IndexType i : v) {
	            seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
	        }
	        return seed;
	    }
	};
	std::unordered_set<ValidPairs, ValidPairsHasher> vp;
	for (IndexType i = 0; i < mesh->vAdjs.size(); i++) {
		VSet& adjFaces = mesh->vAdjs[i];
		//LogInfo("Finding edge pairs for %d\n",i);
		// Add valid pairs that share an existing edge
		for (IndexType face_i : adjFaces) {
			for (auto pair_i : mesh->faces[face_i].v) {
				if (i != pair_i) {
					ValidPairs newVP = ValidPairs();
					newVP.insert(i);
					newVP.insert(pair_i);
					auto result = vp.insert(newVP);
					// DEBUGGING
					/*
					if (result.second) {
						int i = 0;
						IndexType v[2];
						for (auto vertex : *(result.first)) {
							v[i++] = vertex;
						}
						LogInfo("New VP: (%d,%d)\n", v[0], v[1]);
					}
					*/
				}
			}
		}
		//LogInfo("Finding distance pairs for %d\n",i);
		// Add valid pairs of vertices that are within threshold distance
		// to each other
		// Iterate through every other vertex
		// This makes quadratic load time
		/*
		for (auto otherVertex : vAdjs) {
			IndexType j = otherVertex.first;
			if (length(buffer[i].p - buffer[i].p) < threshold) {
				if (i != j) {
					ValidPairs newVP = ValidPairs();
					newVP.insert(i);
					newVP.insert(j);
					auto result = vp.insert(newVP);
					// DEBUGGING
					if (result.second) {
						int i = 0;
						IndexType v[2];
						for (auto vertex : *(result.first)) {
							v[i++] = vertex;
						}
						//LogInfo("New VP: (%d,%d)\n", v[0], v[1]);
					}
				}
			}
		}
		*/
	}

	//LogInfo("Starting VPE creation.\n");
	// Compute quadric errors for contractrions and add to heap
	for (auto pair : vp) {
		int i = 0;
		IndexType v[2];
		for (auto vertex : pair) {
			v[i++] = vertex;
		}

		VPEheap::iterator it = insertVPE(v[0], v[1]);

		// Add a link to the VPE for both verticies
		vpeLinks[v[0]].insert(it);
		vpeLinks[v[1]].insert(it);
	}
}

VPEheap::iterator MeshSimplifier::insertVPE(IndexType v0, IndexType v1) {
	// Collapse the valid pairs
	vec4 vCollapse = vec4(collapseVertices(v0,v1), 1.0);

	// Compute the error of their collapse
	ValidPairError vpe;
	// error = v.T*(K_0 + K_1)*v
	vpe.error = dot(vCollapse*(vertexQ[v0] + vertexQ[v1]),vCollapse);
	vpe.v[0] = v0;
	vpe.v[1] = v1;

	//LogInfo("Adding VPE: (%d,%d,%f)\n",vpe.v[0], vpe.v[1], vpe.error);

	// Insert the Valid Pair Error into the heap
	auto insertResult = vertexErrors.insert(vpe);
	if (!insertResult.second) {
		//LogInfo("Duplicate VPE insert (%d,%d)\n",v0, v1);
	}
	return insertResult.first;
}

void MeshSimplifier::printVPElinks(bool showV) {
	LogInfo("VPElinks state:\n");
	for (IndexType i = 0; i < vpeLinks.size(); i++) {
		if (vpeLinks[i].size() == 0) {
			continue;
		}
		LogInfo("vpeLinks[%d] = {",i);
		for (auto adjV : vpeLinks[i]) {
			if (showV) {
				LogInfo("{%d,%d,%d}",adjV->v[0],adjV->v[1],adjV);
			} else {
				LogInfo("{%d}",adjV);
			}
		}
		LogInfo("}\n");
	}
}

void MeshSimplifier::printVPE(bool showError) {
	LogInfo("VPE :{");
	for (auto pair : vertexErrors) {
		if (showError) {
			LogInfo("{%d,%d,e=%f}",pair.v[0],pair.v[1],pair.error);
		} else {
			LogInfo("{%d,%d}",pair.v[0],pair.v[1]);
		}
	}
	LogInfo("}\n");
}

void MeshSimplifier::quadricSimplifyStep(bool updateVbo) {
	//printVPElinks(true);
	//printVPE(false);
	
	// Simple heuristic to prevent segfaults until vpeLinks bug is fixed
	if (vertexErrors.size() < 5) {
		return;
	}
	
	// Get the minnimum VPE from the heap
	auto it = vertexErrors.begin();
	
	// Collapse the edge
	IndexType newV = pushEdgeCollapse(it->v[0], it->v[1], updateVbo);
	if (newV == NULL_INDEX) {
		LogError("Unable to collapse verticies (%d,%d)\n", it->v[0], it->v[1]);
		return;
	}

	// Add new Q error
	//LogInfo("Collapsing edge (%d,%d,err=%f->(%d)\n", it->v[0], it->v[1], it->error, newV);
	vertexQ.push_back(vertexQ[it->v[0]] + vertexQ[it->v[1]]);

	std::vector<std::pair<IndexType,VPEheap::iterator>> toRemove;
	std::vector<std::pair<IndexType,VPEheap::iterator>> toAdd;

	for (IndexType vp : it->v) {
		for (VPEheap::iterator vpeIt : vpeLinks[vp]) {
			ValidPairError vpe = ValidPairError(*(vpeIt));
			if (vpe.v[0] == it->v[0] && vpe.v[1] == it->v[1]) {
				toRemove.push_back(std::make_pair(vp, vpeIt));
				continue;
			}
			IndexType v;
			if (vpe.v[0] == vp) {
				v = vpe.v[1];
				// maintain order
				if (v < newV) {
					vpe.v[0] = v;
					vpe.v[1] = newV;
				} else {
					vpe.v[0] = newV;
				}
			} else if (vpe.v[1] == vp) {
				v = vpe.v[0];
				if (v > newV) {
					vpe.v[1] = v;
					vpe.v[0] = newV;
				} else {
					vpe.v[1] = newV;
				}
			}

			// Remove the old VPE, add the updated one, update the iterator
			vertexErrors.erase(vpeIt);

			VPEheap::iterator it = insertVPE(vpe.v[0], vpe.v[1]);

			toRemove.push_back(std::make_pair(vp, vpeIt));
			toRemove.push_back(std::make_pair(v, vpeIt));
			toAdd.push_back(std::make_pair(vp, it));
			toAdd.push_back(std::make_pair(v, it));
			toAdd.push_back(std::make_pair(newV, it));
		}
	}

	//Remove the vpeLinks
	for (auto vpeLinkRm : toRemove) {
		auto result = vpeLinks[vpeLinkRm.first].erase(vpeLinkRm.second);
		if (result) {
			//printVPElinks(true);
			//LogInfo("Removing vpeLink[%d]: (%d,%d,%d)\n",vpeLinkRm.first,vpeLinkRm.second->v[0],vpeLinkRm.second->v[1],vpeLinkRm.second);
		}
	}
	// Add the new ones
	for (auto vpeLinkAdd : toAdd) {
		auto result = vpeLinks[vpeLinkAdd.first].insert(vpeLinkAdd.second);
		if (result.second) {
			//printVPElinks(true);
			//LogInfo("Adding vpeLink[%d]: (%d,%d,%d)\n",vpeLinkAdd.first,vpeLinkAdd.second->v[0],vpeLinkAdd.second->v[1],vpeLinkAdd.second);
		}
	}

	// Remove the collapsed pair
	//LogInfo("Removing main VPE: (%d,%d,%f)\n",it->v[0], it->v[1], it->error);
	vertexErrors.erase(it);
}

vec3 MeshSimplifier::collapseVertices(IndexType v0, IndexType v1) {
	return (mesh->buffer[v0].p + mesh->buffer[v1].p)/2.0f;
}