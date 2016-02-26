#include "Mesh.h"
#include "Logger.h"
#include "Util.h"
#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <glm/gtc/matrix_transform.hpp>


// For debugging
#include <glm/gtx/string_cast.hpp>
//#include <glm/gtc/type_ptr.hpp>

using namespace Util;

Mesh::Mesh() {
	
}

// Copy constructor
Mesh::Mesh(const Mesh& m) {
	indicies = m.indicies;
	buffer = m.buffer;
	buffer.reserve(2*buffer.size());

	vAdjs = m.vAdjs;
	faces = m.faces;

	collapseHistory = m.collapseHistory;

	setupBuffers();
}

Mesh::~Mesh() {
	
}

void Mesh::draw() {
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indicies.size(), IndexTypeGL, (void*)0);

	glBindVertexArray(0);
}

IndexType Mesh::pushEdgeCollapse(IndexType v1, IndexType v2, bool updateVbo) {
	// These values are floats but shouldn't have any arthimetic being performed on them
	// i.e: v = 1 or 0
	// Check if these edges are elligible for collapse
	/*if (!buffer[v1].v || !buffer[v2].v || v1 >= 2*buffer.size() || v2 >= 2*buffer.size()) {
		Logger::err("Verticies are unelligble for collapse\n");
		return NULL_INDEX;
	}*/
	VSet* v1Adj = &vAdjs[v1];
	VSet* v2Adj = &vAdjs[v2];

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
	if (buffer.size() != vAdjs.size()) {
		Logger::err("BAD");
	}
	// New vertex index = end of buffer
	IndexType vNindex = buffer.size();
	// Add new vertex to face adjacency list
	vAdjs.push_back(vN);

	// Add edge collapse event to history
	EdgeDelta delta;
	delta.before[0] = v1;
	delta.before[1] = v2;
	delta.after = vNindex;
	collapseHistory.push(delta);
	
	// New vertex is the midpoint of it's predecessors
	Datum newData;
	newData.p = collapseVertices(v1, v2);
	newData.n = (buffer[v1].n + buffer[v2].n)/2.0f;
	newData.c = (buffer[v1].c + buffer[v2].c)/2.0f;
	newData.v = 1;
	buffer.push_back(newData);

	// Set old verticies to invisible so faces get culled by Geometry shader
	buffer[v1].v = 0;
	buffer[v2].v = 0;

	// Iterate through all faces that use the new vertex
	for (IndexType faceInd : vAdjs[vNindex]) {
		Triangle& face = faces[faceInd];
		// Replace old vertices with new
		for (int i = 0; i < 3; i++) {
			if (faces[faceInd].v[i] == v1 || faces[faceInd].v[i] == v2) {
				faces[faceInd].v[i] = vNindex;
				indicies[3*faceInd+i] = vNindex;
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
					for (IndexType& face_i_adj : faces[faceToRemove].f) {
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
										if (faces[faceInd].f[i] == faceToRemove) {
											faces[faceInd].f[i] = face_i_adj;
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
		updateVBO();
	}

	return vNindex;
}

bool Mesh::popEdgeCollapse(bool updateVbo) {
	if (collapseHistory.size() == 0) {
		return false;
	}

	EdgeDelta delta = collapseHistory.top();

	// Set old verticies as visible
	buffer[delta.before[0]].v = 1;
	buffer[delta.before[1]].v = 1;
	// Set collapsed verticies to invisible
	buffer[delta.after].v = 0;

	// Iterate through the previous verticies
	for (IndexType beforeInd : delta.before) {
		// Iterate through this previous verticies face adjacencies
		for (IndexType faceInd : vAdjs[beforeInd]) {
			for (int i = 0; i < 3; i++) {
				if (faces[faceInd].v[i] == delta.after) {
					faces[faceInd].v[i] = beforeInd;
					indicies[3*faceInd+i] = beforeInd;
					break;
				}
			}
		}
	}
	
	// Upload the mesh data to the graphics driver
	if (updateVbo) {
		updateVBO();
	}
	collapseHistory.pop();
	return true;
}

void Mesh::initializeQuadricError(float threshold) {
	
	std::vector<mat4> faceK;
	faceK.reserve(faces.size());
	vertexQ.reserve(2*buffer.size());
	vpeLinks.resize(2*buffer.size());

	//Logger::info("Calculating face errors\n");
	// Find the fundamental error quadric of every face
	for (IndexType i = 0; i < faces.size(); i++) {
		Triangle& t = faces[i];
		vec3 v0 = buffer[t.v[0]].p;
		vec3 ab = buffer[t.v[1]].p - v0;
		vec3 ac = buffer[t.v[2]].p - v0;
		vec3 norm = cross(ab, ac);
		float d = -dot(norm, v0);
		vec4 p = vec4(norm.x, norm.y, norm.z, d);
		mat4 K = outerProduct(p, p);

		faceK.push_back(K);
		//Logger::info("Facek[%d]= %s\n",i,to_string(K).c_str());
	}

	//Logger::info("Computing Vertex Q's\n");
	// Compute initial Q for each vertex
	for (IndexType i = 0; i < vAdjs.size(); i++) {
		VSet& adjFaces = vAdjs[i];
		mat4 Q = mat4(0);
		for (auto face : adjFaces) {
			Q = Q + faceK[face];
		}
		vertexQ.push_back(Q);
		//Logger::info("VertexQ[%d]= %s\n",i,to_string(Q).c_str());
	}

	//Logger::info("Finding all valid pairs.\n");
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
	for (IndexType i = 0; i < vAdjs.size(); i++) {
		VSet& adjFaces = vAdjs[i];
		//Logger::info("Finding edge pairs for %d\n",i);
		// Add valid pairs that share an existing edge
		for (IndexType face_i : adjFaces) {
			for (auto pair_i : faces[face_i].v) {
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
						Logger::info("New VP: (%d,%d)\n", v[0], v[1]);
					}
					*/
				}
			}
		}
		//Logger::info("Finding distance pairs for %d\n",i);
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
						//Logger::info("New VP: (%d,%d)\n", v[0], v[1]);
					}
				}
			}
		}
		*/
	}

	//Logger::info("Starting VPE creation.\n");
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

