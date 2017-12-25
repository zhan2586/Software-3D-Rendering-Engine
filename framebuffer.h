#pragma once

#include "v3f.h"
#include "Camera.h"

class Scene;

class Framebuffer
{
public:
	int width_, height_;
	unsigned int *pixels_;
	const char *label_;
	int win_x_, win_y_;
	float *zbuffer;
	float *abuffer_;
	int type_;
	Framebuffer() : abuffer_(0), zbuffer(0), type_(0) {};
	Framebuffer(int width, int height, const char *label, int type);
	Framebuffer(const string &texture_name, const char *label, int type);
	void ClearBackground(unsigned int col);
	void ClearBackground(V3f col);
	void InitializeWindow(int* argc, char** argv, int winx, int winy);
	void Render();
	void SetPixel(int x, int y, unsigned int col);
	void SetPixel(int x, int y, V3f col);
	bool IsUpdatePixel(int u, int v, float z);

	// 2D funcitons
	void DrawCircle(int u0, int v0, int z0, float r, unsigned int col);
	void DrawRectangle(int x0, int y0, int x1, int y1, unsigned int col);
	void DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, unsigned int col);
	void Draw2DSegment(V3f p0, V3f p1, V3f c0, V3f c1);
	void FillHLine(int x0, int x1, int y, unsigned int col);
	void LoadTiff(const string filename);
	void SaveTiff(const string filename);
	void SaveTiff(const int &frame, const int &num_of_frames, const string &filename);

	// 3D functions
	void ClearZBuffer(float z);
	void TestZBuffer(V3f todraw, unsigned int color);
	void Framebuffer::ClearABuffer(float a);
	void Draw3DPoint(V3f &point, Camera *camera, int size, V3f &color);
	void Draw3DSegment(V3f p0, V3f p1, V3f c0, V3f c1, Camera *camera);

	// Texture Framebuffer
	unsigned int GetPixel(const int &s, const int &t);
	unsigned int Lookup(V3f uv);
	unsigned int Framebuffer::BilinearLookup(V3f uv);
	float GetPixelAlpha(const int &s, const int &t);

	// Shadow
	bool ShadowTest(V3f lightpoint, float zbias);

private:
};

