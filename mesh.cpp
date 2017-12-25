#include "mesh.h"

#include <fstream>
#include <vector>
using namespace std;
#include <math.h>
#include <FreeImage.h>
#include <GL\glew.h>
#include <GL\freeglut.h>

#define PI 3.14159265

Mesh::Mesh(V3f *verts, V3f *texcoords, V3f *normals, unsigned int *indices, int num_verts, int num_tris) : Mesh() {
	verts_ = verts;
	num_verts_ = num_verts;
	triangles_ = indices;
	num_triangles_ = num_tris;
	texcoords_ = texcoords;
	normals_ = normals;
}

void Mesh::LoadFromBin(const string &filename) { // Refrence: class note
	ifstream ifs(filename, ios::binary);
	if (ifs.fail()) {
		cout << "Cannot open file" << filename << endl;
		return;
	}

	ifs.read((char*)&num_verts_, sizeof(int));

	char buffer;
	ifs.read(&buffer, 1);
	if (buffer != 'y')	return;

	if (verts_) delete verts_;
	verts_ = new V3f[num_verts_];

	ifs.read(&buffer, 1);
	if (colors_) delete colors_;
	colors_ = 0;
	if (buffer == 'y') colors_ = new V3f[num_verts_];

	ifs.read(&buffer, 1);
	if (normals_) delete normals_;
	normals_ = 0;
	if (buffer == 'y') normals_ = new V3f[num_verts_];

	ifs.read(&buffer, 1);
	float *texcoords = 0;
	if (texcoords) delete texcoords;
	texcoords_ = 0;
	if (buffer == 'y') texcoords = new float[num_verts_ * 2];

	if (verts_) {
		ifs.read((char*)verts_, num_verts_ * 3 * sizeof(float));
	}
	if (colors_) {
		ifs.read((char*)colors_, num_verts_ * 3 * sizeof(float));
	}
	if (normals_) {
		ifs.read((char*)normals_, num_verts_ * 3 * sizeof(float));
	}
	if (texcoords) {
		ifs.read((char*)texcoords, num_verts_ * 2 * sizeof(float));
		texcoords_ = new V3f[num_verts_];
		for (int i = 0; i < num_verts_; i++) {
			texcoords_[i][0] = texcoords[i * 2 + 0];
			texcoords_[i][1] = texcoords[i * 2 + 1];
			texcoords_[i][2] = 0.f;
		}
	}
		
	ifs.read((char*)&num_triangles_, sizeof(int));
	if (triangles_) delete triangles_;
	triangles_ = new unsigned int[num_triangles_ * 3];
	ifs.read((char*)triangles_, num_triangles_ * 3 * sizeof(unsigned int));

	ifs.close();

	delete[] texcoords;
}

void Mesh::RenderWireframe(Camera *camera, Framebuffer *fb) {
	for (int tri = 0; tri < num_triangles_; tri++) {
		for (int ei = 0; ei < 3; ei++) {
			int ei1 = (ei + 1) % 3;
			V3f V0 = verts_[triangles_[3 * tri + ei]];
			V3f V1 = verts_[triangles_[3 * tri + ei1]];
			V3f c0 = colors_[triangles_[3 * tri + ei]];
			V3f c1 = colors_[triangles_[3 * tri + ei1]];
			fb->Draw3DSegment(V0, V1, c0, c1, camera);
		}
	}
}

