#pragma once
#include <cmath>
#include <string>

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

	inline operator std::string() {
		return "{"+(std::to_string(x))+
				","+(std::to_string(y))+
				","+(std::to_string(z))+"}";
	}
};

struct Vec4f
{
	float x;
	float y;
	float z;
	float w;

	Vec4f() {
	}

	Vec4f(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	inline void normalize() {
		float mag = 1./sqrt(x*x + y*y + z*z + w*w);
		x *= mag;
		y *= mag;
		z *= mag;
		w *= mag;
	}

	inline void dehomogenize() {
		float scale = 1./w;
		x *= scale;
		y *= scale;
		z *= scale;
		w = 1.0;
	}

	inline Vec4f operator + (Vec4f v) {
		return Vec4f(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	inline Vec4f operator - (Vec4f v) {
		return Vec4f(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	inline Vec4f operator * (float s) {
		return Vec4f(x * s, y * s, z * s, w * s);
	}

	inline float dot(Vec4f v) {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	inline float magnitude() {
		return sqrt(x*x + y*y + z*z + w*w);
	}

	inline operator std::string() {
		return "{"+(std::to_string(x))+
				","+(std::to_string(y))+
				","+(std::to_string(z))+
				","+(std::to_string(w))+"}";
	}
};

struct Vec2f
{
	float u;
	float v;

	Vec2f() {
	}

	Vec2f(float _u, float _v) {
		u = _u;
		v = _v;
	}

	void normalize() {
		float mag = 1./sqrt(u*u + v*v);
		u *= mag;
		v *= mag;
	}

	inline Vec2f operator + (Vec2f b) {
		return Vec2f(u + b.u, v + b.v);
	}

	inline Vec2f operator - (Vec2f b) {
		return Vec2f(u - b.u, v - b.v);
	}

	inline Vec2f operator * (float s) {
		return Vec2f(u * s, v * s);
	}

	inline float dot(Vec2f b) {
		return u * b.u + v * b.v;
	}

	inline float magnitude() {
		return sqrt(u*u + v*v);
	}

	inline operator std::string() {
		return "{"+(std::to_string(u))+
				","+(std::to_string(v))+"}";
	}
};