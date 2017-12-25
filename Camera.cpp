#include "Camera.h"

#include <sstream>
#include <fstream>
using namespace std;
#include <GL\glew.h>
#include <GL\freeglut.h>

#include "m33.h"
#include "AABB.h"

#define PI_ROUNDED 3.1415926

Camera::Camera() {

}

Camera::Camera(int width, int height, float horizontal_fov){
	width_ = width;
	height_ = height;
	center_ = V3f(0.0f, 0.0f, 0.0f);
	a_ = V3f(1.0f, 0.0f, 0.0f);
	b_ = V3f(0.0f, -1.0f, 0.0f);
	float h = horizontal_fov * PI_ROUNDED / 180.f / 2.0f;
	c_ = V3f(-0.5f * static_cast<float>(width), 
			0.5f * static_cast<float>(height), 
			static_cast<float>(-width) / (2.0f * tanf(h)));
}

bool Camera::Projection(V3f P, V3f &toproject) {
	M33 camera;
	camera.SetCol(0, a_);
	camera.SetCol(1, b_);
	camera.SetCol(2, c_);

	V3f point_center_inversion = camera.Inversion()*(P - center_);

	if (point_center_inversion[2] <= 0.0f) return false;	

	toproject = point_center_inversion / point_center_inversion[2];	
	toproject[2] = 1.0f / point_center_inversion[2];

	return true;
}

void Camera::Translation(V3f v) {
	center_ = center_ + v;
}

void Camera::Pan(float theta) {
	a_ = a_.RotatePointAboutAxis(V3f(0.0f, 0.0f, 0.0f), b_.normalize() * -1.0f, theta);
	c_ = c_.RotatePointAboutAxis(V3f(0.0f, 0.0f, 0.0f), b_.normalize() * -1.0f, theta);
}
void Camera::Tilt(float theta) {
	b_ = b_.RotatePointAboutAxis(V3f(0.0f, 0.0f, 0.0f), a_.normalize(), theta);
	c_ = c_.RotatePointAboutAxis(V3f(0.0f, 0.0f, 0.0f), a_.normalize(), theta);
}
void Camera::Roll(float theta) {
	a_ = a_.RotatePointAboutAxis(V3f(0.0f, 0.0f, 0.0f), GetViewDirection(), theta);
	b_ = b_.RotatePointAboutAxis(V3f(0.0f, 0.0f, 0.0f), GetViewDirection(), theta);
	c_ = c_.RotatePointAboutAxis(V3f(0.0f, 0.0f, 0.0f), GetViewDirection(), theta);
}

V3f Camera::GetViewDirection() {
	return (a_.cross(b_)).normalize();
};

float Camera::GetFocalLens() {
	float f = GetViewDirection().dot(c_);
	return f;
}

void Camera::PositionAndOrient(V3f new_center, V3f lap, V3f up) {
	V3f newa, newb, newc;
	V3f newvd = (lap - new_center).normalize();
	float f = GetFocalLens();
	newa = (newvd.cross(up)).normalize() * a_.length();
	newb = (newvd.cross(newa)).normalize() * b_.length();
	newc = newvd * f - newb * (float)height_ / 2.0f - newa * (float)width_ / 2.0f;
	center_ = new_center;
	a_ = newa;
	b_ = newb;
	c_ = newc;
}

void Camera::Interpolation(Camera *camera_origin, Camera *camera_target, float frac) {
	*this = *camera_origin;

	V3f newC = camera_origin->center_ + (camera_target->center_ - camera_origin->center_) * frac;
	V3f newvd = camera_origin->GetViewDirection() + (camera_target->GetViewDirection() - camera_target->GetViewDirection()) * frac;
	V3f up = (camera_origin->b_ + (camera_target->b_ - camera_origin->b_) * frac) * -1.0f;
	PositionAndOrient(newC, newC + newvd, up);
}

void Camera::SaveAsText(const string &filename) {
	ofstream save_ofs(filename);
	if (save_ofs.is_open()) {
		save_ofs << this->width_ << endl;
		save_ofs << this->height_ << endl;
		save_ofs << this->center_ << endl;
		save_ofs << this->a_ << endl;
		save_ofs << this->b_ << endl;
		save_ofs << this->c_ << endl;
	}
	save_ofs.close();
}

