#include "framebuffer.h"

#include <iostream>
#include <algorithm>
#include <FreeImage.h>
#include <sstream>
using namespace std;
#include <math.h>
#include "m33.h"

#define PI 3.141516926
#define STEP 0.01
#define DSTEP 1.0

// Basic functions

Framebuffer::Framebuffer(int width, int height, const char *label, int type) {
	this->width_ = width;
	this->height_ = height;
	this->label_ = label;
	this->pixels_ = new unsigned int[width_ * height_];
	this->zbuffer = new float[width_ * height_];
	abuffer_ = new float[width_ * height_];
	type_ = type;
	ClearABuffer(1.f);
	ClearZBuffer(0.f);
	ClearBackground(V3f(0.f, 0.f, 0.f));
}

Framebuffer::Framebuffer(const string &texture_name, const char *label, int type) {
	FreeImage_Initialise();
	FIBITMAP* bmp = FreeImage_Load(FreeImage_GetFileType(texture_name.c_str(), 0), texture_name.c_str());
	width_ = FreeImage_GetWidth(bmp);
	height_ = FreeImage_GetHeight(bmp);
	int bits = FreeImage_GetBPP(bmp);
	pixels_ = new unsigned int[width_ * height_];
	zbuffer = new float[width_ * height_];
	abuffer_ = new float[width_ * height_];
	ClearABuffer(1.f);
	ClearZBuffer(0.f);
	ClearBackground(V3f(0.f, 0.f, 0.f));
	label_ = label;
	type_ = type;
	RGBQUAD color;

	for (int y = 0; y < height_; y++) {
		for (int x = 0; x < width_; x++) {
			FreeImage_GetPixelColor(bmp, x, y, &color);
			pixels_[y * width_ + x] = ((color.rgbRed << 16) | (color.rgbGreen << 8) | (color.rgbBlue));
			if (bits == 32) {
				abuffer_[y * width_ + x] = (float)color.rgbReserved / 255.f;
			}
		}
	}

	FreeImage_Unload(bmp);
	FreeImage_DeInitialise();
}

void Framebuffer::ClearBackground(unsigned int col) {
	for (int i = 0; i < width_ * height_; i++) {
		pixels_[i] = col;
	}
}

void Framebuffer::ClearBackground(V3f col) {
	for (int i = 0; i < width_ * height_; i++) {
		pixels_[i] = col.V3fToHex();
	}
}

void Framebuffer::SetPixel(int x, int y, unsigned int col) {
	int index = (height_ - 1 - y) * width_ + x;
	if (index >= 0 && index < height_ * width_) {
		if (col > 0xffffff) col = 0xffffff;
		else if (col < 0x000000) col = 0x000000;
		pixels_[index] = col;
	}
	else
		cout << x << ", " << y << endl;
}

void Framebuffer::SetPixel(int x, int y, V3f col) {
	int index = (height_ - 1 - y) * width_ + x;
	if (index >= 0 && index < height_ * width_)
		pixels_[index] = col.V3fToHex();
	else
		cout << x << ", " << y << endl;
}

bool Framebuffer::IsUpdatePixel(int u, int v, float z) {
	if (u < 0 || v < 0 || u > width_ - 1 || v > height_ - 1)
		return false;

	if (zbuffer[(height_ - 1 - v) * width_ + u] > z)
		return false;

	zbuffer[(height_ - 1 - v)*width_ + u] = z;
	return true;
}

// 2D Objects Draw Functions

void Framebuffer::DrawCircle(int u0, int v0, int z0, float r, unsigned int col) {
	int minX = u0 - r, maxX = u0 + r, minY = v0 - r, maxY = v0 + r;
	
	if (minX < 0) minX = 0;
	if (minX > width_ - 1)	return;
	if (maxX < 0) return;
	if (maxX > width_ - 1) maxX = width_ - 1;

	if (minY < 0) minY = 0;
	if (minY > height_ - 1)	return;
	if (maxY < 0) return;
	if (maxY > height_ - 1)	maxY = height_ - 1;
	
	float rp2 = r * r;
	for (int v = minY; v <= maxY; v++) {
		for (int u = minX; u <= maxX; u++) {
			V3f vec = V3f((float)u - (float)u0, (float)v - (float)v0, 0.0f);
			if (vec.dot(vec) <= rp2)	TestZBuffer(V3f(u, v, z0), col);//SetPixel(u, v, col);
		}
	}
	
}

