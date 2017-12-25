#define HARDWARE_DEMO 0
#define SOFTWARE_EM_DEMO 1
#define SOFTWARE_SM_PTM_DEMO 2
#define USING_DEMO SOFTWARE_SM_PTM_DEMO

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <imgui\imgui_impl_glut.h>
#include <sstream>
#include <string>
using namespace std;

#include "scene.h"
#include "framebuffer.h"
#include "mesh.h"
#include "light.h"
#include "Camera.h"
#include "cubemap.h"

#define STEP 0.01
#define DSTEP 10.0

int time = 0;
Scene *scene;
Camera *camera;

void InfoSM_PTM() {
	cout << "Rendering Engine v006 Remake" << endl;
	cout << "Software Shadow Mapping and Projective Texture Mapping" << endl;
	cout << "Zhe Zeng" << endl;
	cout << "------------------------------------------" << endl;
	cout << "Functionalities" << endl;
	cout << "- Oct. 30" << endl;
	cout << "\t`\tturn on/off transparent text projector" << endl;
	cout << "\tADEW\tTranslate projector (x, y) position" << endl;
	cout << "\t1\tturn off one light" << endl;
	cout << "\t2\tturn on one light" << endl;
	cout << "\t3\tturn on/off invisibility" << endl;
	cout << "- Camera" << endl;
	cout << "\tadws\tTranslate Camera (x, y) position" << endl;
	cout << "\tqe\tForward/Backward" << endl;
	cout << "\trf\tPan" << endl;
	cout << "\ttg\tTilt" << endl;
	cout << "\tyh\tRoll" << endl;
}

void InfoHW() {
	cout << "Rendering Engine v006 Remake" << endl;
	cout << "Hardware Support Demo" << endl;
	cout << "Zhe Zeng" << endl;
	cout << "------------------------------------------" << endl;
	cout << "Functionalities" << endl;
	cout << "- Dec. 10" << endl;
	cout << "\tz \t Switch between Filled mode and Wireframe mode" << endl;
	cout << "\tx \t Switch between Intersected Shadow and Shadow under Multiple Light Sources" << endl;
	cout << "-----------------------------------------------------------------------------------------" << endl;
	cout << "- Camera" << endl;
	cout << "\tadws\tTranslate Camera (x, y) position" << endl;
	cout << "\tqe\tForward/Backward" << endl;
	cout << "\trf\tPan" << endl;
	cout << "\ttg\tTilt" << endl;
	cout << "\tyh\tRoll" << endl;
}

void InfoSW_EM() {
	cout << "Rendering Engine v006 Remake" << endl;
	cout << "Software Environment Mapping Demo" << endl;
	cout << "Zhe Zeng" << endl;
	cout << "------------------------------------------" << endl;
	cout << "Functionalities" << endl;
	cout << "- Nov. 13" << endl;
	cout << "\ti,k \t revolution up-down" << endl;
	cout << "\tj,l \t revolution left-right" << endl;
	cout << "\tz \t turn on/off refraction / fresnel" << endl;
	cout << "\tX \t increment of ks" << endl;
	cout << "\tx \t increment of kt" << endl;
	cout << "- Camera" << endl;
	cout << "\tadws\tTranslate Camera (x, y) position" << endl;
	cout << "\tqe\tForward/Backward" << endl;
	cout << "\trf\tPan" << endl;
	cout << "\ttg\tTilt" << endl;
	cout << "\tyh\tRoll" << endl;
}

