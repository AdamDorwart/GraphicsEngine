#include "Matrix.h"

Mat4f Mat4f::operator*(Mat4f a) {
	// Super inefficent
	Mat4f b;

	Vec4f row1(m[0][0], m[1][0], m[2][0], m[3][0]);
    Vec4f row2(m[0][1], m[1][1], m[2][1], m[3][1]);
    Vec4f row3(m[0][2], m[1][2], m[2][2], m[3][2]);
    Vec4f row4(m[0][3], m[1][3], m[2][3], m[3][3]);
    
    Vec4f col1(a.m[0][0], a.m[0][1], a.m[0][2], a.m[0][3]);
    Vec4f col2(a.m[1][0], a.m[1][1], a.m[1][2], a.m[1][3]);
    Vec4f col3(a.m[2][0], a.m[2][1], a.m[2][2], a.m[2][3]);
    Vec4f col4(a.m[3][0], a.m[3][1], a.m[3][2], a.m[3][3]);

    b.m[0][0] = row1.dot(col1); 
    b.m[0][1] = row2.dot(col1); 
    b.m[0][2] = row3.dot(col1); 
    b.m[0][3] = row4.dot(col1);
    b.m[1][0] = row1.dot(col2); 
    b.m[1][1] = row2.dot(col2); 
    b.m[1][2] = row3.dot(col2); 
    b.m[1][3] = row4.dot(col2);
    b.m[2][0] = row1.dot(col3); 
    b.m[2][1] = row2.dot(col3); 
    b.m[2][2] = row3.dot(col3); 
    b.m[2][3] = row4.dot(col3);
    b.m[3][0] = row1.dot(col4); 
    b.m[3][1] = row2.dot(col4); 
    b.m[3][2] = row3.dot(col4); 
    b.m[3][3] = row4.dot(col4);

    return b;
}

Mat4f Mat4f::Mat4f operator*(Vec3f v) {
	Vec4f b;
    
    b.x = m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z + m[3][0]*v.w;
    b.y = m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z + m[3][1]*v.w;
    b.z = m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z + m[3][2]*v.w;
    b.w = m[0][3]*v.x + m[1][3]*v.y + m[2][3]*v.z + m[3][3]*v.w;
    
    return b;
}

Mat4f Mat4f::Mat4f operator*(float s) {
	Matrix4 b;

	b.m[0][0] = m[0][0] * s;
	b.m[1][0] = m[1][0] * s;
	b.m[2][0] = m[2][0] * s;
	b.m[3][0] = m[3][0] * s;

	b.m[0][1] = m[0][1] * s;
	b.m[1][1] = m[1][1] * s;
	b.m[2][1] = m[2][1] * s;
	b.m[3][1] = m[3][1] * s;

	b.m[0][2] = m[0][2] * s;
	b.m[1][2] = m[1][2] * s;
	b.m[2][2] = m[2][2] * s;
	b.m[3][2] = m[3][2] * s;

	b.m[0][3] = m[0][3] * s;
	b.m[1][3] = m[1][3] * s;
	b.m[2][3] = m[2][3] * s;
	b.m[3][3] = m[3][3] * s;

	return b;
}

Mat4f Mat4f::Mat4f getTranspose() {
	Matrix4 b;
    for(int x = 0; x < 4; ++x)
    {
        for(int y = 0; y < 4; ++y)
        {
            b.m[y][x] = m[x][y];
        }
    }
    return b;
}

void Mat4f::Mat4f transpose() {
	Matrix4 b;
    for(int x = 0; x < 4; ++x)
    {
        for(int y = 0; y < 4; ++y)
        {
            b.m[y][x] = m[x][y];
        }
    }
    return b;
}

Mat4f Mat4f::Mat4f getInverse() {
	
}

void Mat4f::Mat4f invert() {
	
}

Mat4f Mat4f::Mat4f getRigidInverse() {
	
}

void Mat4f::Mat4f rigidInvert() {
	
}