void Framebuffer::DrawRectangle(int x0, int y0, int x1, int y1, unsigned int col) {
	for (int i = x0; i < x1; i++)
		for (int j = y0; j < y1; j++)
			SetPixel(i, j, col);
}

void Framebuffer::DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, unsigned int col) {
	/*This implementation based on algorithm on http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html */
	float delta10 = (float)(x1 - x0) / (float)(y1 - y0);
	float delta20 = (float)(x2 - x0) / (float)(y2 - y0);
	float delta21 = (float)(x2 - x1) / (float)(y2 - y1);

	float ix = (float)x0, jx = (float)x0;
	int iy = y0, jy = y0;

	if (delta10 > delta20) {
		for (; iy <= y1; iy++, jy++, ix += delta20, jx += delta10)
			FillHLine((int)ix, (int)jx, iy, col);
		jx = (float)x1;
		jy = y1;

		for (; iy <= y2; iy++, jy++, ix += delta20, jx += delta21)
			FillHLine((int)ix, (int)jx, iy, col);
	}
	else {
		for (; iy <= y1; iy++, jy++, ix += delta10, jx += delta20)
			FillHLine(ix, jx, iy, col);
		ix = x1;
		iy = y1;
		for (; iy <= y2; iy++, jy++, ix += delta21, jx += delta20)
			FillHLine(ix, jx, iy, col);
	}
}

void Framebuffer::Draw2DSegment(V3f p0, V3f p1, V3f c0, V3f c1) {
// Reference: Course note
	float du = fabsf(p0[0] - p1[0]);
	float dv = fabsf(p0[1] - p1[1]);
	int steps;
	if (du > dv) {
		steps = static_cast<int>(du + 2);
	} else {
		steps = static_cast<int>(dv + 2);
	}

	V3f duv = (p1 - p0) / static_cast<float>(steps - 1);
	V3f dc = (c1 - c0) / static_cast<float>(steps - 1);
	for (int stepi = 0; stepi < steps; stepi++) {
		V3f uv = p0 + duv * static_cast<float>(stepi);
		V3f color = c0 + dc * static_cast<float>(stepi);
		TestZBuffer(uv, color.V3fToHex());
	}

}

inline void Framebuffer::FillHLine(int x0, int x1, int y, unsigned int col) {
	for (int i = x0; i < x1; i++)
		SetPixel(i, y, col);
}

void Framebuffer::LoadTiff(const string filename) {
	FreeImage_Initialise();
	FIBITMAP* bmp = FreeImage_Load(FreeImage_GetFileType(filename.c_str(), 0), filename.c_str());
	int width = FreeImage_GetWidth(bmp);
	if (width > width_) width = width_;
	int height = FreeImage_GetHeight(bmp);
	if (height > height_) height = height_;
	RGBQUAD color;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned int pix;
			FreeImage_GetPixelColor(bmp, x, y, &color);
			pix = (color.rgbRed << 16) | (color.rgbGreen << 8) | (color.rgbBlue);
			SetPixel(x, height_ - y - 1, pix);
		}
	}

	FreeImage_Unload(bmp);
	FreeImage_DeInitialise();
}

void Framebuffer::SaveTiff(const string filename) {
	FreeImage_Initialise();
	FIBITMAP* bmp = FreeImage_Allocate(width_, height_, 24);
	RGBQUAD color;
	unsigned int cl = 0xffffff;
	for (int y = 0; y < height_; y++) {
		for (int x = 0; x < width_; x++) {
			int index = y * width_ + x;
			unsigned int pix = pixels_[index];
			color.rgbRed = (pix >> 16) & 0xff;
			color.rgbGreen = (pix >> 8) & 0xff;
			color.rgbBlue = pix & 0xff;
			FreeImage_SetPixelColor(bmp, x, y, &color);
		}
	}
	if (FreeImage_Save(FIF_TIFF, bmp, filename.c_str(), 0)) ;
	FreeImage_DeInitialise();
}

void Framebuffer::SaveTiff(const int &frame, const int &num_of_frames, const string &filename) {
	if (frame > num_of_frames) {
		return;
	} else if (frame == num_of_frames) {
		cout << "Tiff sequence saved" << endl;
		return;
	}
	stringstream ss;
	int digits = to_string(num_of_frames).length();
	int frame_len = to_string(frame).length();
	ss << filename;
	
	for (int i = 0; i < digits - frame_len; i++) {
		ss << "0";
	}

	ss << frame << ".tiff";
	SaveTiff("./video/" + ss.str());
}

// 3D Draw Functions