void Mesh::RenderFilledObject(Camera *camera, Framebuffer *fb, Framebuffer *texture, bool lit, int mode, vector<Light*> *lights) {
	V3f *toproject_verts = new V3f[num_verts_];
	for (int vi = 0; vi < num_verts_; vi++) {
		if (!camera->Projection(verts_[vi], toproject_verts[vi]))
			toproject_verts[vi] = V3f(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	for (int tri = 0; tri < num_triangles_; tri++) {
		V3f point_triangle[3];
		M33 tri_texcoords, tri_colors, m_verts, m_normals;
		point_triangle[0] = toproject_verts[triangles_[3 * tri + 0]];
		point_triangle[1] = toproject_verts[triangles_[3 * tri + 1]];
		point_triangle[2] = toproject_verts[triangles_[3 * tri + 2]];

		if (point_triangle[0][0] == FLT_MAX || 
			point_triangle[1][0] == FLT_MAX || 
			point_triangle[2][0] == FLT_MAX)
			continue;
		
		AABB aabb(point_triangle[0]);
		aabb.Generate(point_triangle[1]);
		aabb.Generate(point_triangle[2]);
		V3f minc(0.0f, 0.0f, 0.0f);
		V3f maxc((float)800, (float)600, 0.0f);
		V3f eeqs[3];
		for (int i = 0; i < 3; i++) {
			eeqs[i] = SetEEQs(point_triangle[i], point_triangle[(i + 1) % 3], point_triangle[(i + 2) % 3]);
		}
		if (!aabb.Clip(minc, maxc, 2))
			continue;
		
		int left = static_cast<int>(aabb.corners_[0][0] + 0.5f);
		int right = static_cast<int>(aabb.corners_[1][0] - 0.5f);
		int top = static_cast<int>(aabb.corners_[0][1] + 0.5f);
		int bottom = static_cast<int>(aabb.corners_[1][1] - 0.5f);

		if (!normals_) {
			V3f _a = verts_[triangles_[3 * tri + 1]] - verts_[triangles_[3 * tri + 0]];
			V3f _b = verts_[triangles_[3 * tri + 2]] - verts_[triangles_[3 * tri + 0]];
			V3f _c = _a.cross(_b);
			V3f normal(_c.normalize());
			m_normals[0] = m_normals[1] = m_normals[2] = normal;
		} else {
			m_normals[0] = normals_[triangles_[3 * tri + 0]];
			m_normals[1] = normals_[triangles_[3 * tri + 1]];
			m_normals[2] = normals_[triangles_[3 * tri + 2]];
		}

		m_verts[0] = verts_[triangles_[3 * tri + 0]];
		m_verts[1] = verts_[triangles_[3 * tri + 1]];
		m_verts[2] = verts_[triangles_[3 * tri + 2]];

		M33 ssim = GetScreenSpaceMarix(point_triangle);
		M33 msim = GetModelSapceMatrix(m_verts, camera);
		V3f denm = msim[0] + msim[1] + msim[2];

		vector<V3f> rgb;
		if (mode == 1 && colors_) {
			tri_colors[0] = colors_[triangles_[3 * tri + 0]];
			tri_colors[1] = colors_[triangles_[3 * tri + 1]];
			tri_colors[2] = colors_[triangles_[3 * tri + 2]];
		} else if (mode == 2 && texcoords_) {
			tri_texcoords[0] = texcoords_[triangles_[3 * tri + 0]];
			tri_texcoords[1] = texcoords_[triangles_[3 * tri + 1]];
			tri_texcoords[2] = texcoords_[triangles_[3 * tri + 2]];
		}

		V3f to_screen_z = ssim * V3f(point_triangle[0][2], point_triangle[1][2], point_triangle[2][2]);
		M33 bc_texcoords = (ssim * tri_texcoords).Transpositon(); // barycentric coord abc for (u, v, 0)
		M33 bc_colors = (ssim * tri_colors).Transpositon();
		M33 m_bc_colors = tri_colors.Transpositon() * msim;
		M33 m_bc_normals = m_normals.Transpositon() * msim;
		M33 m_bc_texcoords = tri_texcoords.Transpositon() * msim;


		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3f pixel(.5f + (float)u, .5f + (float)v, 1.0f);

				if (pixel.dot(eeqs[0]) < 0.0f ||
					pixel.dot(eeqs[1]) < 0.0f ||
					pixel.dot(eeqs[2]) < 0.0f)
					continue;

				float realz = pixel.dot(to_screen_z);
				
				if (!fb->IsUpdatePixel(u, v, realz)) continue;

				if (mode == 0) { // depth only
					continue;
				}

				V3f normal;
				if (!normals_) normal = m_normals[0];
				else normal = (m_bc_normals * pixel) / (denm.dot(pixel));


				V3f flcolor(0.f, 0.f, 0.f);
				if (mode == 1) { // color mapping
					if (colors_) {
						// flcolor = bc_colors * pixel; //ssim
						flcolor = (m_bc_colors * pixel) / (denm.dot(pixel));
						// cout << flcolor << endl;
					}
				} else if (mode == 2) {// texture mapping
					if (texture && texcoords_) {
						V3f tomap = m_bc_texcoords * pixel / (denm.dot(pixel)); // msim
						// V3f tomap = bc_texcoords * pixel; //ssim
						flcolor.HexToV3f(texture->Lookup(tomap));
					}
				}

				if (lit) { // lighting on/off
					int num_lights = lights->size();
					float weight = 1.f / num_lights;
					V3f *flcolors = new V3f[num_lights];
					V3f ppoint(pixel[0], pixel[1], realz);
					V3f realpoint = camera->Unproject(ppoint);
					V3f blend = V3f(0.f, 0.f, 0.f);
					for (int i = 0; i < num_lights; i++) {
						flcolors[i] = flcolor;
						Light *light = (*lights)[i];
						if (light->shadowmap_) {// shadow
							V3f lightpoint = V3f(0.0f, 0.0f, 0.0f);
							if (light->camera_->Projection(realpoint, lightpoint)) {
								if (light->shadowmap_->ShadowTest(lightpoint, 1.5f)) {
									flcolors[i] = flcolors[i] * ka;
								} else {
									if (light->imgcolor_) {// projecting image
										V3f proj_color(0.f, 0.f, 0.f);
										if (!(lightpoint[0] < 0.f ||
											lightpoint[0] > light->imgcolor_->width_ - 1 ||
											lightpoint[1] < 0.f ||
											lightpoint[1] > light->imgcolor_->height_ - 1)) {
												proj_color.HexToV3f(light->imgcolor_->GetPixel(lightpoint[0], lightpoint[1]));
												float alpha = light->imgcolor_->GetPixelAlpha(lightpoint[0], lightpoint[1]);
												flcolors[i] = proj_color * alpha + flcolors[i] * (1.f - alpha);
										}
									}
									if (light->name != "projectorP")
									flcolors[i] = PhongLit(flcolors[i], normal, *light, camera->center_, realpoint);
								}
							}
						}
						blend = blend + flcolors[i] * weight;
					}
					flcolor = blend;
					delete[] flcolors;
				}
				pixel[2] = realz;
				fb->TestZBuffer(pixel, flcolor.V3fToHex());
			}
		}


	}
	delete[] toproject_verts;
}