void InitializeSMandPTMProgram() {
	InfoSM_PTM();
	scene = new Scene();

	scene->camera00_ = new Camera(800, 600, 50.f);
	scene->fb00_ = new Framebuffer(scene->camera00_->width_, scene->camera00_->height_, "main", 0);

	Framebuffer *texture00 = new Framebuffer("./textures/wood.jpg", "wood", 1);
	scene->textures.push_back(texture00);
	Framebuffer *texture01 = new Framebuffer("./textures/wood3.jpg", "wood2", 1);
	scene->textures.push_back(texture01);

	Mesh *teapot01 = new Mesh();
	teapot01->name = "teapot01";
	teapot01->LoadFromBin("./geometry/teapot1K.bin");
	teapot01->Scale(0.015f);
	teapot01->Translate(V3f(0.17f, -0.5f, -3.5f));
	scene->objects.push_back(teapot01);

	Mesh *teapot02 = new Mesh();
	teapot02->name = "teapot02";
	teapot02->LoadFromBin("./geometry/teapot1K.bin");
	teapot02->Scale(0.007f);
	teapot02->Translate(V3f(-0.4f, -0.7f, -3.3f));
	scene->objects.push_back(teapot02);

	AABB _floor;
	_floor.GenerateFromMesh(teapot01);
	V3f *verts_floor = new V3f[4];
	verts_floor[0] = _floor.corners_[0] + V3f(-2.5f, -0.3f, -5.5f);
	verts_floor[1] = verts_floor[0] + V3f(6.f, 0.f, 0.f);
	verts_floor[2] = verts_floor[1] + V3f(0.f, 0.f, 7.5f);
	verts_floor[3] = verts_floor[2] + V3f(-5.f, 0.f, 0.f);
	V3f *texcoords_floor = new V3f[4];
	texcoords_floor[0] = V3f(0.f, 0.f, 0.f);
	texcoords_floor[1] = V3f(1.f, 0.f, 0.f);
	texcoords_floor[2] = V3f(1.f, 1.f, 0.f);
	texcoords_floor[0] = V3f(0.f, 1.f, 0.f);
	V3f *normals_floor = new V3f[4];
	normals_floor[0] = normals_floor[1] = normals_floor[2] = normals_floor[3] = V3f(0.f, 1.f, 0.f);
	unsigned int *indices = new unsigned int [6] {
													0, 1, 3,
													1, 3, 2
												};

	Mesh *floor = new Mesh(verts_floor, texcoords_floor, normals_floor, indices, 4, 2);
	floor->name = "floor";
	scene->objects.push_back(floor);

	Light *lg00 = new Light();
	lg00->name = "projector00";
	lg00->color_ = V3f(1.f, 1.f, 1.f);
	lg00->pos_ = V3f(1.0f, 1.0f, -0.3f);
	lg00->imgcolor_ = new Framebuffer("./textures/transparent.png", "brick", 1);
	lg00->camera_ = new Camera(lg00->imgcolor_->width_, lg00->imgcolor_->height_, 30.f);
	lg00->shadowmap_ = new Framebuffer(lg00->camera_->width_, lg00->camera_->height_, "shadowmap", 0);
	lg00->shadowmap_->ClearZBuffer(0.0f);
	lg00->shadowmap_->ClearBackground(V3f(0.f, 0.f, 0.f));
	scene->projector = lg00;
	lg00->enable = false;
	scene->lights.push_back(lg00);

	V3f origin = teapot01->GetCenterOfMass() + V3f(0.15f, 1.f, 0.15f);
	vector<Light*> fourlights(4, NULL);
	int i = 0;
	for (auto light : fourlights) {
		light = new Light();
		light->name = "fourlights" + to_string(i);
		light->color_ = V3f(1.f, 1.f, 1.f);
		light->pos_ = origin;
		light->camera_ = new Camera(256, 256, 30.f);
		light->shadowmap_ = new Framebuffer(light->camera_->width_, light->camera_->height_, "shadowmap", 0);
		light->shadowmap_->ClearZBuffer(0.0f);
		light->shadowmap_->ClearBackground(V3f(0.f, 0.f, 0.f));
		scene->lights.push_back(light);
		i++;
	}

	// Invisibility
	
	scene->invis = new Light();
	scene->invis->name = "projectorP";
	scene->invis->color_ = V3f(1.f, 1.f, 1.f);
	scene->invis->pos_ = scene->camera00_->center_;
	scene->invis->camera_ = scene->camera00_;
	scene->invis->imgcolor_ = new Framebuffer(scene->invis->camera_->width_, scene->invis->camera_->height_, "imgcolor", 0);
	scene->invis->imgcolor_->ClearABuffer(1.f);
	scene->invis->shadowmap_ = new Framebuffer(scene->invis->camera_->width_, scene->invis->camera_->height_, "shadowmap", 0);
	scene->invis->shadowmap_->ClearZBuffer(0.0f);
	scene->invis->shadowmap_->ClearBackground(V3f(0.f, 0.f, 0.f));
	scene->invis->enable = false;
	scene->lights.push_back(scene->invis);
	
}

