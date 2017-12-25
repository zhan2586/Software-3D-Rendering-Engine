#include "v3f.h"

#include "m33.h"

inline V3f::V3f(float x, float y, float z) : V3f() {
	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = z;
}

float& V3f::operator[](int i) {
	return xyz[i];
}

V3f V3f::operator+(V3f v) {
	return V3f(xyz[0] + v[0], xyz[1] + v[1], xyz[2] + v[2]);
}

V3f V3f::operator-(V3f v) {
	return V3f(xyz[0] - v[0], xyz[1] - v[1], xyz[2] - v[2]);
}

V3f V3f::operator*(float s) {
	return V3f(xyz[0] * s, xyz[1] * s, xyz[2] * s);
}

V3f V3f::operator/(float s) {
	return V3f(xyz[0] / s, xyz[1] / s, xyz[2] / s);
}

float V3f::dot(V3f v) {
	return (xyz[0] * v[0] + xyz[1] * v[1] + xyz[2] * v[2]);
}

V3f V3f::cross(V3f v) {
	return V3f(xyz[1] * v[2] - v[1] * xyz[2],
				xyz[2] * v[0] - v[2] * xyz[0], 
				xyz[0] * v[1] - v[0] * xyz[1]);
}

float V3f::length() {
	return sqrtf(this->dot(*this));
}

V3f V3f::normalize() {
	float s = this->length();
	return ((*this) / s);
}

bool V3f::operator==(V3f v) {
	if (xyz[0] == v[0] && xyz[0] == v[1] && xyz[0] == v[2])	return true;
	else return false;
}

ostream& operator<<(ostream &os, V3f v) {
	os << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
	return os;
}

V3f V3f::RotateAround(V3f axis, float theta) {
	V3f v = *this;
	float cost = cosf(theta), sint = sinf(theta);
	if (axis == V3f(1.f, 0.f, 0.f))	return V3f(v[0], v[1] * cost + v[2] * sint, v[2] * cost - v[1] * sint);
	if (axis == V3f(0.f, 1.f, 0.f))	return V3f(v[0] * cost - v[2] * sint, v[1], v[0] * sint + v[2] * cost);
	if (axis == V3f(0.f, 0.f, 1.f))	return V3f(v[0] * cost + v[1] * sint, v[1] * cost - v[0] * sint, v[2]);
	return v;
}

unsigned int V3f::V3fToHex() {
	V3f &toconvert = *this;
	return ((int)(toconvert[0] * 255)) << 16 | ((int)(toconvert[1] * 255)) << 8 | (int)(toconvert[2] * 255);
}

void V3f::HexToV3f(unsigned int hex) {
	this->xyz[0] = (hex >> 16) / 255.f;
	this->xyz[1] = ((hex >> 8) & 0x00ff) / 255.f;
	this->xyz[2] = (hex & 0x0000ff) / 255.f;
}

V3f V3f::RotatePointAboutAxis(V3f O, V3f a, float theta) {
	V3f aux;
	a = a.normalize();
	if (fabsf(V3f(1.0f, 0.0f, 0.0f).dot(a)) < fabsf(V3f(0.0f, 1.0f, 0.0f).dot(a)))
		aux = V3f(1.0f, 0.0f, 0.0f);
	else
		aux = V3f(0.0f, 1.0f, 0.0f);
	V3f c = (a.cross(aux)).normalize();
	V3f b = (a.cross(c)).normalize();

	// (c, a, b) local coordinate system
	V3f &P = *this;
	M33 cs;
	cs[0] = c;
	cs[1] = a;
	cs[2] = b;
	V3f PL = cs * (P - O);
	M33 rot;
	rot.SetRotationAboutY(theta);
	V3f PLR = rot * PL;
	M33 dbg = cs.Inversion();
	V3f PR = dbg * PLR + O;

	return PR;
}