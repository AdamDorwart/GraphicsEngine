#include "ParseOFFMesh.h"
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Logger.h"
#include "Util.h"

using namespace Util;

bool ParseOFFMesh(Mesh* mesh, const char* filename) {
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
		LogError("Error opening file [%s]\n", (std::string(MODEL_PATH) + filename).c_str());
		return false;
	}

	// Clear old mesh data
	mesh->buffer.clear();
	mesh->indices.clear();
	mesh->vAdjs.clear();
	mesh->faces.clear();

	// Check if header string is valid
	std::getline(infile, line);
	if (line.find("OFF") == std::string::npos) {
		LogError("%s: file does not follow the OFF specification\n", filename);
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

			mesh->vAdjs.reserve(2*numVerticies);
			mesh->faces.reserve(numFaces);
			mesh->buffer.reserve(2*numVerticies);
			mesh->indices.reserve(3*numFaces);
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

			/*
			d.c.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			d.c.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			d.c.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			*/
			d.v = 1.0; // Visible

			mesh->buffer.push_back(d);
			mesh->vAdjs.push_back(VSet());

		} else if (tokens.size() == 4) {
			// Reading face
			if (tokens.at(0).compare("3") != 0) {
				LogError("Error parsing %s: file format uses non-triangle faces (%s)\n", filename, line.c_str());
				return false;
			}
			IndexType v0 = std::stoi(tokens.at(1));
			IndexType v1 = std::stoi(tokens.at(2));
			IndexType v2 = std::stoi(tokens.at(3));

			mesh->indices.push_back(v0);
			mesh->indices.push_back(v1);
			mesh->indices.push_back(v2);

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

		face_i = 0;
		// Set all the adjacent faces, ignoring the occurance of itself.
		for (auto adjFace : adjFaces) {
			if (i != adjFace) {
				face.f[face_i++] = adjFace;
			}
		}
	}

#ifndef NDEBUG
	for (IndexType i = 0; i < mesh->faces.size(); i++) {
		LogInfo("faces[%d]: {%d,%d,%d}{%d,%d,%d}\n",i,mesh->faces[i].v[0],
			mesh->faces[i].v[1],mesh->faces[i].v[2],mesh->faces[i].f[0],mesh->faces[i].f[1],mesh->faces[i].f[2]);
	}

	for (IndexType i = 0; i < mesh->vAdjs.size(); i++) {
		LogInfo("vAdjs[%d]:{",i);
		for (auto face : mesh->vAdjs[i]) {
			LogInfo("%d,",face);
		}
		LogInfo("}\n");
	}
#endif

	LogInfo("Num of faces: %d\n", mesh->faces.size());

    mesh->setupBuffers();
    mesh->createVertexNormals();
    return true;
}