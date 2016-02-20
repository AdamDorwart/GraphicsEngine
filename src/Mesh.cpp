#include "Mesh.h"
#include "Logger.h"
#include "Util.h"
#include <algorithm>
#include <fstream>

// For debugging
//#include <glm/gtx/string_cast.hpp>
//#include <glm/gtc/type_ptr.hpp>

using namespace Util;

Mesh::Mesh() {
	
}

// Copy constructor
Mesh::Mesh(const Mesh& m) {
	indicies = m.indicies;
	buffer = m.buffer;
	bufferSize = m.bufferSize;
	buffer.resize(2*bufferSize);

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


// Debugging function - delete asap
void Mesh::drawEdge(IndexType v1, IndexType v2) {
 	Datum vec1 = buffer[v1];
	Datum vec2 = buffer[v2];
	vec1.c = {1.0, 0, 0};
	vec2.c = {1.0, 0, 0};
	Datum vertices[] = { vec1, vec2};

	GLuint vbo, vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), 0);

	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)sizeof(vec3));

	glEnableVertexAttribArray(COLOR_LOCATION);
	glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(2*sizeof(vec3)));

	glEnableVertexAttribArray(VISIBLE_LOCATION);
	glVertexAttribPointer(VISIBLE_LOCATION, 1, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(3*sizeof(vec3)));
	glDrawArrays(GL_LINES, 0, 2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

bool Mesh::pushEdgeCollapse(IndexType v1, IndexType v2) {
	// These values are floats but shouldn't have any arthimetic being performed on them
	// i.e: v = 1 or 0
	// Check if these edges are elligible for collapse
	if (!buffer[v1].v || !buffer[v2].v || v1 >= bufferSize || v2 >= bufferSize) {
		return false;
	}
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

	// New vertex index = end of buffer
	IndexType vNindex = bufferSize++;

	// Add edge collapse event to history
	EdgeDelta delta;
	delta.before[0] = v1;
	delta.before[1] = v2;
	delta.after = vNindex;
	collapseHistory.push(delta);

	// Add new vertex to vertex adjacency list
	auto insertResult = vAdjs.emplace(vNindex, vN);
	if (!insertResult.second) {
		// Insert failed - Something bad happend
		Logger::err("Unable to insert new vertex, index already exists. " 
			        "This shouldn't happen. Verify edgeCollapse algorithm.\n");
		return false;
	}
	
	// New vertex is the midpoint of it's predecessors
	Datum newData;
	newData.p = (buffer[v1].p + buffer[v2].p)/2.0f;
	newData.n = (buffer[v1].n + buffer[v2].n)/2.0f;
	newData.c = (buffer[v1].c + buffer[v2].c)/2.0f;
	newData.v = 1;
	buffer[vNindex] = newData;

	// Set old verticies to invisible so faces get culled by Geometry shader
	buffer[v1].v = 0;
	buffer[v2].v = 0;

	// Store iterator to new vertex inside vAdjs
	auto vNit = insertResult.first;
	VSet* vNptr = &(vNit->second);

	// Iterate through all faces that use the new vertex
	for (IndexType faceInd : *vNptr) {
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
			// For each removed face...
			for (IndexType face_i : removedFaces) {
				// if current face is adjacent to a removed face
				if (face_adj == face_i) {
					IndexType faceToRemove = face_adj;
					// Iterate through the removed face's adjacent faces
					for (IndexType& face_i_adj : faces[faceToRemove].f) {
						// Current face can't be adjacent to itself
						if (face_i_adj != faceInd) {
							// Also can't be adjacent to another removed face
							for (IndexType face_k : removedFaces) {
								if (face_i_adj != face_k) {
									// We've found a face adjacent to the removed
									// face that our current face must now be adjacent
									// to. Update current faces adjaceny and move on
									// to next face.
									face.f[faceToRemove] = face_i_adj;
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
	//for (IndexType face : removedFaces) {
	//	faces.erase(face);
	//}

	// Upload the mesh data to the graphics driver
	updateVBO();

	return true;
}

bool Mesh::popEdgeCollapse() {
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
	updateVBO();
	collapseHistory.pop();
	return true;
}

void Mesh::initializeQuadricError() {
	struct ValidPair {
		float error;
		IndexType v0;
		IndexType v1;
	};
	std::priority_queue<ValidPair> vertexErrors;
	std::vector<mat4> faceK;

	// Find the fundamental error quadric of every face
	for (auto face : faces) {
		IndexType& i = face.first;
		Triangle& t = face.second;
		vec3 v0 = buffer[t.v[0]].p;
		vec3 ab = buffer[t.v[1]].p - v0;
		vec3 ac = buffer[t.v[2]].p - v0;
		vec3 norm = cross(ab, ac);
		float d = -dot(norm, v0);
		vec4 p = transpose(vec4(norm.x, norm.y, norm.z, d));
		mat4 K = p*transpose(p);

		faceK[i] = K;
	}

	// Compute initial Q for each vertex
	std::vector<mat4> vertexQ;
	for (auto vertex : vAdjs) {
		IndexType& i = vertex.first;
		VSet& faces = vertex.second;
		mat4 Q = mat4(0);
		for (auto face : faces) {
			Q = Q + faceErrors[face];
		}
		vertexQ[i] = Q;
	}

	// Find all Valid Pairs
	using ValidPairs = std::set<IndexType>;
	std::unordered_map<ValidPairs, ???> vp;
	// Fuck this part of code
	for (auto vertex : vAdjs) {
		IndexType& i = vertex.first;
		VSet& faces = vertex.second;
		for (IndexType face_i : faces) {
			for (auto pair : faces[face_i].v) {
				ValidPairs newVP = std::set();
				newVP.insert(vertex);
				newVP.insert(pair);
				vp[newVP] = ???;
			}
		}
		// Iterate through every other vertex
		for (otherVertex ...) {
			if (length(buffer[i].p - otherVertex.p) < threshold) {
				ValidPairs newVP = std::set();
				newVP.insert(vertex);
				newVP.insert(otherVertex);
				vp[newVP] = ???;
			}
		}
	}

	// Compute quadric errors for contractrions and add to heap
	for (auto pair : vp) {
		vec4 vCollapse = collapseVertex(pair);
		float error = transpose(vCollapse)*(vertexQ[pair[0]] + vertexQ[pair[1]])*vCollapse;
		vertexErrors.insert( {error, pair});
	}
}

void Mesh::createVertexNormals() {
	std::vector<vec3> faceNormals;
	faceNormals.resize(faces.size());
	for (auto face : faces) {
		vec3 v0 = buffer[face.second.v[0]].p;
		vec3 v1 = buffer[face.second.v[1]].p;
		vec3 v2 = buffer[face.second.v[2]].p;

		faceNormals[face.first] = cross(v0 - v1, v0 - v2);
	}

	for (auto vertex : vAdjs) {
		vec3 vTot = vec3(0);
		int i = 0;
		for (auto face : vertex.second) {
			vTot = vTot + faceNormals[face];
			i++;
		}
		vTot = vTot * (1.0f/i);
		buffer[vertex.first].n = normalize(vTot);
	}
	updateVBO();
}

void Mesh::setupBuffers() {
	// Size of buffer is 2*(# vertices) to leave room for vertices created through edge collapse
	dataBufferMaxSize = 2 * sizeof(buffer[0]) * bufferSize;
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
	bufferSize = 0;

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
			//IndexType numVerticies = std::stoi(tokens.at(0));
			//IndexType numFaces = std::stoi(tokens.at(1));
			//IndexType numEdges = std::stoi(tokens.at(2));

			//buffer.resize(numVerticies);
			//indicies.resize(3*numFaces);
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
			vAdjs[bufferSize++] = VSet();

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

			Triangle t;
			t.v = {v0, v1, v2};
			t.f = {NULL_INDEX, NULL_INDEX, NULL_INDEX};

			faces[face_i++] = t;
		}
    }

    maxWidth = abs(xmax - xmin);
    maxHeight = abs(ymax - ymin);
    maxDepth = abs(zmax - zmin);
    center = vec3((xmin + xmax) / 2.0,
    			  (ymin + ymax) / 2.0,
    			  (zmin + zmax) / 2.0);

    // Create Face Adjaceny in Indexed Face list
	for (auto face : faces) {
		IndexType v0 = face.second.v[0];
		IndexType v1 = face.second.v[1];
		IndexType v2 = face.second.v[2];

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
			if (face.first != adjFace) {
				face.second.f[face_i++] = adjFace;
			}
		}
	}

	// Anticipate new verticies for edge collapse
	buffer.resize(2*bufferSize);

    setupBuffers();
    createVertexNormals();
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