void Mesh::RenderFilledObject(Camera *camera, Framebuffer *fb, CubeMap *cube, bool lit, int mode, vector<Light*> *lights) {
	V3f *toproject_verts = new V3f[num_verts_];
	for (int vi = 0; vi < num_verts_; vi++) {
		if (!camera->Projection(verts_[vi], toproject_verts[vi]))
			toproject_verts[vi] = V3f(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	for (int tri = 0; tri < num_triangles_; tri++) {
		V3f point_triangle[3];
		M33 tri_texcoords, tri_colors, m_verts, m_normals;
		point_triangle[0] = toproject_verts[triangles_[3 * tri + 0]];
		point_triangle[1] = toproject_verts[triangles_[3 * tri + 1]];
		point_triangle[2] = toproject_verts[triangles_[3 * tri + 2]];

		if (point_triangle[0][0] == FLT_MAX ||
			point_triangle[1][0] == FLT_MAX ||
			point_triangle[2][0] == FLT_MAX)
			continue;

		AABB aabb(point_triangle[0]);
		aabb.Generate(point_triangle[1]);
		aabb.Generate(point_triangle[2]);
		V3f minc(0.0f, 0.0f, 0.0f);
		V3f maxc((float)800, (float)600, 0.0f);
		V3f eeqs[3];
		for (int i = 0; i < 3; i++) {
			eeqs[i] = SetEEQs(point_triangle[i], point_triangle[(i + 1) % 3], point_triangle[(i + 2) % 3]);
		}
		if (!aabb.Clip(minc, maxc, 2))
			continue;

		int left = static_cast<int>(aabb.corners_[0][0] + 0.5f);
		int right = static_cast<int>(aabb.corners_[1][0] - 0.5f);
		int top = static_cast<int>(aabb.corners_[0][1] + 0.5f);
		int bottom = static_cast<int>(aabb.corners_[1][1] - 0.5f);

		if (!normals_) {
			V3f _a = verts_[triangles_[3 * tri + 1]] - verts_[triangles_[3 * tri + 0]];
			V3f _b = verts_[triangles_[3 * tri + 2]] - verts_[triangles_[3 * tri + 0]];
			V3f _c = _a.cross(_b);
			V3f normal(_c.normalize());
			m_normals[0] = m_normals[1] = m_normals[2] = normal;
		} else {
			m_normals[0] = normals_[triangles_[3 * tri + 0]];
			m_normals[1] = normals_[triangles_[3 * tri + 1]];
			m_normals[2] = normals_[triangles_[3 * tri + 2]];
		}

		m_verts[0] = verts_[triangles_[3 * tri + 0]];
		m_verts[1] = verts_[triangles_[3 * tri + 1]];
		m_verts[2] = verts_[triangles_[3 * tri + 2]];

		M33 ssim = GetScreenSpaceMarix(point_triangle);
		M33 msim = GetModelSapceMatrix(m_verts, camera);
		V3f denm = msim[0] + msim[1] + msim[2];

		vector<V3f> rgb;
		if (!texcoords_ && colors_) {
			tri_colors[0] = colors_[triangles_[3 * tri + 0]];
			tri_colors[1] = colors_[triangles_[3 * tri + 1]];
			tri_colors[2] = colors_[triangles_[3 * tri + 2]];
		} else if (texcoords_) {
			tri_texcoords[0] = texcoords_[triangles_[3 * tri + 0]];
			tri_texcoords[1] = texcoords_[triangles_[3 * tri + 1]];
			tri_texcoords[2] = texcoords_[triangles_[3 * tri + 2]];
		}

		V3f to_screen_z = ssim * V3f(point_triangle[0][2], point_triangle[1][2], point_triangle[2][2]);
		M33 bc_texcoords = (ssim * tri_texcoords).Transpositon(); // barycentric coord abc for (u, v, 0)
		M33 bc_colors = (ssim * tri_colors).Transpositon();
		M33 m_bc_colors = tri_colors.Transpositon() * msim;
		M33 m_bc_normals = m_normals.Transpositon() * msim;
		M33 m_bc_texcoords = tri_texcoords.Transpositon() * msim;


		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3f pixel(.5f + (float)u, .5f + (float)v, 1.0f);

				if (pixel.dot(eeqs[0]) < 0.0f ||
					pixel.dot(eeqs[1]) < 0.0f ||
					pixel.dot(eeqs[2]) < 0.0f)
					continue;

				float realz = pixel.dot(to_screen_z);

				if (!fb->IsUpdatePixel(u, v, realz)) continue;

				if (mode == 0) { // depth only
					continue;
				}

				V3f normal;
				if (!normals_) normal = m_normals[0];
				else normal = (m_bc_normals * pixel) / (denm.dot(pixel));


				V3f flcolor(0.f, 0.f, 0.f);
				if (mode == 1) { // color mapping
					if (colors_) {
						flcolor = bc_colors * pixel; //ssim
													 // color = ((m_bc_colors * pixel)/ (denm.dot(pixel)).V3fToHex();
					}
				} else if (mode == 2) {// Cube Mapping
					if (cube) {					
						V3f point_toproj = V3f(pixel[0], pixel[1], realz);
						V3f realpoint = camera->Unproject(point_toproj);
						V3f eye_dir = camera->center_ - realpoint;
						V3f eye_dir_normal = normal * eye_dir.dot(normal);
						V3f eye_reflect = (eye_dir_normal * 2.f - eye_dir).normalize();
						unsigned int cube_texture = cube->LookUp(eye_reflect);
						flcolor.HexToV3f(cube_texture);
						
						if (fresnel_enable_ && kfresnel_ != 0.f) {
							V3f ray_dir = realpoint - camera->center_;
							float etai = 1.0f;
							float etat = 0.6f;
							float cosi = ray_dir.dot(normal);
							if (cosi > 1.f) cosi = 1.f;
							else if (cosi < -1.f) cosi = -1.f;
							
							V3f _n = normal;
							if (cosi < 0.f) {
								cosi = -cosi;
							} else {
								swap(etai, etat);
								_n = _n * -1.f;
							}

							float eta = etai / etat;
							float k = 1.f - powf(eta, 2.f) * (1.f - powf(cosi, 2.f));
							V3f refract_fcolor(0.f, 0.f, 0.f);
							if (k >= 0.f) {
								V3f out_dir = (ray_dir * eta + _n * (cosi * eta - sqrtf(k)));
								unsigned int refract_color = cube->LookUp(out_dir);					
								refract_fcolor.HexToV3f(refract_color);
								flcolor = flcolor * (1.f - kfresnel_) + refract_fcolor * kfresnel_;
							} else {
								// cout << "k < 0" << endl;
							}
						}
					}
				}

				if (lit) { // lighting on/off
					int num_lights = lights->size();
					float weight = 1.f / num_lights;
					V3f *flcolors = new V3f[num_lights];
					V3f ppoint(pixel[0], pixel[1], realz);
					V3f realpoint = camera->Unproject(ppoint);
					V3f blend = V3f(0.f, 0.f, 0.f);
					for (int i = 0; i < num_lights; i++) {
						flcolors[i] = flcolor;
						Light *light = (*lights)[i];
						if (light->shadowmap_) {// shadow
							V3f lightpoint = V3f(0.0f, 0.0f, 0.0f);
							if (light->camera_->Projection(realpoint, lightpoint)) {
								if (light->shadowmap_->ShadowTest(lightpoint, 1.5f)) {
									flcolors[i] = flcolors[i] * ka;
								} else {
									if (light->imgcolor_) {// projecting image
										V3f proj_color(0.f, 0.f, 0.f);
										if (!(lightpoint[0] < 0.f ||
											lightpoint[0] > light->imgcolor_->width_ - 1 ||
											lightpoint[1] < 0.f ||
											lightpoint[1] > light->imgcolor_->height_ - 1)) {
											proj_color.HexToV3f(light->imgcolor_->GetPixel(lightpoint[0], lightpoint[1]));
											float alpha = light->imgcolor_->GetPixelAlpha(lightpoint[0], lightpoint[1]);
											flcolors[i] = proj_color * alpha + flcolors[i] * (1.f - alpha);
										}
									}
									if (light->name != "projectorP")
										flcolors[i] = PhongLit(flcolors[i], normal, *light, camera->center_, realpoint);
								}
							}
						}
						blend = blend + flcolors[i] * weight;
					}
					flcolor = blend;
					delete[] flcolors;
				}
				pixel[2] = realz;
				// cout << realz << endl;
				fb->TestZBuffer(pixel, flcolor.V3fToHex());
			}
		}


	}
	delete[] toproject_verts;
}

