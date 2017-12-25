#pragma once

#include <vector>
using namespace std;

#include "mesh.h"
#include "Camera.h"
#include "framebuffer.h"
#include "AABB.h"
#include "cubemap.h"
#include "CGInterface.h"

class Light;

class Scene {
public:
	CGInterface *cgi_;
	ShaderOneInterface *soi_;
	Camera *billboard_camera;
	Framebuffer *billboard_fb;
	V3f *billboard_verts_;
	float shadow_passf_;

	int frame;
	Framebuffer* fb00_;
	Camera* camera00_;
	vector<Mesh*> objects;
	vector<AABB*> bounding_boxs;
	vector<Light*> lights;
	vector<Camera*> lightcameras;
	vector<Framebuffer*> shadowmaps;
	vector<Framebuffer*> textures;

	// Invisibility
	Light* invis;
	Light* projector;
	Light* tmp;

	// Cubemap
	CubeMap *cube_texture_;
	CubeMap *cube_texture_apt_;
	CubeMap *cube_texture_cross_;
	Mesh *skybox_;

	Scene() : shadow_passf_(0), billboard_verts_(0), billboard_camera(0), billboard_fb(0), cgi_(0), cube_texture_cross_(0), skybox_(0), cube_texture_apt_(0), cube_texture_(0), tmp(0), projector(0), frame(0), fb00_(0), camera00_(0){};

	void Show();
	void ShowInvisibility();
	void ShowEM();
	void RenderGPU();
	void UpdateShadowMap(Light *light);
	void PrecomputeBillboard();

};