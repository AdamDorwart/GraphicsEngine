#include "Mesh.h"
#include "Logger.h"
#include "Util.h"
#include <algorithm>
#include <fstream>

using namespace Util;


Mesh::Mesh() {
	
}

Mesh::~Mesh() {
	
}

void Mesh::render() {
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indicies.size(), IndexTypeGL,(void*)0);

	glBindVertexArray(0);
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
	VSet vN(*v1Adj);
	vN.insert(v2Adj->begin(), v2Adj->end());
	vN.erase(removedFaces.begin(), removedFaces.end());

	// Remove old vertices, add new
	// O(2) ; Constant
	vAdjs.erase(v1);
	vAdjs.erase(v2);
	auto insertResult = vAdjs.emplace(v1, vN);
	if (!insertResult.second) {
		// Insert failed - Something bad happend
		Logger::err("Unable to insert new vertex, this shouldn't happen. Verify edgeCollapse algorithm.\n");
		return false;
	}
	Datum newData;
	newData.p.x = (buffer[v1].p.x + buffer[v2].p.x)/2.0;
	newData.p.y = (buffer[v1].p.y + buffer[v2].p.y)/2.0;
	newData.p.z = (buffer[v1].p.z + buffer[v2].p.z)/2.0;
	buffer[v1] = newData;

	// Store iterator to new vertex inside vAdjs
	auto vNit = insertResult.first;
	IndexType vNindex = vNit->first;
	VSet* vNptr = &(vNit->second);

	// Iterate through all faces that use the new vertex
	for (IndexType faceInd : *vNptr) {
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
			for (IndexType face_i : removedFaces) {
				// if current face is adjacent to a removed face
				if (face_adj == face_i) {
					// We need to find the new adjacent face
					// Iterate through the removed face's adjacent faces
					for (IndexType& face_i_adj : faces[face_i].f) {
						// Current face can't be adjacent to itself
						if (face_i_adj != faceInd) {
							// Also can't be adjacent to another removed face
							for (IndexType face_k : removedFaces) {
								if (face_i_adj != face_k) {
									// We've found an face adjacent to the removed
									// face that our current face must now be adjacent
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
	for (IndexType face : removedFaces) {
		faces.erase(face);
	}

	// Upload the mesh data to the graphics driver
	updateVBO();

	return true;
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
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)sizeof(Vec3f));

	glEnableVertexAttribArray(COLOR_LOCATION);
	glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(2*sizeof(Vec3f)));

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
	std::ifstream infile((MODEL_PATH + filename));
    std::string line;
    std::vector<std::string> tokens;
	std::vector<std::string> faceTokens;
    std::string token;
    bool hasReadSize = false;

    if (!infile.is_open() || infile.bad() || !infile.good() || infile.fail()) {
    	Logger::err("Error opening file [%s]", (MODEL_PATH + filename).c_str());
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

			d.c.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			d.c.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			d.c.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			buffer.push_back(d);
		} else if (tokens.size() == 4) {
			// Reading face
			if (tokens.at(0).compare("3") != 0) {
				Logger::err("Error parsing %s: file format uses non-triangle faces (%s)\n", filename, line);
				return false;
			}
			indicies.push_back(std::stoi(tokens.at(1)));
			indicies.push_back(std::stoi(tokens.at(2)));
			indicies.push_back(std::stoi(tokens.at(3)));
		}
    }

    setupBuffers();
    return true;
}