VPEheap::iterator Mesh::insertVPE(IndexType v0, IndexType v1) {
	// Collapse the valid pairs
	vec4 vCollapse = vec4(collapseVertices(v0,v1), 1.0);

	// Compute the error of their collapse
	ValidPairError vpe;
	// error = v.T*(K_0 + K_1)*v
	vpe.error = dot(vCollapse*(vertexQ[v0] + vertexQ[v1]),vCollapse);
	vpe.v[0] = v0;
	vpe.v[1] = v1;

	//Logger::info("Adding VPE: (%d,%d,%f)\n",vpe.v[0], vpe.v[1], vpe.error);

	// Insert the Valid Pair Error into the heap
	auto insertResult = vertexErrors.insert(vpe);
	if (!insertResult.second) {
		//Logger::info("Duplicate VPE insert (%d,%d)\n",v0, v1);
	}
	return insertResult.first;
}

void Mesh::printVPElinks(bool showV) {
	Logger::info("VPElinks state:\n");
	for (IndexType i = 0; i < vpeLinks.size(); i++) {
		if (vpeLinks[i].size() == 0) {
			continue;
		}
		Logger::info("vpeLinks[%d] = {",i);
		for (auto adjV : vpeLinks[i]) {
			if (showV) {
				Logger::info("{%d,%d,%d}",adjV->v[0],adjV->v[1],adjV);
			} else {
				Logger::info("{%d}",adjV);
			}
		}
		Logger::info("}\n");
	}
}

void Mesh::printVPE(bool showError) {
	Logger::info("VPE :{");
	for (auto pair : vertexErrors) {
		if (showError) {
			Logger::info("{%d,%d,e=%f}",pair.v[0],pair.v[1],pair.error);
		} else {
			Logger::info("{%d,%d}",pair.v[0],pair.v[1]);
		}
	}
	Logger::info("}\n");
}

void Mesh::quadricSimplifyStep(bool updateVbo) {
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
		Logger::err("Unable to collapse verticies (%d,%d)\n", it->v[0], it->v[1]);
		return;
	}

	// Add new Q error
	//Logger::info("Collapsing edge (%d,%d,err=%f->(%d)\n", it->v[0], it->v[1], it->error, newV);
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
			//Logger::info("Removing vpeLink[%d]: (%d,%d,%d)\n",vpeLinkRm.first,vpeLinkRm.second->v[0],vpeLinkRm.second->v[1],vpeLinkRm.second);
		}
	}
	// Add the new ones
	for (auto vpeLinkAdd : toAdd) {
		auto result = vpeLinks[vpeLinkAdd.first].insert(vpeLinkAdd.second);
		if (result.second) {
			//printVPElinks(true);
			//Logger::info("Adding vpeLink[%d]: (%d,%d,%d)\n",vpeLinkAdd.first,vpeLinkAdd.second->v[0],vpeLinkAdd.second->v[1],vpeLinkAdd.second);
		}
	}

	// Remove the collapsed pair
	//Logger::info("Removing main VPE: (%d,%d,%f)\n",it->v[0], it->v[1], it->error);
	vertexErrors.erase(it);
}

