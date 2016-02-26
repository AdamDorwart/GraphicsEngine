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
#include <cfloat>

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

struct ValidPairError {
	float error;
	std::array<IndexType,2> v;
	inline bool operator<(const ValidPairError& rhs) const {
		if (!(error < rhs.error) && !(rhs.error < error)) {
			return std::lexicographical_compare(v.begin(),v.end(),
        								   		rhs.v.begin(),rhs.v.end());
		} else {
			return error < rhs.error;
		}
	}
};

using VPEheap = std::set<ValidPairError>;

struct VPEheapItComp {
	bool operator() (const VPEheap::iterator& lhs, const VPEheap::iterator& rhs) const {
      	return &(*lhs) < &(*rhs);
    }
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

		double maxWidth, maxHeight, maxDepth;
		vec3 center;

		unsigned int VAO;
		unsigned int bufferIds[2];
		unsigned int dataBufferMaxSize;
		unsigned int indexBufferMaxSize;

		std::vector<VSet> vAdjs; // Vertex-Face Adj list
		std::vector<Triangle> faces; // Indexed Face list
 		// Links a vertex to all of it's member pairs in the VPEheap
		std::vector<std::set<VPEheap::iterator, VPEheapItComp>> vpeLinks;

		std::stack<EdgeDelta> collapseHistory;
		std::vector<mat4> vertexQ; // Q error matrix for each vertex
		VPEheap vertexErrors; // Sorts the pairs by error

		void printVPElinks(bool showV);
		void printVPE(bool showError);

		vec3 collapseVertices(IndexType v0, IndexType v1);
		void initializeQuadricError(float threshold);
		VPEheap::iterator insertVPE(IndexType v0, IndexType v1);

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

		IndexType pushEdgeCollapse(IndexType v1, IndexType v2, bool updateVbo=true);
		bool popEdgeCollapse(bool updateVbo=true);
		void quadricSimplifyStep(bool updateVbo=true);

		void createVertexNormals();

		double getMaxWidth();
		double getMaxHeight();
		double getMaxDepth();
		vec3 getCenter();

		void setupBuffers();

		void updateVBO();

		IndexType getNumberVerticies();

};