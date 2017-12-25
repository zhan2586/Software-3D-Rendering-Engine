#pragma once
#include "v3f.h"
class Mesh;

class AABB {
 public:
	 V3f corners_[2];
	 V3f *verts_;
	 unsigned int *triangles_;
	 int num_verts_, num_triangles;

	 AABB() {};
	 AABB(V3f point);
	 AABB(V3f point1, V3f point2);
	 void Generate(V3f new_point);
	 void GenerateFromMesh(Mesh *mesh);
	 void RotateAboutAxis(V3f O, V3f a, float theta);
	 void Translate(V3f direction);
	 V3f GetCenterOfMass();
	 int Clip(V3f minc, V3f maxc, int dimsN);
};