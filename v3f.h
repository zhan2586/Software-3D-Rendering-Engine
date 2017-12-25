#pragma once
#include <vector>
#include <iostream>
using namespace std;
//#include "m33.h"

class V3f
{
public:
	float xyz[3];
	V3f() {};
	V3f(float x, float y, float z);
	float& operator[](int i);
	V3f operator+(V3f v);
	V3f operator-(V3f v);
	V3f operator*(float s);
	V3f operator/(float s);
	bool operator==(V3f v);
	friend ostream& operator<<(ostream &os, V3f v);
	float dot(V3f v);
	V3f cross(V3f v);
	float length();
	V3f normalize();
	V3f RotateAround(V3f axis, float theta);
	unsigned int V3fToHex();
	void HexToV3f(unsigned int hex);
	V3f RotatePointAboutAxis(V3f o, V3f axis, float theta);

private:

};