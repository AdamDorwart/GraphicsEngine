#include "ParseOBJMesh.h"
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Contract.h"
#include "Logger.h"
#include "Util.h"

using namespace Util;

bool ParseOBJMesh(Mesh* mesh, const char* filename) {
	// Read file
	std::ifstream infile(std::string(MODEL_PATH) + filename);
	std::string line;
	std::vector<std::string> tokens;
	std::vector<std::string> faceTokens;

	IndexType vertex_i = 0;
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

			Datum vertex[3];
			for (int i = 1; i <= 3; i++) {
				faceTokens.clear();
				faceTokens = split(tokens.at(i), '/', faceTokens);

				//Check if vertex and normals exist
				if (faceTokens.size() == 3) {
					position = std::stoi(faceTokens.at(0));
					texCoord = std::stoi(faceTokens.at(1));
					normal = std::stoi(faceTokens.at(2));

					vertex[i-1].p = positions[position-1];
					vertex[i-1].n = normals[normal-1];
					vertex[i-1].t = texCoords[texCoord-1];
				} else if (faceTokens.size() == 2) {
					if (tokens.at(i).find("//") != std::string::npos) {
						position = std::stoi(faceTokens.at(0));
						normal = std::stoi(faceTokens.at(1));

						vertex[i-1].p = positions[position - 1];
						vertex[i-1].n = normals[normal - 1];
					} else {
						position = std::stoi(faceTokens.at(0));
						texCoord = std::stoi(faceTokens.at(1));

						vertex[i-1].p = positions[position - 1];
						vertex[i-1].t = texCoords[texCoord - 1];
					}
					
				} else {
					std::cout << "Unsupported data" << std::endl;
				}
				// Not handleing faces without texture and normals
				// TODO: Implement^

				vertex[i-1].v = 1.0;
				// Check if Vertex already exists, insert it if it doesn't
				auto loc = vertices.insert(std::pair<Datum, IndexType>(vertex[i-1], vertex_i));
				
				mesh->indices.push_back(loc.first->second);

				// Is this a new insert, increment the new index if so.
				if (loc.second) {
					vertex_i++;
					mesh->vAdjs.push_back(VSet());
				} 
			}

			IndexType v0 = vertices[vertex[0]];
			IndexType v1 = vertices[vertex[1]];
			IndexType v2 = vertices[vertex[2]];

			mesh->vAdjs[v0].insert(face_i);
			mesh->vAdjs[v1].insert(face_i);
			mesh->vAdjs[v2].insert(face_i);
			face_i++;

			Triangle t;
			t.v = {v0, v1, v2};
			t.f = {NULL_INDEX, NULL_INDEX, NULL_INDEX};

			mesh->faces.push_back(t);
        }
    }

    mesh->maxWidth = abs(xmax - xmin);
    mesh->maxHeight = abs(ymax - ymin);
    mesh->maxDepth = abs(zmax - zmin);
    mesh->center = vec3((xmin + xmax) / 2.0,
    			  (ymin + ymax) / 2.0,
    			  (zmin + zmax) / 2.0);

    mesh->buffer.resize(2*vertices.size());
    mesh->vAdjs.reserve(2*vertices.size());

	for (auto it : vertices) {
		mesh->buffer[it.second] = it.first;
		LogInfo("%d: (%f,%f,%f)(%f,%f)\n", it.second, it.first.p[0],it.first.p[1],it.first.p[2],
			it.first.t[0],it.first.t[1]);
	}

#ifndef NDEBUG
	for (auto index : mesh->indices) {
		LogInfo("%d," , index);
	}
#endif

	mesh->setupBuffers();
	
    // Create Face Adjaceny in Indexed Face list
	for (IndexType i = 0; i < mesh->faces.size(); i++) {
		Triangle& face = mesh->faces[i];

		IndexType v0 = face.v[0];
		IndexType v1 = face.v[1];
		IndexType v2 = face.v[2];

		VSet v0Adj = mesh->vAdjs[v0];
		VSet v1Adj = mesh->vAdjs[v1];
		VSet v2Adj = mesh->vAdjs[v2];

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

		IndexType face_i = 0;
		// Set all the adjacent faces, ignoring the occurance of itself.
		for (auto adjFace : adjFaces) {
			if (i != adjFace) {
				face.f[face_i++] = adjFace;
			}
		}
	}

	return true;
}

