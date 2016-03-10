#pragma once
#include "Mesh.h"
#include <set>
#include <vector>
#include <stack>
#include <glm/glm.hpp>

using namespace glm;

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

class MeshSimplifier {
	private:
		Mesh* mesh;

		std::stack<EdgeDelta> collapseHistory;
		std::vector<mat4> vertexQ; // Q error matrix for each vertex
		VPEheap vertexErrors; // Sorts the pairs by error
		// Links a vertex to all of it's member pairs in the VPEheap
		std::vector<std::set<VPEheap::iterator, VPEheapItComp>> vpeLinks;

		void printVPElinks(bool showV);
		void printVPE(bool showError);

		vec3 collapseVertices(IndexType v0, IndexType v1);
		void initializeQuadricError(float threshold);
		VPEheap::iterator insertVPE(IndexType v0, IndexType v1);

	public:
		MeshSimplifier(Mesh* mesh);
		~MeshSimplifier();

		IndexType pushEdgeCollapse(IndexType v1, IndexType v2, bool updateVbo=true);
		bool popEdgeCollapse(bool updateVbo=true);
		void quadricSimplifyStep(bool updateVbo=true);
};