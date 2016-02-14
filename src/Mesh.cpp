#include "Mesh.h"
#include "Logger.h"

Mesh::Mesh() {
	
}

Mesh::~Mesh() {
	
}

bool Mesh::edgeCollapse(IndexType v1, IndexType v2) {
	VSet* v1Adj = &vAdjs[v1];
	VSet* v2Adj = &vAdjs[v2];

	// Removed Faces = v1 intersect v2
	// O(2*(v1Adj.size+v2Adj.size)-1) ; Linear in # Adj
	VSet removedFaces; // Size will be 1 or 2
	set_intersection(v1Adj->begin(), v1Adj->end(), 
					 v2Adj->begin(), v2Adj->end(),
					 std::inserter(removedFaces, removedFaces.begin()));

	// New Vertex = (v1 union v2) - (v1 intersect v2)
	// O(v1Adj.size + v2Adj.size*log(v1Adj.size*v2Adj.size) + removedFaces.size) ; Logrithmic in # Adj
	VSet vN(v1Adj);
	vN.insert(v2Adj->begin(), v2Adj->end()).erase(removedFaces.begin(), removedFaces.end());

	// Remove old vertices, add new
	// O(2) ; Constant
	vAdjs.erase(v1);
	vAdjs.erase(v2);
	auto insertResult = vAdjs.emplace(v1, vN);
	if (!insertResult.second) {
		// Insert failed - Something bad happend
		Logger::err("Unable to insert new vertex, this shouldn't happen. Verify edgeCollapse algorithm.");
		return false;
	}
	// Store iterator to new vertex inside vAdjs
	auto vNit = insertResult.first;
	IndexType vNindex = vNit->first;
	Triangle* vNptr = &(vNit->second);

	// Iterate through all faces that use the new vertex
	for (IndexType& faceInd : vNptr) {
		Triangle& face = faces[faceInd];
		// Replace old vertices with new
		for (IndexType& vi : face.v) {
			if (vi == v1 || vi == v2) {
				vi = vNindex;
			}
		}
		// Fix face adj
		bool b = false;
		// Iterate through the current face's adjacent faces
		for (IndexType& face_adj : face.f) {
			// For each removed face...
			for (IndexType& face_i : removedFaces) {
				// if current face is adjacent to a removed face
				if (face_adj == face_i) {
					// We need to find the new adjacent face
					// Iterate through the removed face's adjacent faces
					for (IndexType& face_i_adj : faces[face_i].f) {
						// Current face can't be adjacent to itself
						if (face_i_adj != faceInd) {
							// Also can't be adjacent to another removed face
							for (IndexType& face_k : removedFaces) {
								if (face_i_adj != face_k) {
									// We've found an adjacent face in the removed
									// face that our current face must not be adjacent
									// to. Update current faces adjaceny and move on
									// to next face.
									face.f[face_adj] = face_i_adj;
									b = true; break;
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

	// Remove faces sharing the old edge
	// O(removedFaces.size) ; Constant
	faces.erase(removedFaces.begin(), removedFaces.end());

	return true;
}