bool ParseMTLMaterial(std::unordered_map<std::string, Material*>& materials, const char* filename) {
	// Read file
	std::ifstream infile(std::string(MATERIAL_PATH) + filename);
	std::string line;
	std::vector<std::string> tokens;
	std::vector<std::string> faceTokens;

	Material* currentMaterial = NULL;

	EnsuresMsg(infile.is_open() && !infile.bad() && infile.good() && !infile.fail(),
			  "Error opening file [%s]\n", (std::string(MATERIAL_PATH) + filename).c_str());

	while (std::getline(infile, line)) {
        trim(line);

		// Split line
		tokens.clear();
		split(line, ' ', tokens);

		// Ignore blank/comment line
		if (tokens.size() == 0 || tokens.at(0).compare("#") == 0)
			continue;

		if (tokens.at(0).compare("newmtl") == 0) {
			// Set current material
			currentMaterial = materials[tokens.at(1)] = new Material();
		} else if (tokens.at(0).compare("Ka") == 0) {
			// Ambient Color
			float r = std::stof(tokens.at(1));
			float g = std::stof(tokens.at(2));
			float b = std::stof(tokens.at(3));

			EnsuresMsg(currentMaterial != NULL,
					  "Error parsing material file [%s]: malformed file.\n", filename);

			currentMaterial->setAmbient(vec3(r, g, b));
		} else if (tokens.at(0).compare("Kd") == 0) {
			// Diffuse Color
			float r = std::stof(tokens.at(1));
			float g = std::stof(tokens.at(2));
			float b = std::stof(tokens.at(3));

			EnsuresMsg(currentMaterial != NULL,
					  "Error parsing material file [%s]: malformed file.\n", filename);

			currentMaterial->setDiffuse(vec3(r, g, b));
		} else if (tokens.at(0).compare("Ks") == 0) {
			// Diffuse Color
			float r = std::stof(tokens.at(1));
			float g = std::stof(tokens.at(2));
			float b = std::stof(tokens.at(3));

			EnsuresMsg(currentMaterial != NULL,
					  "Error parsing material file [%s]: malformed file.\n", filename);

			currentMaterial->setSpecular(vec3(r, g, b));
		} else if (tokens.at(0).compare("Ns") == 0) {
			// Specular Exponent
			float exponent = std::stof(tokens.at(1));

			EnsuresMsg(currentMaterial != NULL,
					  "Error parsing material file [%s]: malformed file.\n", filename);

			currentMaterial->setSpecularExp(exponent);
		} else if (tokens.at(0).compare("map_Ka") == 0) {
			// Ambient map
			EnsuresMsg(currentMaterial != NULL,
					  "Error parsing material file [%s]: malformed file.\n", filename);

			currentMaterial->loadAmbient(tokens.at(1).c_str());
		} else if (tokens.at(0).compare("map_Kd") == 0) {
			// Diffuse map
			EnsuresMsg(currentMaterial != NULL,
					  "Error parsing material file [%s]: malformed file.\n", filename);

			currentMaterial->loadDiffuse(tokens.at(1).c_str());
		} else if (tokens.at(0).compare("map_Ks") == 0) {
			// Specular map
			EnsuresMsg(currentMaterial != NULL,
					  "Error parsing material file [%s]: malformed file.\n", filename);

			currentMaterial->loadSpecular(tokens.at(1).c_str());
		} else if (tokens.at(0).compare("map_Bump") == 0 || tokens.at(0).compare("bump") == 0) {
			// Bump map
			EnsuresMsg(currentMaterial != NULL,
					  "Error parsing material file [%s]: malformed file.\n", filename);

			currentMaterial->loadBump(tokens.at(1).c_str());
		} else if (tokens.at(0).compare("refl") == 0) {
			// Reflection map

			EnsuresMsg(currentMaterial != NULL,
					  "Error parsing material file [%s]: malformed file.\n", filename);

			currentMaterial->loadReflection(tokens.at(1).c_str());
		}
	}

	return true;
}

