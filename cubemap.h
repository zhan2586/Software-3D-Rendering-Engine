#pragma once
#include <vector>
using namespace std;

#include "framebuffer.h"
#include "v3f.h"
#include "Camera.h"


class CubeMap {
public:
	vector<Framebuffer*> faces_;
	int index_;
	V3f uv;
	bool isprint_;

	CubeMap() : isprint_(false), index_(0) {	};
	CubeMap(const string &name);

	unsigned int LookUp(V3f dir);
	void ClearBackgroundWithEnvironment(Framebuffer *fb, Camera *camera);
	
};