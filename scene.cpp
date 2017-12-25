#include "scene.h"
#include "light.h"

#define Invisibility 3

void Scene::UpdateShadowMap(Light *light) {
	if (light != invis) {
		light->camera_->center_ = light->pos_;
		float fl = light->camera_->GetFocalLens();
		V3f view_dir = (objects[0]->GetCenterOfMass() - light->pos_).normalize();
		light->camera_->a_ = (view_dir.cross(V3f(0.0f, 1.0f, 0.0f))).normalize() * light->camera_->a_.length();
		light->camera_->b_ = (view_dir.cross(light->camera_->a_)).normalize() * light->camera_->b_.length();
		light->camera_->c_ = view_dir * fl - light->camera_->a_ * ((float)light->camera_->width_) / 2.0f - light->camera_->b_ * ((float)light->camera_->height_) / 2.0f;
		light->camera_->center_ = light->pos_;
		if (!light->imgcolor_) {
			AABB aabb = objects[0]->GetAABB();
			light->camera_->SetToAABB(aabb);
		}
	}
	light->shadowmap_->ClearZBuffer(0.f);
	light->shadowmap_->ClearBackground(V3f(0.f, 0.f, 0.f));
	for (auto object : objects) {
		object->RenderFilledObject(light->camera_, light->shadowmap_, (Framebuffer*)0, false, 0, NULL);
	}
	
}

void Scene::Show() {
	vector<Light*> enabled_lights;
	vector<Mesh*> enabled_objects;
	for (auto light : lights) {
		if (light->enable) enabled_lights.push_back(light);
	}
	for (auto object : objects) {
		if (object->enable) enabled_objects.push_back(object);
	}
	
	if (invis->enable) ShowInvisibility();

	fb00_->ClearBackground(V3f(0.8f, 0.8f, 0.8f));
	fb00_->ClearZBuffer(0.f);
	for (auto light : enabled_lights) {
		if (light->needUpdate) {
			UpdateShadowMap(light);
			light->needUpdate = false;
		}
		if (frame < 100) {
			if (light->name == "fourlights0") {
				light->pos_ = light->pos_ + V3f(0.01f, 0.f, 0.f);
				light->needUpdate = true;
			} else if (light->name == "fourlights1") {
				light->pos_ = light->pos_ + V3f(0.f, 0.f, 0.01f);
				light->needUpdate = true;
			} else if (light->name == "fourlights2") {
				light->pos_ = light->pos_ + V3f(-0.01f, 0.f, 0.f);
				light->needUpdate = true;
			} else if (light->name == "fourlights3") {
				light->pos_ = light->pos_ + V3f(0.0f, 0.f, -0.01f);
				light->needUpdate = true;
			}
		}
	}

	for (auto object : enabled_objects) {
		Framebuffer *texture;
		if (object->name == "teapot01") {
			texture = textures[0];
		} else if (object->name == "teapot02") {
			texture = textures[0];
		} else if (object->name == "floor") {
			texture = textures[1];
		} else {
			texture = textures[0];
		}
		if (texture) {
			object->RenderFilledObject(camera00_, fb00_, texture, true, 2, &enabled_lights);
		}	
		if (object->name == "teapot01") {
			object->RotateAboutAxis(object->GetCenterOfMass(), V3f(0.f, 1.f, 0.f), 1.2f);
			for (auto light : lights) light->needUpdate = true;
		}
	}
}

void Scene::ShowEM() { // EM demo
	vector<Mesh*> enabled_objects;
	for (auto object : objects) {
		if (object->enable) enabled_objects.push_back(object);
	}
	fb00_->ClearBackground(V3f(0.8f, 0.8f, 0.8f));
	fb00_->ClearZBuffer(0.f);
	skybox_->RenderSkybox(camera00_, fb00_, cube_texture_);

	for (auto object : enabled_objects) {
		if (object->name == "teapot01") {
			object->RenderFilledObject(camera00_, fb00_, cube_texture_, false, 2, 0);
			object->RotateAboutAxis(object->GetCenterOfMass(), V3f(0.f, 1.f, 0.f), 1.2f);
			for (auto light : lights) light->needUpdate = true;
		}
	}
}

void Scene::ShowInvisibility() { // Invisibility Demo
	vector<Light*> enabled_lights;
	vector<Mesh*> enabled_objects;
	for (auto light : lights) {
		if (light->enable && light != tmp) enabled_lights.push_back(light);
	}
	for (auto object : objects) {
		if (object->enable && object->name != "teapot1") enabled_objects.push_back(object);
	}
	
	for (auto object : enabled_objects) {// render without teapot01
		if (!(object->enable)) continue;
		if (object->name != "teapot01") {
			Framebuffer *texture;

			if (object->name == "teapot02") {
				texture = textures[0];
			} else if (object->name == "floor") {
				texture = textures[1];
			}

			object->RenderFilledObject(invis->camera_, invis->imgcolor_, texture, true, 2, &enabled_lights);
		}
	}
}

void Scene::RenderGPU() {
	if (!cgi_) {
		cgi_ = new CGInterface();
		cgi_->PerSessionInit();
		soi_ = new ShaderOneInterface();
		soi_->faces_ = cube_texture_->faces_;
		soi_->PerSessionInit(cgi_, textures);
	}
	frame++;
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT);

	camera00_->SetIntrinsicsHW(1.0f, 1000.0f);
	camera00_->SetExtrinsicsHW();

	cgi_->EnableProfiles();
	soi_->PerFrameInit();
	soi_->BindPrograms();
	soi_->EnableTexture();

	if (billboard_verts_)
		soi_->SetBillboard(billboard_verts_);
	soi_->SetEye(camera00_->center_);
	soi_->SetShadowPass(shadow_passf_);
	for (auto object : objects) {
		if (object->name == "box0") {
			soi_->SetSoftshadow(object->verts_, 0);
		}else if (object->name == "box1") {
			soi_->SetSoftshadow(object->verts_, 1);
		}
	}

	for (auto object : objects) {
		if (!object->enable) continue;
		if (object->name == "teapot01") soi_->SetPass(1.0f);
		else if (object->name == "billboard") soi_->SetPass(2.0f);
		else if (object->name == "teapot02")soi_->SetPass(3.0f);
		else if (object->name == "skybox") soi_->SetPass(4.0f);
		else if (object->name == "floor") soi_->SetPass(5.0f);
		else if (object->name == "box0") soi_->SetPass(6.0f);
		else if (object->name == "box1") soi_->SetPass(7.0f);
		else  soi_->SetPass(8.0f);
		if (object->name == "skybox") glDisable(GL_DEPTH_TEST);
		if (object->name == "box0" || object->name == "box1") {
			object->Translate(V3f(0.06f, 0.f, 0.f) * sinf(frame / 20.f));
		}
		object->RenderHW();
		if(object->name == "skybox") glEnable(GL_DEPTH_TEST);
	}
	soi_->DisableTexture();
	cgi_->DisableProfiles();
}

void Scene::PrecomputeBillboard() {
	vector<Mesh*> enabled_objects;
	for (auto object : objects) {
		if (object->enable) enabled_objects.push_back(object);
	}

	billboard_fb->ClearBackground(V3f(1.0f, 1.0f, 1.0f));
	billboard_fb->ClearZBuffer(0.f);

	for (auto object : enabled_objects) {
		if (object->name == "teapot01" || object->name == "skybox" || object->name == "floor") continue;
		if (object->name == "teapot02")
			object->RenderFilledObject(billboard_camera, billboard_fb, textures[0], false, 1, 0);
	}
}