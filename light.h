#pragma once
#include <string>
using namespace std;
#include "v3f.h"
#include "framebuffer.h"
#include "Camera.h"

class Light {// point lighting source for now
public:
	V3f pos_, color_;
	Framebuffer *shadowmap_;
	Framebuffer *imgcolor_;
	Camera *camera_;
	Camera *projector_;
	string name;
	bool needUpdate;
	bool enable;

	Light() : enable(true), needUpdate(true), name(""), imgcolor_(0), camera_(0), shadowmap_(0), pos_(0.f, 0.f, 0.f), color_(1.f, 1.f, 1.f) {};
};