void Framebuffer::ClearZBuffer(float z) {
	for (int i = 0; i < width_ * height_; i++) {
		zbuffer[i] = z;
	}
}

void Framebuffer::ClearABuffer(float a) {
	for (int i = 0; i < width_ * height_; i++) {
		abuffer_[i] = a;
	}
}

void Framebuffer::TestZBuffer(V3f todraw, unsigned int color) {
	int u = static_cast<int>(todraw[0]);
	int v = static_cast<int>(todraw[1]);
	if (u < 0 || v < 0 || u > width_ - 1 || v > height_ - 1) return;
	
	int uv = (height_ - 1 - v) * width_ + u;
	if (zbuffer[uv] > todraw[2]) return;
	zbuffer[uv] = todraw[2];
	
	SetPixel(todraw[0], todraw[1], color);
}

void Framebuffer::Draw3DPoint(V3f &point, Camera *camera, int size, V3f &color) {
	V3f toproject;
	if (!camera->Projection(point, toproject)) return;
	DrawCircle( static_cast<int>(toproject[0]), 
				static_cast<int>(toproject[1]), 
				static_cast<int>(toproject[2]), 
				size, 
				color.V3fToHex());
}

void Framebuffer::Draw3DSegment(V3f p0, V3f p1, V3f c0, V3f c1, Camera *camera) {
	V3f pp0, pp1;
	if (!camera->Projection(p0, pp0))	return;
	if (!camera->Projection(p1, pp1))	return;

	Draw2DSegment(pp0, pp1, c0, c1);
}

unsigned int Framebuffer::GetPixel(const int &s, const int &t) {
	return pixels_[(height_ - t - 1) * width_ + s];
}

unsigned int Framebuffer::Lookup(V3f uv) {
	/*
	float u = uv[0] - floor(uv[0]), v = uv[1] - floor(uv[1]);
	if (u < 0.f) {
		u += 1.f;
	}
	if (v < 0.f) {
		v += 1.f;
	}
	int s = (int)((float)width_ * u);
	int t = (int)((float)height_ * v);
	return GetPixel(s, t);
	*/
	float u = uv[0], v = uv[1];
	int s = (int)((float)width_ * u);
	int t = (int)((float)height_ * v);
	if (s < 0) s = 0;
	else if (s > width_ - 1) s = width_ - 1;
	if (t < 0) t = 0;
	else if (t > height_ - 1) s = height_ - 1;
	return GetPixel(s, t);
}

unsigned int Framebuffer::BilinearLookup(V3f uv) {
	float uf = (float)width_ * uv[0];
	float vf = (float)height_ * uv[1];
	
	
	if (uf < 0.5f) uf = 1.0f;
	else if (uf > (float) width_ - 0.5f) uf = (float)width_ - 1.0f;
	if (vf < 0.5f) vf = 1.0f;
	else if (vf > (float) height_ - 0.5f) vf = (float)height_ - 1.0f;
	
	int u0 = (int)(uf - 0.5f);
	int v0 = (int)(vf - 0.5f);
	float du = uf - ((float)u0 + 0.5f);
	float dv = vf - ((float)v0 + 0.5f);

	V3f c00; 
	c00.HexToV3f(GetPixel(u0, v0));

	if (u0 + 1 > width_ - 1) u0 = width_ - 2;
	V3f c10; 
	c10.HexToV3f(GetPixel(u0 + 1, v0));

	if (v0 + 1 > height_ - 1) v0 = height_ - 2;
	V3f c11;
	c11.HexToV3f(GetPixel(u0 + 1, v0 + 1));

	V3f c01;
	c01.HexToV3f(GetPixel(u0, v0 + 1));

	V3f ret =
		c00*(1.0f - du)*(1.0f - dv) +
		c10*du*(1.0f - dv) +
		c11*du*dv +
		c01*(1.0f - du)*dv;

	return ret.V3fToHex();
}

float Framebuffer::GetPixelAlpha(const int &s, const int &t) {
	return abuffer_[(height_ - t - 1) * width_ + s];
}

bool Framebuffer::ShadowTest(V3f lightpoint, float zbias) {
	if (lightpoint[0] < 0.0f || lightpoint[0] >= (float)width_ ||
		lightpoint[1] < 0.0f || lightpoint[1] >= (float)height_)
		return false;

	int uv = (height_ - 1 - (int)lightpoint[1])*width_ + (int)lightpoint[0];
	return (lightpoint[2] + zbias < zbuffer[uv]);
}