void Mesh::RenderSkybox(Camera *camera, Framebuffer *fb, CubeMap *cube) {
	for (int v = 0; v <= camera->height_ - 1; v++) {
		for (int u = 0; u <= camera->width_ - 1; u++) {
			V3f pixel(.5f + (float)u, .5f + (float)v, 10.0f);
			V3f flcolor(0.f, 0.f, 0.f);
			if (cube) {
				V3f realpoint = camera->Unproject(pixel);
				V3f dir = realpoint - camera->center_;
				unsigned int cube_texture = cube->LookUp(dir);
				flcolor.HexToV3f(cube_texture);

			}
			fb->SetPixel(int(pixel[0]), int(pixel[1]), flcolor.V3fToHex());
		}
	}
	fb->ClearZBuffer(0.0f);
}

void Mesh::RotateAboutAxis(V3f origin, V3f axis, float theta) {
	for (int vi = 0; vi < num_verts_; vi++) {
		V3f normal_top = verts_[vi] + normals_[vi];
		verts_[vi] = verts_[vi].RotatePointAboutAxis(origin, axis, theta);
		normal_top = normal_top.RotatePointAboutAxis(origin, axis, theta);
		normals_[vi] = (normal_top - verts_[vi]).normalize();
	}
}

V3f Mesh::GetCenterOfMass() {
	V3f res(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < num_verts_; vi++) {
		res = res + verts_[vi];
	}
	res = res / (float)num_verts_;
	return res;
}

