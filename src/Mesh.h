#pragma once

#include <string>
#include <set>
#include <unordered_map>
#include <array>
#include "Vector.h"

template <typename T>
struct Triangle {
	std::array<T,3> v; // Vertice indexes
	std::array<T,3> f; // Shares edges with
};
// Max # Verticies/Faces == sizeof(IndexType)
using IndexType = int; // Must use a default number type
using VSet = std::set<IndexType>;

class Mesh {
	protected:
		unsigned int VAO;
		unsigned int buffers[3];

		std::unordered_map<IndexType, VSet> vAdjs; // Vertex-Adj list
		std::unordered_map<IndexType, Triangle> faces; // Indexed Face list
	public:
		enum { VERTEX_BUFFER = 0, NORMAL_BUFFER, INDEX_BUFFER, NUM_OF_BUFFERS};

		Mesh();
		~Mesh();

		void render();

		bool edgeCollapse();

}