void InitializeEnvrionmentMapping() { // Environment Mapping Demo
	InfoSW_EM();
	scene = new Scene();
	scene->camera00_ = new Camera(800, 600, 60.f);
	scene->camera00_->Translation(V3f(0.f, 0.f, 3.f));
	scene->fb00_ = new Framebuffer(scene->camera00_->width_, scene->camera00_->height_, "main", 0);

	Framebuffer *texture00 = new Framebuffer("./textures/wood.jpg", "wood", 1);
	scene->textures.push_back(texture00);
	
	Mesh *teapot01 = new Mesh();
	teapot01->name = "teapot01";
	teapot01->LoadFromBin("./geometry/teapot1K.bin");
	teapot01->Scale(0.015f);
	teapot01->Translate(teapot01->GetCenterOfMass() * -1.f);
	scene->objects.push_back(teapot01);
	
	scene->cube_texture_cross_ = new CubeMap("./skybox/uffizi_cross.tiff");
	scene->cube_texture_apt_ = new CubeMap("./skybox/aptv100.jpg");
	scene->skybox_ = new Mesh();
	scene->cube_texture_ = scene->cube_texture_cross_;
}

void InitializeShaderProgram() { // Shader Program Demo
	InfoHW();
	scene = new Scene();
	scene->camera00_ = new Camera(800, 600, 50.f);
	scene->fb00_ = new Framebuffer(scene->camera00_->width_, scene->camera00_->height_, "main", 0);
	
	scene->cube_texture_ = new CubeMap();
	Framebuffer *_px = new Framebuffer("./skybox/uffizi_cross/right_.tiff", "right", 1);
	scene->cube_texture_->faces_.push_back(_px);
	Framebuffer *_nx = new Framebuffer("./skybox/uffizi_cross/left_.tiff", "left", 1);
	scene->cube_texture_->faces_.push_back(_nx);
	Framebuffer *_py = new Framebuffer("./skybox/uffizi_cross/top_.tiff", "top", 1);
	scene->cube_texture_->faces_.push_back(_py);
	Framebuffer *_ny = new Framebuffer("./skybox/uffizi_cross/bottom_.tiff", "bottom", 1);
	scene->cube_texture_->faces_.push_back(_ny);
	Framebuffer *_pz = new Framebuffer("./skybox/uffizi_cross/back_.tiff", "front", 1);
	scene->cube_texture_->faces_.push_back(_pz);
	Framebuffer *_nz = new Framebuffer("./skybox/uffizi_cross/front_.tiff", "back", 1);
	scene->cube_texture_->faces_.push_back(_nz);
	
	V3f *verts_cube = new V3f[8];
	float x = 10.f, y = 10.f, z = 10.f;
	float zpos = 0.f;
	verts_cube[0] = V3f(-x, -y, -z + zpos);
	verts_cube[1] = V3f(x, -y, -z + zpos);
	verts_cube[2] = V3f(x, y, -z + zpos);
	verts_cube[3] = V3f(-x, y, -z + zpos);
	verts_cube[4] = V3f(-x, y, z + zpos);
	verts_cube[5] = V3f(-x, -y, z + zpos);
	verts_cube[6] = V3f(x, -y, z + zpos);
	verts_cube[7] = V3f(x, y, z + zpos);
	unsigned int *indices_cube = new unsigned int[36]{ 0,1,2, 2,3,0, 0,3,4, 4,5,0, 4,5,6, 6,7,4, 7,6,1, 1,2,7, 7,4,3, 3,2,7, 0,5,6, 6,1,0 };
	Mesh *cube = new Mesh(verts_cube, 0, 0, indices_cube, 8, 12);
	cube->name = "skybox";
	scene->skybox_ = cube;
	scene->objects.push_back(cube);

	V3f *verts_box0 = new V3f[8];
	float verts_x = 0.2f, verts_y = 0.2f, verts_z = 0.2f;
	V3f box0_translate(-0.5f, -0.3f, -4.5f);
	verts_box0[0] = V3f(-verts_x, -verts_y, -verts_z) + box0_translate;
	verts_box0[1] = V3f(verts_x, -verts_y, -verts_z) + box0_translate;
	verts_box0[2] = V3f(verts_x, verts_y, -verts_z) + box0_translate;
	verts_box0[3] = V3f(-verts_x, verts_y, -verts_z) + box0_translate;
	verts_box0[4] = V3f(-verts_x, verts_y, verts_z) + box0_translate;
	verts_box0[5] = V3f(-verts_x, -verts_y, verts_z) + box0_translate;
	verts_box0[6] = V3f(verts_x, -verts_y, verts_z) + box0_translate;
	verts_box0[7] = V3f(verts_x, verts_y, verts_z) + box0_translate;
	unsigned int *indices_box0 = new unsigned int[36]{ 0,1,2, 2,3,0, 0,3,4, 4,5,0, 4,5,6, 6,7,4, 7,6,1, 1,2,7, 7,4,3, 3,2,7, 0,5,6, 6,1,0 };
	Mesh *box0 = new Mesh(verts_box0, 0, 0, indices_box0, 8, 12);
	box0->name = "box0";
	scene->objects.push_back(box0);

	V3f *verts_box1 = new V3f[8];
	verts_x = 0.4f, verts_y = 0.4f, verts_z = 0.4f;
	V3f box1_translate(-1.4f, -0.2f, -4.5f);
	verts_box1[0] = V3f(-verts_x, -verts_y, -verts_z) + box1_translate;
	verts_box1[1] = V3f(verts_x, -verts_y, -verts_z) + box1_translate;
	verts_box1[2] = V3f(verts_x, verts_y, -verts_z) + box1_translate;
	verts_box1[3] = V3f(-verts_x, verts_y, -verts_z) + box1_translate;
	verts_box1[4] = V3f(-verts_x, verts_y, verts_z) + box1_translate;
	verts_box1[5] = V3f(-verts_x, -verts_y, verts_z) + box1_translate;
	verts_box1[6] = V3f(verts_x, -verts_y, verts_z) + box1_translate;
	verts_box1[7] = V3f(verts_x, verts_y, verts_z) + box1_translate;
	unsigned int *indices_box1 = new unsigned int[36]{ 0,1,2, 2,3,0, 0,3,4, 4,5,0, 4,5,6, 6,7,4, 7,6,1, 1,2,7, 7,4,3, 3,2,7, 0,5,6, 6,1,0 };
	Mesh *box1 = new Mesh(verts_box1, 0, 0, indices_box1, 8, 12);
	box1->name = "box1";
	scene->objects.push_back(box1);

	Mesh *teapot01 = new Mesh();
	teapot01->name = "teapot01";
	teapot01->LoadFromBin("./geometry/teapot1K.bin");
	teapot01->Scale(0.015f);
	teapot01->Translate(V3f(0.17f, -0.5f, -3.5f));
	scene->objects.push_back(teapot01);

	Mesh *teapot02 = new Mesh();
	teapot02->name = "teapot02";
	teapot02->LoadFromBin("./geometry/teapot1K.bin");
	teapot02->Scale(0.01f);
	teapot02->Translate(V3f(0.57f, -0.5f, -2.2f));
	scene->objects.push_back(teapot02);

	AABB _floor;
	_floor.GenerateFromMesh(teapot01);
	V3f *verts_floor = new V3f[4];
	verts_floor[0] = _floor.corners_[0] + V3f(-2.5f, -0.3f, -5.5f);
	verts_floor[1] = verts_floor[0] + V3f(6.f, 0.f, 0.f);
	verts_floor[2] = verts_floor[1] + V3f(0.f, 0.f, 7.5f);
	verts_floor[3] = verts_floor[2] + V3f(-5.f, 0.f, 0.f);
	V3f *texcoords_floor = new V3f[4];
	texcoords_floor[0] = V3f(0.f, 0.f, 0.f);
	texcoords_floor[1] = V3f(1.f, 0.f, 0.f);
	texcoords_floor[2] = V3f(1.f, 1.f, 0.f);
	texcoords_floor[0] = V3f(0.f, 1.f, 0.f);
	V3f *normals_floor = new V3f[4];
	normals_floor[0] = normals_floor[1] = normals_floor[2] = normals_floor[3] = V3f(0.f, 1.f, 0.f);
	unsigned int *_floor_indices = new unsigned int[6]{
		0, 1, 3,
		1, 3, 2
	};
	Mesh *floor = new Mesh(verts_floor, texcoords_floor, normals_floor, _floor_indices, 4, 2);
	floor->name = "floor";
	scene->objects.push_back(floor);

	Framebuffer *texture00 = new Framebuffer("./textures/wood.jpg", "wood", 1);
	scene->textures.push_back(texture00);
	
	scene->billboard_camera = new Camera(800, 600, 50.f);
	scene->billboard_fb = new Framebuffer(scene->billboard_camera->width_, scene->billboard_camera->height_, "billboard", 0);
	V3f new_center = teapot01->GetCenterOfMass();
	V3f new_vd = (teapot02->GetCenterOfMass() - new_center).normalize();
	scene->billboard_camera->PositionAndOrient(new_center, new_center + new_vd * 100.f, scene->billboard_camera->b_ * -1.f);
	scene->PrecomputeBillboard();
	scene->textures.push_back(scene->billboard_fb);

	V3f bb_up = V3f(0.f, 1.f, 0.f);
	V3f bb_normal = V3f(0.f, 0.f, -1.f);
	V3f target_normal = new_vd * -1.f;
	float _angle = acos(bb_normal.dot(target_normal)) * 180.f / 3.1415936f;
	V3f bb_center = teapot02->GetCenterOfMass();
	V3f *verts_billboard = new V3f[4];
	float h = 0.6f, w = 0.8f;
	verts_billboard[0] = bb_center + V3f(-w, h, 0.f);
	verts_billboard[1] = bb_center + V3f(w, h, 0.f);
	verts_billboard[2] = bb_center + V3f(w, -h, 0.f);
	verts_billboard[3] = bb_center + V3f(-w, -h, 0.f);
	V3f *texcoords_billboard = new V3f[4];
	texcoords_billboard[0] = V3f(1.f, 1.f, 0.f);
	texcoords_billboard[1] = V3f(0.f, 1.f, 0.f);
	texcoords_billboard[2] = V3f(0.f, 0.f, 0.f);
	texcoords_billboard[3] = V3f(1.f, 0.f, 0.f);
	V3f *normals_billboard = new V3f[4];
	normals_billboard[0] = normals_billboard[1] = normals_billboard[2] = normals_billboard[3] = bb_normal;
	unsigned int *indices = new unsigned int[6]{
		0, 1, 2,
		2, 3, 0
	};

	Mesh *billboard = new Mesh(verts_billboard, texcoords_billboard, normals_billboard, indices, 4, 2);
	billboard->name = "billboard";
	billboard->RotateAboutAxis(billboard->GetCenterOfMass(), bb_up, -_angle);
	scene->billboard_verts_ = verts_billboard;
}