void Mesh::Translate(V3f direction) {
	for (int vi = 0; vi < num_verts_; vi++) {
		verts_[vi] = verts_[vi] + direction;
	}
}

void Mesh::Scale(float k) {
	for (int vi = 0; vi < num_verts_; vi++) {
		verts_[vi] = verts_[vi] * k;
	}
}

V3f Mesh::SetEEQs(V3f v0, V3f v1, V3f v2) {
	V3f ret;
	ret[0] = v1[1] - v0[1];
	ret[1] = v0[0] - v1[0];
	ret[2] = -v0[0] * ret[0] + v0[1] * -ret[1];

	V3f v2p(v2[0], v2[1], 1.0f);
	if (v2p.dot(ret) < 0.0f)
		ret = ret * -1.0f;

	return ret;
}

AABB Mesh::GetAABB() {
	AABB ret(verts_[0], verts_[1]);
	for (int i = 0; i < num_verts_; i++) {
		ret.Generate(verts_[i]);
	}
	return ret;
}


M33 Mesh::GetScreenSpaceMarix(V3f *vertices) {
	M33 ssim;
	ssim.SetCol(0, V3f(vertices[0][0], vertices[1][0], vertices[2][0]));
	ssim.SetCol(1, V3f(vertices[0][1], vertices[1][1], vertices[2][1]));
	ssim.SetCol(2, V3f(1.0f, 1.0f, 1.0f));
	ssim = ssim.Inversion();
	return ssim;
}

