#pragma once

#include <string>
using namespace std;

#include "v3f.h"
class AABB;

class Camera {
public:
	int width_, height_;
	V3f center_, a_, b_, c_;
	Camera();
	Camera(int width, int height, float horizontal_fov);
	bool Projection(V3f P, V3f &toproject);
	void Translation(V3f v);
	void Pan(float theta);
	void Tilt(float theta);
	void Roll(float theta);
	V3f GetViewDirection();
	float GetFocalLens();
	void PositionAndOrient(V3f new_center, V3f lap, V3f up);
	void Interpolation(Camera *camera_origin, Camera *camera_target, float frac);
	void SaveAsText(const string &filename);
	void LoadFromText(const string &filename);
	V3f Unproject(V3f &point);
	void SetToAABB(AABB aabb);

	void SetIntrinsicsHW(float zNear, float zFar);
	void SetExtrinsicsHW();

 private:
};