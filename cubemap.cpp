#include "cubemap.h"
#include <math.h>
#include <algorithm>
using namespace std;
#include <FreeImage.h>

bool PosX(V3f &dir, V3f &uv, float &max_dim) {
	if (dir[0] == max_dim) {
		uv = V3f(-dir[2], -dir[1], 0.f) / dir[0];
		return true;
	}
	return false;
}
bool NegX(V3f &dir, V3f &uv, float &max_dim) {
	if (-dir[0] == max_dim) {
		uv = V3f(dir[2], -dir[1], 0.f) / -dir[0];
		return true;
	}
	return false;
}
bool PosY(V3f &dir, V3f &uv, float &max_dim) {
	if (dir[1] == max_dim) {
		uv = V3f(dir[0], -dir[2], 0.f) / dir[1];
		return true;
	}
	return false;
}
bool NegY(V3f &dir, V3f &uv, float &max_dim) {
	if (-dir[1] == max_dim) {
		uv = V3f(dir[0], -dir[2], 0.f) / -dir[1];
		return true;
	}
	return false;
}
bool PosZ(V3f &dir, V3f &uv, float &max_dim) {
	if (dir[2] == max_dim) {
		uv = V3f(dir[0], -dir[1], 0.f) / dir[2];
		return true;
	}
	return false;
}
bool NegZ(V3f &dir, V3f &uv, float &max_dim) {
	if (-dir[2] == max_dim) {
		uv = V3f(-dir[0], -dir[1], 0.f) / -dir[2];
		return true;
	}
	return false;
}

bool(*table_[6])(V3f &dir, V3f &uv, float &max_dim) = { PosX, NegX, PosY, NegY, PosZ, NegZ };

CubeMap::CubeMap(const string &name) : CubeMap(){
	if (!faces_.empty()) {
		for (auto face : faces_) delete face;
	}
	faces_.resize(6);
	faces_[0] = new Framebuffer();
	faces_[0]->label_ = "right";
	faces_[1] = new Framebuffer();
	faces_[1]->label_ = "left";
	faces_[2] = new Framebuffer();
	faces_[2]->label_ = "top";
	faces_[3] = new Framebuffer();
	faces_[3]->label_ = "bottom";
	faces_[4] = new Framebuffer();
	faces_[4]->label_ = "back";
	faces_[5] = new Framebuffer();
	faces_[5]->label_ = "front";
	/*
	// x
	faces_[0] = new Framebuffer("./skybox/right.jpg", "right", 1);
	faces_[1] = new Framebuffer("./skybox/left.jpg", "left", 1);
	// y
	faces_[2] = new Framebuffer("./skybox/bottom.jpg", "top", 1);
	faces_[3] = new Framebuffer("./skybox/top.jpg", "bottom", 1);
	// z
	faces_[4] = new Framebuffer("./skybox/front.jpg", "front", 1);
	faces_[5] = new Framebuffer("./skybox/back.jpg", "back", 1);
	*/
	FreeImage_Initialise();
	FIBITMAP* bmp = FreeImage_Load(FreeImage_GetFileType(name.c_str(), 0), name.c_str());
	int width_sum = FreeImage_GetWidth(bmp);
	int width = width_sum / 3;
	int height_sum = FreeImage_GetHeight(bmp);
	int height = height_sum / 4;
	for (auto face : faces_) {
		face->height_ = height;
		face->width_ = width;
		face->pixels_ = new unsigned int[width * height];
		face->zbuffer = new float[width * height];
		face->abuffer_ = new float[width * height];
		face->ClearZBuffer(0.f);
		face->ClearABuffer(0.f);
		face->ClearBackground(V3f(0.f, 0.f, 0.f));	
		RGBQUAD color;
		int w = 0, h = 0;

		if (face->label_ == "right") {
			w = 2;
			h = 2;
		} else if (face->label_ == "left") {
			w = 0;
			h = 2;
		} else if (face->label_ == "top") {
			w = 1;
			h = 3;
		} else if (face->label_ == "bottom") {
			w = 1;
			h = 1;
		} else if (face->label_ == "front") {
			w = 1;
			h = 2;
		} else if (face->label_ == "back") {
			w = 1;
			h = 0;
		}

		for (int y = height * h; y < height * (h + 1); y++) {
			for (int x = width * w; x < width * (w + 1); x++) {
				FreeImage_GetPixelColor(bmp, x, y, &color);
				int v = y - height * h, u = x - width * w;
				int index = 0;
				if (face->label_ == "back") {
					index = (height - v - 1) * width + u;
				} else if (face->label_ == "left" || face->label_ == "right") {
					index = v * width + (width - u - 1);
				} else if (face->label_ == "top") {
					index = v * width + u;
				} else if (face->label_ == "bottom") {
					index = (height - v - 1) * width + u;
				} else if (face->label_ == "front") {
					index = v * width + (width - u - 1);
				}

				face->pixels_[index] = ((color.rgbRed << 16) | (color.rgbGreen << 8) | (color.rgbBlue));
			}
		}
		
	}	
	FreeImage_Unload(bmp);
	FreeImage_DeInitialise();
}

unsigned int CubeMap::LookUp(V3f dir) {
	float max_dim = max(max(fabs(dir[0]), fabs(dir[1])), fabs(dir[2]));
	int res = 0;
	V3f uv(dir[1], dir[2], 0.f);
	int loop_counts = 0;
	for (int i = 0; i < 6; i++) {
		loop_counts++;
		if (table_[index_](dir, uv, max_dim)) {
			res = index_;
			break;
		}
		index_ = (index_ + 1) % 6;
	}
#if DEBUG
	if (isprint_) {
		cout << "current index:\t "<< index_ << endl;
		cout << "counts before find correct face:\t " << loop_counts << endl;
		isprint_ = false;
	}
#endif
	uv = uv * 0.5f + V3f(0.5f, 0.5f, 0.f);

	// return faces_[res]->Lookup(uv);
	return faces_[res]->BilinearLookup(uv);
	// return 0xff0000;
}

void CubeMap::ClearBackgroundWithEnvironment(Framebuffer *fb, Camera *camera) {
	for (int v = 0; v < camera->height_; v++) {
		for (int u = 0; u < camera->width_; u++) {
			V3f pixel(.5f + (float)v, .5f + (float)u, -1.f);
			float realz = (camera->center_ + camera->GetViewDirection() * camera->GetFocalLens())[2];
			V3f eye_dir = V3f(pixel[0], pixel[1], realz);
			fb->SetPixel(u, v, LookUp(eye_dir));
		}
	}
	fb->ClearZBuffer(1.0f);
}