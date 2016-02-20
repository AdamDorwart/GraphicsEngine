#pragma once

#include <string>
#include <set>
#include <unordered_map>
#include <array>
#include <vector>
#include <stack>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <limits>
#include "SceneGraph.h"

#ifndef MODEL_PATH
#define MODEL_PATH "../media/"
#endif

using namespace glm;

/* A Mesh is a type of object that contains a graph of vertices comprising of
 * triangle faces that describe a 3D geometry that can be rendered by
 * the OpenGL pipeline. It also defines various operations that can be performed
 * on itself like edge collapsing for mesh decimation.
 */

// Max # Verticies/Faces == # of positive numbers representable by IndexType
using IndexType = unsigned int; // Must use a default number type
static const GLenum IndexTypeGL = GL_UNSIGNED_INT; // This must match IndexType
static const IndexType NULL_INDEX = std::numeric_limits<IndexType>::max();

using VSet = std::set<IndexType>;
struct Triangle {
	std::array<IndexType,3> v; // Vertice indexes
	std::array<IndexType,3> f; // Shares edges with
};
struct EdgeDelta {
	std::array<IndexType,2> before;
	IndexType after;
};

struct Datum {
	vec3 p;  // Position
	vec3 n;  // Normal
	vec3 c;  // Color
	float v; // Visibility
};

class Mesh : public SceneNode {
	protected:
		//Storage vectors
		std::vector<IndexType> indicies;
		std::vector<Datum> buffer;
		IndexType bufferSize;

		double maxWidth, maxHeight, maxDepth;
		vec3 center;

		unsigned int VAO;
		unsigned int bufferIds[2];
		unsigned int dataBufferMaxSize;
		unsigned int indexBufferMaxSize;

		std::unordered_map<IndexType, VSet> vAdjs; // Vertex-Face Adj list
		std::unordered_map<IndexType, Triangle> faces; // Indexed Face list

		std::stack<EdgeDelta> collapseHistory;
	public:
		// Buffer IDs
		enum { DATA_BUFFER = 0, INDEX_BUFFER, NUM_OF_BUFFERS};
		// Layout location of data sent to shaders
		enum { POSITION_LOCATION = 0, NORMAL_LOCATION, COLOR_LOCATION, VISIBLE_LOCATION};

		Mesh();
		Mesh(const Mesh& m);
		~Mesh();

		virtual void draw();
		void drawEdge(IndexType v1, IndexType v2);

		bool parseOFF(const char* filename);

		bool pushEdgeCollapse(IndexType v1, IndexType v2);
		bool popEdgeCollapse();

		void createVertexNormals();

		double getMaxWidth();
		double getMaxHeight();
		double getMaxDepth();
		vec3 getCenter();

		void setupBuffers();

		void updateVBO();

};