#include "AABB.h"
#include "mesh.h"

AABB::AABB(V3f point){
	corners_[0] = point;
	corners_[1] = point;
}

AABB::AABB(V3f point1, V3f point2){
	corners_[0] = point1;
	corners_[1] = point2;
}

void AABB::Generate(V3f new_point) {
	for (int i = 0; i < 3; i++) {
		if (corners_[0][i] > new_point[i]) corners_[0][i] = new_point[i];
		if (corners_[1][i] < new_point[i]) corners_[1][i] = new_point[i];
	}
}

void AABB::GenerateFromMesh(Mesh *mesh) {
	if (mesh->verts_ == NULL) return;
	corners_[0] = mesh->verts_[0];
	corners_[1] = mesh->verts_[1];
	for (int i = 1; i < mesh->num_verts_; i++) {
		Generate(mesh->verts_[i]);
	}
}

void AABB::RotateAboutAxis(V3f O, V3f a, float theta) {
	for (int vi = 0; vi < num_verts_; vi++) {
		verts_[vi] = verts_[vi].RotatePointAboutAxis(O, a, theta);
	}
}

void AABB::Translate(V3f direction) {
	for (int vi = 0; vi < num_verts_; vi++) {
		verts_[vi] = verts_[vi] + direction;
	}
}

V3f AABB::GetCenterOfMass() {
	V3f ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < num_verts_; vi++) {
		ret = ret + verts_[vi];
	}
	ret = ret / (float)num_verts_;
	return ret;
}

int AABB::Clip(V3f minc, V3f maxc, int dimsN) {

	for (int i = 0; i < dimsN; i++) {
		if (corners_[0][i] > maxc[i])
			return 0;
		if (corners_[1][i] < minc[i])
			return 0;
	}

	for (int i = 0; i < dimsN; i++) {
		if (corners_[0][i] < minc[i])
			corners_[0][i] = minc[i];
		if (corners_[1][i] > maxc[i])
			corners_[1][i] = maxc[i];
	}

	return 1;

}