void Display() {
#if USING_DEMO == HARDWARE_DEMO
	scene->RenderGPU();
#elif USING_DEMO == SOFTWARE_EM_DEMO
	scene->ShowEM();
	scene->frame++;
	glDrawPixels(scene->fb00_->width_, scene->fb00_->height_, GL_BGRA, GL_UNSIGNED_BYTE, scene->fb00_->pixels_);
#elif USING_DEMO == SOFTWARE_SM_PTM_DEMO
	scene->Show();
	scene->frame++;
	glDrawPixels(scene->fb00_->width_, scene->fb00_->height_, GL_BGRA, GL_UNSIGNED_BYTE, scene->fb00_->pixels_);
#endif	
	glutSwapBuffers();
	glutPostRedisplay();
	
}

void DisplayHW() {
	glDrawPixels(scene->billboard_fb->width_, scene->billboard_fb->height_, GL_BGRA, GL_UNSIGNED_BYTE, scene->billboard_fb->pixels_);
	glutSwapBuffers();
	glutPostRedisplay();
}

void keyboard_up(unsigned char key, int x, int y) {
	ImGui_ImplGlut_KeyUpCallback(key);
}

void special_up(int key, int x, int y) {
	ImGui_ImplGlut_SpecialUpCallback(key);
}

void passive(int x, int y) {
	ImGui_ImplGlut_PassiveMouseMotionCallback(x, y);
}