vec3 Mesh::collapseVertices(IndexType v0, IndexType v1) {
	return (buffer[v0].p + buffer[v1].p)/2.0f;
}

void Mesh::createVertexNormals() {
	std::vector<vec3> faceNormals;
	faceNormals.resize(faces.size());
	for (IndexType i = 0; i < faces.size(); i++) {
		Triangle& face = faces[i];
		vec3 v0 = buffer[face.v[0]].p;
		vec3 v1 = buffer[face.v[1]].p;
		vec3 v2 = buffer[face.v[2]].p;

		faceNormals[i] = cross(v0 - v1, v0 - v2);
	}

	for (IndexType i = 0; i < vAdjs.size(); i++) {
		vec3 vTot = vec3(0);
		int n = 0;
		for (auto face : vAdjs[i]) {
			vTot = vTot + faceNormals[face];
			n++;
		}
		vTot = vTot * (1.0f/n);
		buffer[i].n = normalize(vTot);
	}
	updateVBO();
}

void Mesh::setupBuffers() {
	// Size of buffer is 2*(# vertices) to leave room for vertices created through edge collapse
	dataBufferMaxSize = 2 * sizeof(buffer[0]) * buffer.size();
	// Number of faces shouldn't increase
	indexBufferMaxSize = sizeof(indicies[0]) * indicies.size();

	// Generate the Vertex Array and bind to it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(NUM_OF_BUFFERS, bufferIds);

	// Bind the data VBO
	glBindBuffer(GL_ARRAY_BUFFER, bufferIds[DATA_BUFFER]);

	// Specify how the information is packed
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), 0);

	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)sizeof(vec3));

	glEnableVertexAttribArray(COLOR_LOCATION);
	glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(2*sizeof(vec3)));

	glEnableVertexAttribArray(VISIBLE_LOCATION);
	glVertexAttribPointer(VISIBLE_LOCATION, 1, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(3*sizeof(vec3)));

	glBindVertexArray(0);

	// Upload the data
	updateVBO();
}

void Mesh::updateVBO() {
	// Bind to the VAO
	glBindVertexArray(VAO);

	// Bind the data buffer vbo
	glBindBuffer(GL_ARRAY_BUFFER, bufferIds[DATA_BUFFER]);

	// Orphan the old data VBO. Implements Buffer respecification.
	// Let the graphics driver implement multi-buffering of the VBO.
	glBufferData(GL_ARRAY_BUFFER, dataBufferMaxSize, NULL, GL_STREAM_DRAW);
	glBufferData(GL_ARRAY_BUFFER, dataBufferMaxSize, &buffer[0], GL_STREAM_DRAW);

	// Bind the index buffer vbo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIds[INDEX_BUFFER]);

	// Orphan the old index VBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferMaxSize, NULL, GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferMaxSize, &indicies[0], GL_STREAM_DRAW);

	glBindVertexArray(0);
}

