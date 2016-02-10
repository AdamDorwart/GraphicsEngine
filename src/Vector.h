#pragma once
#include <math>

struct Vec3f
{
	float x;
	float y;
	float z;

	Vec3f() {
	}

	Vec3f(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	inline void normalize() {
		float mag = 1./sqrt(x*x + y*y + z*z);
		x *= mag;
		y *= mag;
		z *= mag;
	}

	inline Vec3f operator + (Vec3f v) {
		return Vec3f(x + v.x, y + v.y, z + v.z);
	}

	inline Vec3f operator - (Vec3f v) {
		return Vec3f(x - v.x, y - v.y, z - v.z);
	}

	inline Vec3f operator * (float s) {
		return Vec3f(x * s, y * s, z * s);
	}

	inline float dot(Vec3f v) {
		return x * v.x + y * v.y + z * v.z;
	}

	inline Vec3f cross(Vec3f v) {
		return Vec3f(y * v.z - z * v.y,
					 z * v.x - x * v.z,
					 x * v.y - y * v.x);
	}

	inline float magnitude() {
		return sqrt(x*x + y*y + z*z);
	}
};

struct Vec2f
{
	float u;
	float v;

	Vector2f() {
	}

	Vector2f(float _u, float _v) {
		u = _u;
		v = _v;
	}

	void normalize() {
		float mag = 1./sqrt(u*u + v*v);
		u *= mag;
		v *= mag;
	}

	inline Vec2f operator + (Vec2f v) {
		return Vec2f(u + v.u, v + v.v);
	}

	inline Vec2f operator - (Vec2f v) {
		return Vec2f(u - v.u, v - v.v);
	}

	inline Vec2f operator * (float s) {
		return Vec2f(u * s, v * s);
	}

	inline float dot(Vec2f v) {
		return u * v.u + v * v.v;
	}

	inline float magnitude() {
		return sqrt(u*u + v*v);
	}
};