M33 Mesh::GetModelSapceMatrix(M33 vs, Camera *camera) {
	M33 qm = vs.Transpositon();
	qm.SetCol(0, qm.GetCol(0) - camera->center_);
	qm.SetCol(1, qm.GetCol(1) - camera->center_);
	qm.SetCol(2, qm.GetCol(2) - camera->center_);

	M33 cam;
	cam.SetCol(0, camera->a_);
	cam.SetCol(1, camera->b_);
	cam.SetCol(2, camera->c_);

	qm = qm.Inversion() * cam;

	return qm;
}

V3f Mesh::PhongLit(V3f &color, V3f &normal, Light &light, V3f &eye, V3f &point) {
	V3f dir = (point - light.pos_).normalize();
	// V3f dir = V3f(0.f, -0.5f, -1.f).normalize();
	float kd = fmax(0.0f, (dir * -1.f).dot(normal));
	V3f eye_dir = eye - point;
	V3f eye_dir_normal = normal * eye_dir.dot(normal);
	V3f eye_reflect = (eye_dir_normal * 2.f - eye_dir).normalize();
	float ks = fmax(0.f, eye_reflect.dot(dir * -1.f));
	return color * (ka + (1.f - ka) * kd) + light.color_ * powf(ks, spec);
}

void Mesh::RenderHW() {
	if (isFill_) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	if (normals_)
		glEnableClientState(GL_NORMAL_ARRAY);
	if (texcoords_)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, (float*)verts_);
	glColorPointer(3, GL_FLOAT, 0, (float*)colors_);
	if (normals_)
		glNormalPointer(GL_FLOAT, 0, (float*)normals_);
	if (texcoords_)
		glTexCoordPointer(3, GL_FLOAT, 0, (float*)texcoords_);

	glDrawElements(GL_TRIANGLES, 3 * num_triangles_, GL_UNSIGNED_INT, triangles_);
	
	if (texcoords_)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if (normals_)
		glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}