bool ParseOBJMeshMaterial(std::unordered_map<std::string, Mesh>& meshes, const char* filename) {
	// Read file
	std::ifstream infile(std::string(MODEL_PATH) + filename);
	std::string line;
	std::vector<std::string> tokens;
	std::vector<std::string> faceTokens;

	std::vector<vec3> positions;
	std::vector<vec3> normals;
	std::vector<vec2> texCoords;

	std::unordered_map<Datum, IndexType, DatumHasher> vertices;

	IndexType vertex_i = 0;
	IndexType face_i = 0;
	double xmin, xmax, ymin, ymax, zmin, zmax;

	xmin = ymin = zmin = std::numeric_limits<double>::max();
	xmax = ymax = zmax = std::numeric_limits<double>::min();

	Mesh* currentMesh = NULL;
	Material* currentMaterial = NULL;

	std::unordered_map<std::string, Material*> materials;

	EnsuresMsg(infile.is_open() && !infile.bad() && infile.good() && !infile.fail(),
			  "Error opening file [%s]\n", (std::string(MODEL_PATH) + filename).c_str());

	while (std::getline(infile, line)) {
        trim(line);

		// Split line
		tokens.clear();
		split(line, ' ', tokens);

		// Ignore blank/comment line
		if (tokens.size() == 0 || tokens.at(0).compare("#") == 0)
			continue;

		if (tokens.at(0).compare("mtllib") == 0) {
			// Load new Material Library
			EnsuresMsg(ParseMTLMaterial(materials, tokens.at(1).c_str()),
					  "Unable to load material file %s\n", tokens.at(1).c_str());

		} else if (tokens.at(0).compare("usemtl") == 0) {
			// Set current material
			EnsuresMsg(materials.size() != 0,
					  "Unable to parse OBJ: No materials loaded to use.\n");
			
			currentMaterial = materials[tokens.at(1)];
			if (currentMesh != NULL) {
				currentMesh->setMaterial(currentMaterial);
			}
		} else if (tokens.at(0).compare("o") == 0 || tokens.at(0).compare("g") == 0) {
			// New Mesh
			if (currentMesh != NULL) {
				// Finalize previous mesh
				currentMesh->maxWidth = abs(xmax - xmin);
				currentMesh->maxHeight = abs(ymax - ymin);
				currentMesh->maxDepth = abs(zmax - zmin);
				currentMesh->center = vec3((xmin + xmax) / 2.0,
							  (ymin + ymax) / 2.0,
							  (zmin + zmax) / 2.0);

				currentMesh->buffer.resize(2*vertices.size());
				currentMesh->vAdjs.reserve(2*vertices.size());

				for (auto it : vertices) {
					currentMesh->buffer[it.second] = it.first;
					LogInfo("%d: (%f,%f,%f)(%f,%f)\n", it.second, it.first.p[0],it.first.p[1],it.first.p[2],
						it.first.t[0],it.first.t[1]);
				}

				// Create Face Adjaceny in Indexed Face list
				for (IndexType i = 0; i < currentMesh->faces.size(); i++) {
					Triangle& face = currentMesh->faces[i];

					IndexType v0 = face.v[0];
					IndexType v1 = face.v[1];
					IndexType v2 = face.v[2];

					VSet v0Adj = currentMesh->vAdjs[v0];
					VSet v1Adj = currentMesh->vAdjs[v1];
					VSet v2Adj = currentMesh->vAdjs[v2];

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

					IndexType face_i = 0;
					// Set all the adjacent faces, ignoring the occurance of itself.
					for (auto adjFace : adjFaces) {
						if (i != adjFace) {
							face.f[face_i++] = adjFace;
						}
					}
				}

				currentMesh->createVertexTangents();
				currentMesh->setupBuffers();

				vertices.clear();
				vertex_i = 0;
				face_i = 0;

				xmin = ymin = zmin = std::numeric_limits<double>::max();
				xmax = ymax = zmax = std::numeric_limits<double>::min();
			}
			currentMesh = &meshes[tokens.at(1)];
			if (currentMaterial != NULL) {
				currentMesh->setMaterial(currentMaterial);
			}
		} else if(tokens.at(0).compare("v") == 0) {
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

			std::array<Datum,3> vertex;
			for (int i = 1; i <= 3; i++) {
				faceTokens.clear();
				faceTokens = split(tokens.at(i), '/', faceTokens);

				//Check if vertex and normals exist
				if (faceTokens.size() == 3) {
					position = std::stoi(faceTokens.at(0));
					texCoord = std::stoi(faceTokens.at(1));
					normal = std::stoi(faceTokens.at(2));

					vertex[i-1].p = positions[position-1];
					vertex[i-1].n = normals[normal-1];
					vertex[i-1].t = texCoords[texCoord-1];
				} else if (faceTokens.size() == 2) {
					if (tokens.at(i).find("//") != std::string::npos) {
						position = std::stoi(faceTokens.at(0));
						normal = std::stoi(faceTokens.at(1));

						vertex[i-1].p = positions[position - 1];
						vertex[i-1].n = normals[normal - 1];
					} else {
						position = std::stoi(faceTokens.at(0));
						texCoord = std::stoi(faceTokens.at(1));

						vertex[i-1].p = positions[position - 1];
						vertex[i-1].t = texCoords[texCoord - 1];
					}
					
				} else {
					std::cout << "Unsupported data" << std::endl;
				}
				// Not handleing faces without texture and normals
				// TODO: Implement^

				vertex[i-1].v = 1.0;
				// Check if Vertex already exists, insert it if it doesn't
				auto loc = vertices.insert(std::pair<Datum, IndexType>(vertex[i-1], vertex_i));
				
				currentMesh->indices.push_back(loc.first->second);

				// Is this a new insert, increment the new index if so.
				if (loc.second) {
					vertex_i++;
					currentMesh->vAdjs.push_back(VSet());
				}
			}

			IndexType v0 = vertices[vertex[0]];
			IndexType v1 = vertices[vertex[1]];
			IndexType v2 = vertices[vertex[2]];

			currentMesh->vAdjs[v0].insert(face_i);
			currentMesh->vAdjs[v1].insert(face_i);
			currentMesh->vAdjs[v2].insert(face_i);
			face_i++;

			Triangle t;
			t.v = {v0, v1, v2};
			t.f = {NULL_INDEX, NULL_INDEX, NULL_INDEX};

			currentMesh->faces.push_back(t);
        }
    }

    if (currentMesh == NULL) {
    	currentMesh = &meshes["Object"];
    }

	// Finalize previous mesh
	currentMesh->maxWidth = abs(xmax - xmin);
	currentMesh->maxHeight = abs(ymax - ymin);
	currentMesh->maxDepth = abs(zmax - zmin);
	currentMesh->center = vec3((xmin + xmax) / 2.0,
				  (ymin + ymax) / 2.0,
				  (zmin + zmax) / 2.0);

	currentMesh->buffer.resize(2*vertices.size());
	currentMesh->vAdjs.reserve(2*vertices.size());

	for (auto it : vertices) {
		currentMesh->buffer[it.second] = it.first;
		LogInfo("%d: (%f,%f,%f)(%f,%f)\n", it.second, it.first.p[0],it.first.p[1],it.first.p[2],
			it.first.t[0],it.first.t[1]);
	}

    // Create Face Adjaceny in Indexed Face list
	for (IndexType i = 0; i < currentMesh->faces.size(); i++) {
		Triangle& face = currentMesh->faces[i];

		IndexType v0 = face.v[0];
		IndexType v1 = face.v[1];
		IndexType v2 = face.v[2];

		VSet v0Adj = currentMesh->vAdjs[v0];
		VSet v1Adj = currentMesh->vAdjs[v1];
		VSet v2Adj = currentMesh->vAdjs[v2];

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

		IndexType face_i = 0;
		// Set all the adjacent faces, ignoring the occurance of itself.
		for (auto adjFace : adjFaces) {
			if (i != adjFace) {
				face.f[face_i++] = adjFace;
			}
		}
	}

	currentMesh->createVertexTangents();
	currentMesh->setupBuffers();

	return true;

}