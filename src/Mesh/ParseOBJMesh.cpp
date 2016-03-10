#include "ParseOBJMesh.h"
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include "Logger.h"
#include "Util.h"

using namespace Util;

bool ParseOBJMesh(Mesh* mesh, const char* filename) {
	// Read file
	std::ifstream infile(std::string(MODEL_PATH) + filename);
	std::string line;
	std::vector<std::string> tokens;
	std::vector<std::string> faceTokens;
	std::string token;

	IndexType face_i = 0;

	std::unordered_map<Datum, IndexType, DatumHasher> vertices;

	std::vector<vec3> positions;
	std::vector<vec3> normals;
	std::vector<vec2> texCoords;

	double xmin, xmax, ymin, ymax, zmin, zmax;

	xmin = ymin = zmin = std::numeric_limits<double>::max();
	xmax = ymax = zmax = std::numeric_limits<double>::min();

	if (!infile.is_open() || infile.bad() || !infile.good() || infile.fail()) {
		LogError("Error opening file [%s]\n", (std::string(MODEL_PATH) + filename).c_str());
		return false;
	}

	// Clear old mesh data
	mesh->buffer.clear();
	mesh->indices.clear();
	mesh->vAdjs.clear();
	mesh->faces.clear();

	while (std::getline(infile, line)) {
        trim(line);

		// Split line
		tokens.clear();
		split(line, ' ', tokens);

		// Ignore blank/comment line
		if (tokens.size() == 0 || tokens.at(0).compare("#") == 0)
			continue;

		if(tokens.at(0).compare("v") == 0) {
			//Parse the vertex line
			float x = std::stof(tokens.at(1));
			float y = std::stof(tokens.at(2));
			float z = std::stof(tokens.at(3));

			positions.push_back(vec3(x, y, z));
			LogInfo("p: (%f,%f,%f)\n", x,y,z);
		} else if(tokens.at(0).compare("vn") == 0) {
			float x = std::stof(tokens.at(1));
			float y = std::stof(tokens.at(2));
			float z = std::stof(tokens.at(3));

			normals.push_back(vec3(x, y, z));
			LogInfo("n: (%f,%f,%f)\n", x,y,z);
        } else if (tokens.at(0).compare("vt") == 0) {
			float u = std::stof(tokens.at(1));
			float v = std::stof(tokens.at(2));

			texCoords.push_back(vec2(u, v));
			LogInfo("t: (%f,%f)\n", u,v);
		} else if(tokens.at(0).compare("f") == 0) {
			const std::string delim = "/";
			int normPos, position, texCoord, normal;

			for (int i = 1; i <= 3; i++) {
				Datum vertex;

				faceTokens.clear();
				faceTokens = split(tokens.at(i), '/', faceTokens);

				//Check if vertex and normals exist
				if (faceTokens.size() == 3) {
					position = std::stoi(faceTokens.at(0));
					texCoord = std::stoi(faceTokens.at(1));
					normal = std::stoi(faceTokens.at(2));

					vertex.p = positions[position-1];
					vertex.n = normals[normal-1];
					vertex.t = texCoords[texCoord-1];
				} else if (faceTokens.size() == 2) {
					if (tokens.at(i).find("//") != std::string::npos) {
						position = std::stoi(faceTokens.at(0));
						normal = std::stoi(faceTokens.at(1));

						vertex.p = positions[position - 1];
						vertex.n = normals[normal - 1];
					} else {
						position = std::stoi(faceTokens.at(0));
						texCoord = std::stoi(faceTokens.at(1));

						vertex.p = positions[position - 1];
						vertex.t = texCoords[texCoord - 1];
					}
					
				} else {
					std::cout << "Unsupported data" << std::endl;
				}
				// Not handleing faces without texture and normals
				// TODO: Implement^

				vertex.v = 1.0;
				// Check if Vertex already exists, insert it if it doesn't
				auto loc = vertices.insert(std::pair<Datum, IndexType>(vertex, face_i));
				
				mesh->indices.push_back(loc.first->second);

				// Is this a new insert, increment the new index if so.
				if (loc.second) {
					face_i++;
				}

			}	
        }  
    }

    mesh->buffer.resize(2*vertices.size());

	for (auto it : vertices) {
		mesh->buffer[it.second] = it.first;
		LogInfo("%d: (%f,%f,%f)(%f,%f)\n", it.second, it.first.p[0],it.first.p[1],it.first.p[2],
			it.first.t[0],it.first.t[1]);
	}

	for (auto index : mesh->indices) {
		LogInfo("%d," , index);
	}

	mesh->setupBuffers();
	return true;
}