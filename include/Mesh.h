#pragma once

#include <set>
#include <array>
#include <vector>
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

struct Datum {
	vec3 p;  // Position
	vec3 n;  // Normal
	vec2 t;  // Texture Coords
	vec3 c;  // Color
	float v; // Visibility

	bool operator== (const Datum& d) const {
		// Float equality tests. Very bad if math is being performed
		// Usually okay because we're usually checking if values are same
		// at load time.
		if (p[0] == d.p[0] && p[1] == d.p[1] && p[2] == d.p[2]) {
			return true;
		}
	}
};

struct DatumHasher {
    size_t operator()(const Datum& d) const {
        std::hash<float> hasher;
        size_t seed = 0;
        seed ^= hasher(d.p[0]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(d.p[1]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(d.p[2]) + 0x9e3779b9 + (seed<<6) + (seed>>2);

        seed ^= hasher(d.n[0]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(d.n[1]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(d.n[2]) + 0x9e3779b9 + (seed<<6) + (seed>>2);

        seed ^= hasher(d.t[0]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(d.t[1]) + 0x9e3779b9 + (seed<<6) + (seed>>2);

        seed ^= hasher(d.c[0]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(d.c[1]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(d.c[2]) + 0x9e3779b9 + (seed<<6) + (seed>>2);

        seed ^= hasher(d.v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        return seed;
    }
};

class Mesh;
class MeshSimplifier;
bool ParseOFFMesh(Mesh*, const char*);
bool ParseOBJMesh(Mesh*, const char*);
class Mesh : public SceneNode {
	friend class MeshSimplifier;
	friend bool ParseOFFMesh(Mesh*, const char*);
	friend bool ParseOBJMesh(Mesh*, const char*);
	protected:
		//Storage vectors
		std::vector<IndexType> indices;
		std::vector<Datum> buffer;

		double maxWidth, maxHeight, maxDepth;
		vec3 center;

		unsigned int VAO;
		unsigned int bufferIds[2];
		unsigned int dataBufferMaxSize;
		unsigned int indexBufferMaxSize;

		std::vector<VSet> vAdjs; // Vertex-Face Adj list
		std::vector<Triangle> faces; // Indexed Face list
	public:
		// Buffer IDs
		enum { DATA_BUFFER = 0, INDEX_BUFFER, NUM_OF_BUFFERS};
		// Layout location of data sent to shaders
		enum { POSITION_LOCATION = 0, NORMAL_LOCATION, TEXCOORD_LOCATION, COLOR_LOCATION, VISIBLE_LOCATION};

		Mesh();
		Mesh(const Mesh& m);
		~Mesh();

		virtual void draw();
		void drawEdge(IndexType v1, IndexType v2);
		
		void createVertexNormals();

		double getMaxWidth();
		double getMaxHeight();
		double getMaxDepth();
		vec3 getCenter();

		void setupBuffers();
		void deleteBuffers();

		bool parseFile(const char* filename);

		void updateVBO();

		IndexType getNumberVerticies();

};