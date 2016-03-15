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

MaterialShader* Mesh::m_materialShader = NULL;

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
	delete material;
	deleteBuffers();
}

void Mesh::setMaterialShader(MaterialShader* shader) {
	m_materialShader = shader;
}

void Mesh::draw() {
	glBindVertexArray(VAO);

	if (m_materialShader != NULL) {
		m_materialShader->bindMaterial(material);
	}

	glDrawElements(GL_TRIANGLES, indices.size(), IndexTypeGL, (void*)0);

	if (m_materialShader != NULL) {
		m_materialShader->unbindMaterial(material);
	}

	glBindVertexArray(0);
}

void Mesh::drawDebug(CoordFrame* frame) {

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

void Mesh::createVertexTangents() {
	for (IndexType i = 0; i < faces.size(); i++) {
		Triangle& face = faces[i];
		vec3 v0 = buffer[face.v[0]].p;
		vec3 v1 = buffer[face.v[1]].p;
		vec3 v2 = buffer[face.v[2]].p;

		vec2 uv0 = buffer[face.v[0]].t;
		vec2 uv1 = buffer[face.v[1]].t;
		vec2 uv2 = buffer[face.v[2]].t;

		vec3 deltaPos1 = v0 - v1;
		vec3 deltaPos2 = v0 - v2;
		vec2 deltaUV1  = uv0 - uv1;
		vec2 deltaUV2  = uv0 - uv2;

		float r = 1.0/(deltaUV1.x*deltaUV2.y - deltaUV1.y*deltaUV2.x);
		vec3 tangent = r * (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y);
		vec3 bitangent = r * (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x);

		for (int j = 0; j < 3; j++) {
			// Orthogonalization
			vec3 tangent_j = normalize(tangent - buffer[face.v[j]].n * dot(buffer[face.v[j]].n, tangent));
			// Fix handedness for symmetric models
			if (dot(cross(buffer[face.v[j]].n, tangent_j), bitangent) < 0.0f){
			    tangent_j = tangent_j * -1.0f;
			}
			buffer[face.v[0]].tn += tangent_j;
			buffer[face.v[0]].bn += bitangent;
		}
	}
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

	glEnableVertexAttribArray(TANGENT_LOCATION);
	glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(2*sizeof(vec3)));

	glEnableVertexAttribArray(BITANGENT_LOCATION);
	glVertexAttribPointer(BITANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(3*sizeof(vec3)));

	glEnableVertexAttribArray(TEXCOORD_LOCATION);
	glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Datum), (void*)(4*sizeof(vec3)));

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
		return false;
	}
}

void Mesh::setMaterial(Material* _material) {
	material = _material;
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