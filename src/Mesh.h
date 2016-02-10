#pragma once

#include <string>
#include "Vector.h"

class Mesh {
	protected:
		unsigned int VAO;
		unsigned int buffers[3];
	public:
		enum { VERTEX_BUFFER = 0, NORMAL_BUFFER, INDEX_BUFFER, NUM_OF_BUFFERS};

		Mesh();
		~Mesh();

		void render();

		bool edgeCollapse(v1, v2) {
			v_n = v1.union(v2) - v1.intersect(v2);
			removeFaces(v1.intersect(v2));
			removeVertices(v1,v2);
			for (auto face : v_n) {
				// Replace old vertices with new
				for (auto vi : face.v) {
					if (face.v[0] == v1 || face.v[0] == v2) {
						face.v[vi] = v_n;
					}
				}
				// Fix face adj
				bool b = false;
				for (auto face_adj : face.f) {
					for (auto face_i : v1.intersect(v2)) {
						if (face_adj == face_i) {
							for (auto face_i_adj : face_i) {
								if (face_i_adj != face_adj) {
									for (auto face_k : v1.intersect(v2)) {
										if (face_i_adj != face_k) {
											// This face has been fixed, move to next one
											face.f[face_adj] = face_i_adj;
											b = true; break;
										}
									}
									if (b) break;
								}
							}
						}
						if (b) break;
					}
					if (b) break;
				}
			}
		}
}