void special(int key, int x, int y) {
	ImGui_ImplGlut_SpecialCallback(key);
}

void motion(int x, int y) {
	ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y) {
	ImGui_ImplGlut_MouseButtonCallback(button, state);
}

void KeyboardHW(unsigned char input, int x, int y) {
	ImGui_ImplGlut_KeyCallback(input);
	Camera *camera = scene->camera00_;
	switch (input)
	{
	
	case 'a': {
		camera->Translation(camera->a_.normalize() * (-STEP));
		break;
	}
	case 'd': {
		camera->Translation(camera->a_.normalize() * (STEP));
		break;
	}
	case 'w': {
		camera->Translation(camera->b_.normalize() * (-STEP));
		break;
	}
	case 's': {
		camera->Translation(camera->b_.normalize() * (STEP));
		break;
	}
	case 'q': {
		camera->Translation(camera->GetViewDirection() * (-STEP));
		break;
	}
	case 'e': {
		camera->Translation(camera->GetViewDirection() * (STEP));
		break;
	}
	case 'r': {
		camera->Pan(DSTEP);
		break;
	}
	case 'f': {
		camera->Pan(-DSTEP);
		break;
	}
	case 't': {
		camera->Tilt(DSTEP);
		break;
	}
	case 'g': {
		camera->Tilt(-DSTEP);
		break;
	}
	case 'y': {
		camera->Roll(DSTEP);
		break;
	}
	case 'h': {
		camera->Roll(-DSTEP);
		break;
	}
	case 'z': {
		for (auto object : scene->objects) {
			if (object->isFill_) object->isFill_ = false;
			else object->isFill_ = true;
		}
		break;
	}
	case 'x': {
		if (scene->shadow_passf_ == 0.f) scene->shadow_passf_ = 1.f;
		else scene->shadow_passf_ = 0.f;
		break;
	}
	default:
		break;
	}
	glutPostRedisplay();
}