void Camera::LoadFromText(const string &filename) {
	ifstream load_ifs;
	string word;
	vector<float> dataset;
	load_ifs.open(filename);
	if (load_ifs.is_open()) {
		while (load_ifs >> word) {
			stringstream ss;
			for (int i = 0; i < word.length(); i++) {
				char curr = word[i];
				if (curr == '.' || curr == '-' || (curr >= '0' && curr <= '9')) ss << curr;
			}
			float number;
			ss >> number;
			dataset.push_back(number);
		}
	}
	load_ifs.close();

	if (!dataset.empty()) {
		this->width_ = (int)dataset[0];
		this->height_ = (int)dataset[1];
		this->center_ = V3f(dataset[2], dataset[3], dataset[4]);
		this->a_ = V3f(dataset[5], dataset[6], dataset[7]);
		this->b_ = V3f(dataset[8], dataset[9], dataset[10]);
		this->c_ = V3f(dataset[11], dataset[12], dataset[13]);
	}
}

V3f Camera::Unproject(V3f &point) {
	V3f upoint = center_ + (c_ + a_ * point[0] + b_ * point[1]) / point[2];
	return upoint;
}

void Camera::SetToAABB(AABB aabb) {
	V3f vs[8];
	V3f maxc = aabb.corners_[1], minc = aabb.corners_[0];
	V3f diag = maxc - minc;
	
	vs[0] = minc + V3f(diag[0] * 0.0f, diag[1] * 0.0f, diag[2] * 0.0f);
	vs[1] = minc + V3f(diag[0] * 0.0f, diag[1] * 0.0f, diag[2] * 1.0f);
	vs[2] = minc + V3f(diag[0] * 0.0f, diag[1] * 1.0f, diag[2] * 0.0f);
	vs[3] = minc + V3f(diag[0] * 0.0f, diag[1] * 1.0f, diag[2] * 1.0f);
	vs[4] = minc + V3f(diag[0] * 1.0f, diag[1] * 0.0f, diag[2] * 0.0f);
	vs[5] = minc + V3f(diag[0] * 1.0f, diag[1] * 0.0f, diag[2] * 1.0f);
	vs[6] = minc + V3f(diag[0] * 1.0f, diag[1] * 1.0f, diag[2] * 0.0f);
	vs[7] = minc + V3f(diag[0] * 1.0f, diag[1] * 1.0f, diag[2] * 1.0f);

	V3f projVs[8];
	AABB paabb;
	for (int i = 0; i < 8; i++) {
		Projection(vs[i], projVs[i]);
		if (projVs[i][0] == FLT_MAX)
			return;
		if (i == 0) {
			paabb.corners_[0] = projVs[0];
			paabb.corners_[1] = projVs[0];
		}
		paabb.Generate(projVs[i]);
	}

	float dul = 0.0f - paabb.corners_[0][0];
	float dur = -(float)width_ + paabb.corners_[1][0];
	float du;
	if (dul < dur)
		du = dur;
	else
		du = dul;

	c_ = c_ - a_ * du;
	a_ =( a_.normalize() * a_.length() * (du * 2.0f + (float)width_)) / (float)width_;


	float dvl = 0.0f - paabb.corners_[0][1];
	float dvr = -(float)height_ + paabb.corners_[1][1];
	float dv;
	if (dvl < dvr)
		dv = dvr;
	else
		dv = dvl;

	c_ = c_ - b_ * dv;
	b_ = (b_.normalize() * b_.length() * (dv * 2.0f + (float)height_)) / (float)height_;
}

void Camera::SetIntrinsicsHW(float zNear, float zFar) {

	glViewport(0, 0, width_, height_);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float left, right, bottom, top;
	float scf = zNear / GetFocalLens();
	left = -a_.length()*(float)width_ / 2.0f * scf;
	right = +a_.length()*(float)width_ / 2.0f * scf;
	bottom = -b_.length()*(float)height_ / 2.0f*scf;
	top = +b_.length()*(float)height_ / 2.0f*scf;
	glFrustum(left, right, bottom, top, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);

}

void Camera::SetExtrinsicsHW() {

	V3f LAP = center_ + GetViewDirection()*100.0f;
	glLoadIdentity();
	gluLookAt(center_[0], center_[1], center_[2], LAP[0], LAP[1], LAP[2], -b_[0], -b_[1], -b_[2]);

}