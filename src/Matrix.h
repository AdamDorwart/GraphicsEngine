#pragma once
#include "Vector.h"

struct Mat4f {
	float m[4][4];

	Mat4f operator*(Mat4f w);
	Mat4f operator*(Vec3f v);
	Mat4f operator*(float s);

	Mat4f getTranspose();
	void transpose();
	Mat4f getInverse();
	void invert();
	Mat4f getRigidInverse();
	void rigidInvert();


}