void KeyboardSW_EM(unsigned char input, int x, int y) {
	ImGui_ImplGlut_KeyCallback(input);
	Camera *camera = scene->camera00_;
	switch (input)
	{
	case 'a': {
		camera->Translation(camera->a_.normalize() * (-STEP));
		break;
	}
	case 'd': {
		camera->Translation(camera->a_.normalize() * (STEP));
		break;
	}
	case 'w': {
		camera->Translation(camera->b_.normalize() * (-STEP));
		break;
	}
	case 's': {
		camera->Translation(camera->b_.normalize() * (STEP));
		break;
	}
	case 'q': {
		camera->Translation(camera->GetViewDirection() * (-STEP));
		break;
	}
	case 'e': {
		camera->Translation(camera->GetViewDirection() * (STEP));
		break;
	}
	case 'r': {
		camera->Pan(DSTEP);
		break;
	}
	case 'f': {
		camera->Pan(-DSTEP);
		break;
	}
	case 't': {
		camera->Tilt(DSTEP);
		break;
	}
	case 'g': {
		camera->Tilt(-DSTEP);
		break;
	}
	case 'y': {
		camera->Roll(DSTEP);
		break;
	}
	case 'h': {
		camera->Roll(-DSTEP);
		break;
	}
	case 'z': {
		for (auto obj : scene->objects) {
			if (obj->fresnel_enable_) {
				obj->fresnel_enable_ = false;
			} else {
				obj->fresnel_enable_ = true;
			}
		}
		break;
	}
	case 'x': {
		for (auto obj : scene->objects) {
			obj->kfresnel_ += 0.1f;
			if (obj->kfresnel_ > 1.f) obj->kfresnel_ = 1.f;
		}
		break;
	}
	case 'X': {
		for (auto obj : scene->objects) {
			obj->kfresnel_ -= 0.1f;
			if (obj->kfresnel_ < 0.f) obj->kfresnel_ = 0.f;
		}
		break;
	}
	case 'j': {
		Camera *_camera = scene->camera00_;
		for (auto obj : scene->objects) {
			if (obj->name == "teapot01") {
				_camera->center_ = _camera->center_.RotatePointAboutAxis(obj->GetCenterOfMass(), V3f(0.f, 1.f, 0.f), 2.5f);
				camera->Pan(2.5f);
				break;
			}
		}
		break;
	}
	case 'l': {
		Camera *_camera = scene->camera00_;
		for (auto obj : scene->objects) {
			if (obj->name == "teapot01") {
				_camera->center_ = _camera->center_.RotatePointAboutAxis(obj->GetCenterOfMass(), V3f(0.f, 1.f, 0.f), -2.5f);
				camera->Pan(-2.5f);
				break;
			}
		}
		break;
	}
	case 'i': {
		Camera *_camera = scene->camera00_;
		for (auto obj : scene->objects) {
			if (obj->name == "teapot01") {
				_camera->center_ = _camera->center_.RotatePointAboutAxis(obj->GetCenterOfMass(), V3f(1.f, 0.f, 0.f), 2.5f);
				camera->Tilt(2.5f);
				break;
			}
		}
		break;
	}
	case 'k': {
		Camera *_camera = scene->camera00_;
		for (auto obj : scene->objects) {
			if (obj->name == "teapot01") {
				_camera->center_ = _camera->center_.RotatePointAboutAxis(obj->GetCenterOfMass(), V3f(1.f, 0.f, 0.f), -2.5f);
				camera->Tilt(-2.5f);
				break;
			}
		}
		break;
	}
	default:
		break;
	}
	glutPostRedisplay();
}

