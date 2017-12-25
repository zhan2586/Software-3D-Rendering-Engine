#include "m33.h"
/*
inline M33::M33(V3f v0, V3f v1, V3f v2) : M33() {
	m33[0] = v0;
	m33[1] = v1;
	m33[2] = v2;
}

inline M33::M33() {
	m33.resize(3, V3f(1.0f, 1.0f, 1.0f));
}
*/
V3f& M33::operator[](int i) {
	return m33[i];
}

V3f M33::GetCol(int i) {
	return V3f(m33[0][i], m33[1][i], m33[2][i]);
}

void M33::SetCol(int i, V3f v) {
	for (int j = 0; j < 3; j++) {
		m33[j][i] = v[j];
	}
}

V3f M33::operator*(V3f v) {
	return V3f(m33[0].dot(v), m33[1].dot(v), m33[2].dot(v));
}

M33 M33::operator*(M33 m) {
	M33 ret = M33();
	ret.SetCol(0, *this * m.GetCol(0));
	ret.SetCol(1, *this * m.GetCol(1));
	ret.SetCol(2, *this * m.GetCol(2));
	return ret;
}

M33 M33::Inversion() {
	M33 ret = M33();
	V3f c0 = GetCol(0), c1 = GetCol(1), c2 = GetCol(2);
	ret[0] = c1.cross(c2);
	ret[0] = ret[0] / c0.dot(ret[0]);
	ret[1] = c2.cross(c0);
	ret[1] = ret[1] / c1.dot(ret[1]);
	ret[2] = c0.cross(c1);
	ret[2] = ret[2] / c2.dot(ret[2]);
	return ret;
}

M33 M33::Transpositon() {
	M33 ret = M33();
	ret[0] = GetCol(0);
	ret[1] = GetCol(1);
	ret[2] = GetCol(2);
	return ret;
}

M33 M33::Rotation(V3f axis, float theta) {
	M33 ret = M33();
	V3f n = axis.normalize();
	float a = n.xyz[0], b = n.xyz[1], c = n.xyz[2];
	float ap2 = a * a, bp2 = b * b, cp2 = c * c;
	float sint = sinf(theta), cost = cosf(theta);
	ret[0] = V3f(ap2 + (1.0f - ap2)*cost, a*b*(1.0f - cost) + c*sint, a*c*(1.0f - cost) - b*sint);
	ret[1] = V3f(a*b*(1.0f - cost) - c*sint, bp2 + (1.0f - bp2)*cost, b*c*(1.0f - cost) + a*sint);
	ret[2] = V3f(a*c*(1.0f - cost) + b*sint, b*c*(1.0f - cost) - a*sint, cp2 + (1.0f - cp2)*cost);
	return ret;
}

ostream& operator<<(ostream &os, M33 m) {
	os << m[0] << endl << m[1] << endl << m[2];
	return os;
}

void M33::SetRotationAboutY(float theta) {
	float thetar = theta / 180.0f * 3.1415f;
	M33 &m = *this;
	m[0] = V3f(cosf(thetar), 0.0f, sinf(thetar));
	m[1] = V3f(0.0f, 1.0f, 0.0f);
	m[2] = V3f(-sinf(thetar), 0.0f, cosf(thetar));
}