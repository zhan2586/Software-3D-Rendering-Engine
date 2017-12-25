#pragma once
#include <vector>
#include <string>
using namespace std;

#include "v3f.h"
#include "m33.h"
#include "AABB.h"
#include "framebuffer.h"
#include "Camera.h"
#include "light.h"
#include "cubemap.h"

class Mesh
{
public:
	V3f *verts_, *colors_, *texcoords_, *normals_;
	int num_verts_;
	unsigned int *triangles_;
	int num_triangles_;
	float ka, spec;
	string name;
	bool enable;
	bool fresnel_enable_;
	float kfresnel_;
	bool isFill_;

	Mesh() : isFill_(true), kfresnel_(1.0f), fresnel_enable_(false), enable(true), name(""), spec(180.f), ka(0.3f), verts_(0), colors_(0), num_verts_(0), triangles_(0), num_triangles_(0), texcoords_(0), normals_(0) {};
	Mesh(V3f *verts, V3f *texcoords, V3f *normals, unsigned int *indices, int num_verts, int num_tris);
	
	void LoadFromBin(const string &filename);
	void RenderWireframe(Camera *camera, Framebuffer *fb);
	void RenderFilledObject(Camera *camera, Framebuffer *fb, Framebuffer *texture, bool lit, int mode, vector<Light*> *lights);
	void RenderFilledObject(Camera *camera, Framebuffer *fb, CubeMap *cube, bool lit, int mode, vector<Light*> *lights);
	void RenderSkybox(Camera *camera, Framebuffer *fb, CubeMap *cube);
	void RotateAboutAxis(V3f origin, V3f axis, float theta);
	void Translate(V3f direction);
	void Scale(float k);
	V3f GetCenterOfMass();
	V3f SetEEQs(V3f v0, V3f v1, V3f v2);
	AABB GetAABB();
	// Interpolation
	M33 GetScreenSpaceMarix(V3f *vertices);
	M33 GetModelSapceMatrix(M33 vs, Camera *camera);

	V3f PhongLit(V3f &color, V3f &normal, Light &light, V3f &eye, V3f &point);

	void RenderHW();

private:
};