void KeyboardSW_SM_PTM(unsigned char input, int x, int y) {
	ImGui_ImplGlut_KeyCallback(input);
	Camera *camera = scene->camera00_;
	switch (input)
	{
	case '`': {
		if (scene->projector->enable) scene->projector->enable = false;
		else scene->projector->enable = true;
		break;
	}
	case '1': {
		for (auto light : scene->lights) {
			if (light->enable && light->name != "projector00" && light->name != "projectorP") {
				light->enable = false;
				break;
			}
		}
		break;
	}
	case '2': {
		for (auto light : scene->lights) {
			if (!(light->enable) && light->name != "projector00" && light->name != "projectorP") {
				light->enable = true;
				break;
			}
		}
		break;
	}
	case '3': {
		if (scene->invis->enable) scene->invis->enable = false;
		else scene->invis->enable = true;
		break;
	}
	case 'S': {
		scene->projector->pos_ = scene->projector->pos_ + V3f(0.0f, 0.f, 0.1f);
		scene->projector->needUpdate = true;
		break;
	}
	case 'W': {
		scene->projector->pos_ = scene->projector->pos_ + V3f(0.0f, 0.f, -0.1f);
		scene->projector->needUpdate = true;
		break;
	}
	case 'A': {
		scene->projector->pos_ = scene->projector->pos_ + V3f(-0.1f, 0.f, 0.0f);
		scene->projector->needUpdate = true;
		break;
	}
	case 'D': {
		scene->projector->pos_ = scene->projector->pos_ + V3f(0.1f, 0.f, 0.0f);
		scene->projector->needUpdate = true;
		break;
	}
	case 'a': {
		camera->Translation(camera->a_.normalize() * (-STEP));
		break;
	}
	case 'd': {
		camera->Translation(camera->a_.normalize() * (STEP));
		break;
	}
	case 'w': {
		camera->Translation(camera->b_.normalize() * (-STEP));
		break;
	}
	case 's': {
		camera->Translation(camera->b_.normalize() * (STEP));
		break;
	}
	case 'q': {
		camera->Translation(camera->GetViewDirection() * (-STEP));
		break;
	}
	case 'e': {
		camera->Translation(camera->GetViewDirection() * (STEP));
		break;
	}
	case 'r': {
		camera->Pan(DSTEP);
		break;
	}
	case 'f': {
		camera->Pan(-DSTEP);
		break;
	}
	case 't': {
		camera->Tilt(DSTEP);
		break;
	}
	case 'g': {
		camera->Tilt(-DSTEP);
		break;
	}
	case 'y': {
		camera->Roll(DSTEP);
		break;
	}
	case 'h': {
		camera->Roll(-DSTEP);
		break;
	}
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
#if USING_DEMO == HARDWARE_DEMO
	InitializeShaderProgram();
#elif USING_DEMO == SOFTWARE_EM_DEMO
	InitializeEnvrionmentMapping();
#elif USING_DEMO == SOFTWARE_SM_PTM_DEMO
	InitializeSMandPTMProgram();
#endif
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("CS535 ver006 Remake");
	glutDisplayFunc(Display);
#if USING_DEMO == HARDWARE_DEMO
	glutKeyboardFunc(KeyboardHW);
#elif USING_DEMO == SOFTWARE_EM_DEMO
	glutKeyboardFunc(KeyboardSW_EM);
#elif USING_DEMO == SOFTWARE_SM_PTM_DEMO
	glutKeyboardFunc(KeyboardSW_SM_PTM);
#endif
	glutSpecialFunc(special);
	glutKeyboardUpFunc(keyboard_up);
	glutSpecialUpFunc(special_up);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);
#if USING_DEMO == HARDWARE_DEMO
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(820, 20);
	glutCreateWindow("Display2");
	glutDisplayFunc(DisplayHW);
#endif
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: initialize glew failed");
		exit(0);
	}

	glutMainLoop();
	return 0;
}