bool Mesh::parseOFF(const char* filename) {
	// Read file
	std::ifstream infile(std::string(MODEL_PATH) + filename);
	std::string line;
	std::vector<std::string> tokens;
	std::vector<std::string> faceTokens;
	std::string token;
	bool hasReadSize = false;

	IndexType face_i = 0;

	double xmin, xmax, ymin, ymax, zmin, zmax;

	xmin = ymin = zmin = std::numeric_limits<double>::max();
	xmax = ymax = zmax = std::numeric_limits<double>::min();

	if (!infile.is_open() || infile.bad() || !infile.good() || infile.fail()) {
		Logger::err("Error opening file [%s]\n", (std::string(MODEL_PATH) + filename).c_str());
		return false;
	}

	// Clear old mesh data
	buffer.clear();
	indicies.clear();
	vAdjs.clear();
	faces.clear();

	// Check if header string is valid
	std::getline(infile, line);
	if (line.find("OFF") == std::string::npos) {
		Logger::err("%s: file does not follow the OFF specification\n", filename);
		return false;
	}

	while (std::getline(infile, line)) {
		// Remove beginning and ending whitespace
		trim(line);

		// Split line
		tokens.clear();
		split(line, ' ', tokens);

		// Ignore blank/comment line
		if (tokens.size() == 0 || tokens.at(0).compare("#") == 0)
			continue;

		if (!hasReadSize) {
			// Read object data size
			IndexType numVerticies = std::stoi(tokens.at(0));
			IndexType numFaces = std::stoi(tokens.at(1));
			//IndexType numEdges = std::stoi(tokens.at(2));

			vAdjs.reserve(2*numVerticies);
			faces.reserve(numFaces);
			buffer.reserve(2*numVerticies);
			indicies.reserve(3*numFaces);
			hasReadSize = true;
		} else if (tokens.size() == 3) {
			// Reading vertex
			Datum d;
			d.p.x = std::stof(tokens.at(0));
			d.p.y = std::stof(tokens.at(1));
			d.p.z = std::stof(tokens.at(2));

			xmin = (d.p.x < xmin) ? (d.p.x) : (xmin);
			xmax = (d.p.x > xmax) ? (d.p.x) : (xmax);
			ymin = (d.p.y < ymin) ? (d.p.y) : (ymin);
			ymax = (d.p.y > ymax) ? (d.p.y) : (ymax);
			zmin = (d.p.z < zmin) ? (d.p.z) : (zmin);
			zmax = (d.p.z > zmax) ? (d.p.z) : (zmax);

			d.c.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			d.c.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			d.c.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			d.v = 1.0; // Visible

			buffer.push_back(d);
			vAdjs.push_back(VSet());

		} else if (tokens.size() == 4) {
			// Reading face
			if (tokens.at(0).compare("3") != 0) {
				Logger::err("Error parsing %s: file format uses non-triangle faces (%s)\n", filename, line);
				return false;
			}
			IndexType v0 = std::stoi(tokens.at(1));
			IndexType v1 = std::stoi(tokens.at(2));
			IndexType v2 = std::stoi(tokens.at(3));

			indicies.push_back(v0);
			indicies.push_back(v1);
			indicies.push_back(v2);

			vAdjs[v0].insert(face_i);
			vAdjs[v1].insert(face_i);
			vAdjs[v2].insert(face_i);
			face_i++;

			Triangle t;
			t.v = {v0, v1, v2};
			t.f = {NULL_INDEX, NULL_INDEX, NULL_INDEX};

			faces.push_back(t);
		}
    }

    maxWidth = abs(xmax - xmin);
    maxHeight = abs(ymax - ymin);
    maxDepth = abs(zmax - zmin);
    center = vec3((xmin + xmax) / 2.0,
    			  (ymin + ymax) / 2.0,
    			  (zmin + zmax) / 2.0);

    // Create Face Adjaceny in Indexed Face list
	for (IndexType i = 0; i < faces.size(); i++) {
		Triangle& face = faces[i];

		IndexType v0 = face.v[0];
		IndexType v1 = face.v[1];
		IndexType v2 = face.v[2];

		VSet v0Adj = vAdjs[v0];
		VSet v1Adj = vAdjs[v1];
		VSet v2Adj = vAdjs[v2];

		// Size will be 1 or 2
		VSet v0v1Intersect;
		VSet v1v2Intersect;
		VSet v0v2Intersect;
		// Find the adjacent faces to the edge
		set_intersection(v0Adj.begin(), v0Adj.end(), 
						 v1Adj.begin(), v1Adj.end(),
						 std::inserter(v0v1Intersect, v0v1Intersect.begin()));
		set_intersection(v0Adj.begin(), v0Adj.end(), 
						 v2Adj.begin(), v2Adj.end(),
						 std::inserter(v0v2Intersect, v0v2Intersect.begin()));
		set_intersection(v1Adj.begin(), v1Adj.end(), 
						 v2Adj.begin(), v2Adj.end(),
						 std::inserter(v1v2Intersect, v1v2Intersect.begin()));

		// Combine all the faces
		VSet adjFaces(v0v1Intersect);
		adjFaces.insert(v1v2Intersect.begin(), v1v2Intersect.end());
		adjFaces.insert(v0v2Intersect.begin(), v0v2Intersect.end());

		face_i = 0;
		// Set all the adjacent faces, ignoring the occurance of itself.
		for (auto adjFace : adjFaces) {
			if (i != adjFace) {
				face.f[face_i++] = adjFace;
			}
		}
	}
/*
	for (IndexType i = 0; i < faces.size(); i++) {
		Logger::info("faces[%d]: {%d,%d,%d}{%d,%d,%d}\n",i,faces[i].v[0],
			faces[i].v[1],faces[i].v[2],faces[i].f[0],faces[i].f[1],faces[i].f[2]);
	}

	for (IndexType i = 0; i < vAdjs.size(); i++) {
		Logger::info("vAdjs[%d]:{",i);
		for (auto face : vAdjs[i]) {
			Logger::info("%d,",face);
		}
		Logger::info("}\n");
	}
*/

    setupBuffers();
    createVertexNormals();
    initializeQuadricError(0.15);
    return true;
}

double Mesh::getMaxWidth() {
	return maxWidth;
}

double Mesh::getMaxHeight() {
	return maxHeight;
}

double Mesh::getMaxDepth() {
	return maxDepth;
}

vec3 Mesh::getCenter() {
	return center;
}

IndexType Mesh::getNumberVerticies() {
	return buffer.size();
}