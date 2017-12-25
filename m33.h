#pragma once
#include <iostream>
#include <vector>
using namespace std;

#include "v3f.h"

class M33
{
public:
	V3f m33[3];
	M33() {};
	V3f& operator[](int i);
	V3f operator*(V3f v);
	M33 operator*(M33 m);
	M33 Inversion();
	M33 Transpositon();
	M33 Rotation(V3f axis, float theta);
	V3f GetCol(int i);
	void SetCol(int i, V3f v);
	friend ostream& operator<<(ostream &os, M33 m);
	void SetRotationAboutY(float theta);

private:

};