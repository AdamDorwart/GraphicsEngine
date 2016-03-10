#include "Mesh.h"
#include "Logger.h"
#include "Util.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

// For debugging
#ifndef NDEBUG
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

using namespace Util;

Mesh::Mesh() {
	
}

// Copy constructor
Mesh::Mesh(const Mesh& m) {
	indices = m.indices;
	buffer = m.buffer;
	buffer.reserve(2*buffer.size());

	vAdjs = m.vAdjs;
	faces = m.faces;

	setupBuffers();
}

Mesh::~Mesh() {
	deleteBuffers();
}

void Mesh::draw() {
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indices.size(), IndexTypeGL, (void*)0);

	glBindVertexArray(0);
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
	indexBufferMaxSize = sizeof(indices[0]) * indices.size();

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

	glEnableVertexAttribArray(TEXCOORD_LOCATION);
	glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(2*sizeof(vec3)));

	glEnableVertexAttribArray(COLOR_LOCATION);
	glVertexAttribPointer(COLOR_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(2*sizeof(vec3)+sizeof(vec2)));

	glEnableVertexAttribArray(VISIBLE_LOCATION);
	glVertexAttribPointer(VISIBLE_LOCATION, 1, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(3*sizeof(vec3)+sizeof(vec2)));

	glBindVertexArray(0);

	// Upload the data
	updateVBO();
}

void Mesh::deleteBuffers() {
	if (bufferIds[0] != 0) {
		glDeleteBuffers(NUM_OF_BUFFERS, bufferIds);
	}

	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferMaxSize, &indices[0], GL_STREAM_DRAW);

	glBindVertexArray(0);
}

bool Mesh::parseFile(const char* filename) {
	std::vector<std::string> tokens;
	split(std::string(filename), '.', tokens);
	if (tokens.at(1).compare("off") == 0) {
		return ParseOFFMesh(this, filename);
	} else if (tokens.at(1).compare("obj") == 0) {
		return ParseOBJMesh(this, filename);
	} else {
		LogError("Unable to load %s as mesh: Unsupported file format\n", filename);
	}
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