#pragma once

#include <string>
#include <set>
#include <unordered_map>
#include <array>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Vector.h"

#ifndef MODEL_PATH
#define MODEL_PATH std::string("../media/")
#endif

/* A Mesh is a type of object that contains a graph of vertices comprimising
 * triangle faces that describe a 3D geometry that can be rendered by
 * the OpenGL pipeline. It also defines various operations that can be performed
 * on itself like edge collapsing for mesh decimation.
 */

// Max # Verticies/Faces == # of positive numbers representable by IndexType
using IndexType = unsigned int; // Must use a default number type
static const GLenum IndexTypeGL = GL_UNSIGNED_INT; // This must match

struct Triangle {
	std::array<IndexType,3> v; // Vertice indexes
	std::array<IndexType,3> f; // Shares edges with
};
using VSet = std::set<IndexType>;

struct Datum {
	Vec3f p; // Position
	Vec3f n; // Normal
	Vec3f c; // Color
};

class Mesh {
	protected:
		//Storage vectors
		std::vector<IndexType> indicies;
		std::vector<Datum> buffer;

		unsigned int VAO;
		unsigned int bufferIds[2];
		unsigned int dataBufferMaxSize;
		unsigned int indexBufferMaxSize;

		std::unordered_map<IndexType, VSet> vAdjs; // Vertex-Adj list
		std::unordered_map<IndexType, Triangle> faces; // Indexed Face list
	public:
		// Buffer IDs
		enum { DATA_BUFFER = 0, INDEX_BUFFER, NUM_OF_BUFFERS};
		// Layout location of data sent to shaders
		enum { POSITION_LOCATION = 0, NORMAL_LOCATION, COLOR_LOCATION};

		Mesh();
		~Mesh();

		void render();

		bool parseOFF(const char* filename);

		bool edgeCollapse(IndexType v1, IndexType v2);

		void setupBuffers();

		void updateVBO();

};