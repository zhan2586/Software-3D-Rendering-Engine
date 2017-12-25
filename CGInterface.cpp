//#define GEOM_SHADER

#include "CGInterface.h"
#include "v3f.h"

#include <iostream>

using namespace std;

GLuint texture_obj;
GLuint billboard_obj;
GLuint depthBillboard_obj;
GLuint cubemap_obj;

CGInterface::CGInterface() {};

void CGInterface::PerSessionInit() {

  glEnable(GL_DEPTH_TEST);

  CGprofile latestVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
  CGprofile latestPixelProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

  CGerror Error = cgGetError();
  if (Error) {
	  cerr << "CG ERROR: " << cgGetErrorString(Error) << endl;
  }

  cout << "Info: Latest VP Profile Supported: " << cgGetProfileString(latestVertexProfile) << endl;
  cout << "Info: Latest FP Profile Supported: " << cgGetProfileString(latestPixelProfile) << endl;

  vertexCGprofile = latestVertexProfile;
  pixelCGprofile = latestPixelProfile;
  cgContext = cgCreateContext();  


}

bool ShaderOneInterface::PerSessionInit(CGInterface *cgi, vector<Framebuffer*> &textures) {

#ifdef GEOM_SHADER
  geometryProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, 
    "CG/shaderOne.cg", cgi->geometryCGprofile, "GeometryMain", NULL);
  if (geometryProgram == NULL)  {
    CGerror Error = cgGetError();
    cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
    cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
    return false;
  }
#endif

  vertexProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, 
    "CG/shaderOne.cg", cgi->vertexCGprofile, "VertexMain", NULL);
  if (vertexProgram == NULL) {
    CGerror Error = cgGetError();
    cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
    cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
    return false;
  }

  fragmentProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, 
    "CG/shaderOne.cg", cgi->pixelCGprofile, "FragmentMain", NULL);
  if (fragmentProgram == NULL)  {
    CGerror Error = cgGetError();
    cerr << "Shader One Fragment Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
    cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
    return false;
  }

	// load programs
#ifdef GEOM_SHADER
	cgGLLoadProgram(geometryProgram);
#endif
	cgGLLoadProgram(vertexProgram);
	cgGLLoadProgram(fragmentProgram);

	// build some parameters by name such that we can set them later...
	vertexModelViewProj = cgGetNamedParameter(vertexProgram, "modelViewProj" );
	textureParameter = cgGetNamedParameter(fragmentProgram, "tex0");
	billboardParameter = cgGetNamedParameter(fragmentProgram, "billboard");
	passParameter = cgGetNamedParameter(fragmentProgram, "passf");
	bbVertexParameter = cgGetNamedParameter(fragmentProgram, "bb_verts");
	eyeParameter = cgGetNamedParameter(fragmentProgram, "eye");
	depthBillboardParameter = cgGetNamedParameter(fragmentProgram, "depth_billboard");
	cubeTextureParameter = cgGetNamedParameter(fragmentProgram, "cube_texture");
	box0vertsParameter = cgGetNamedParameter(fragmentProgram, "box0_verts");
	box1vertsParameter = cgGetNamedParameter(fragmentProgram, "box1_verts");
	shadowParameter = cgGetNamedParameter(fragmentProgram, "passf_shadow");
	// frameParameter = cgGetNamedParameter(vertexProgram, "frame_id");

	glGenTextures(1, &texture_obj);
	glBindTexture(GL_TEXTURE_2D, texture_obj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, textures[0]->width_, textures[0]->height_, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)textures[0]->pixels_);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	cgGLSetTextureParameter(textureParameter, texture_obj);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &billboard_obj);
	glBindTexture(GL_TEXTURE_2D, billboard_obj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, textures[1]->width_, textures[1]->height_, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)textures[1]->pixels_);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	cgGLSetTextureParameter(billboardParameter, billboard_obj);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &depthBillboard_obj);
	glBindTexture(GL_TEXTURE_2D, depthBillboard_obj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, textures[1]->width_, textures[1]->height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)textures[1]->zbuffer);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	cgGLSetTextureParameter(depthBillboardParameter, depthBillboard_obj);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &cubemap_obj);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_obj);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_BGRA, faces_[0]->width_, faces_[0]->height_, 0, GL_BGRA, GL_UNSIGNED_BYTE, faces_[1]->pixels_);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_BGRA, faces_[1]->width_, faces_[1]->height_, 0, GL_BGRA, GL_UNSIGNED_BYTE, faces_[0]->pixels_);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_BGRA, faces_[2]->width_, faces_[2]->height_, 0, GL_BGRA, GL_UNSIGNED_BYTE, faces_[3]->pixels_);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_BGRA, faces_[3]->width_, faces_[3]->height_, 0, GL_BGRA, GL_UNSIGNED_BYTE, faces_[2]->pixels_);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_BGRA, faces_[5]->width_, faces_[5]->height_, 0, GL_BGRA, GL_UNSIGNED_BYTE, faces_[5]->pixels_);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_BGRA, faces_[4]->width_, faces_[4]->height_, 0, GL_BGRA, GL_UNSIGNED_BYTE, faces_[4]->pixels_);
	cgGLSetTextureParameter(cubeTextureParameter, cubemap_obj);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return true;

}

void ShaderOneInterface::PerFrameInit() {

	//set parameters
	cgGLSetStateMatrixParameter(
    vertexModelViewProj, 
		CG_GL_MODELVIEW_PROJECTION_MATRIX, 
    CG_GL_MATRIX_IDENTITY);
}

void ShaderOneInterface::EnableTexture() {
	cgGLEnableTextureParameter(textureParameter);
	cgGLEnableTextureParameter(billboardParameter);
	cgGLEnableTextureParameter(depthBillboardParameter);
	cgGLEnableTextureParameter(cubeTextureParameter);
}

void ShaderOneInterface::DisableTexture() {
	cgGLDisableTextureParameter(cubeTextureParameter);
	cgGLDisableTextureParameter(depthBillboardParameter);
	cgGLDisableTextureParameter(billboardParameter);
	cgGLDisableTextureParameter(textureParameter);
}

void ShaderOneInterface::SetPass(float id) {
	cgGLSetParameter1f(passParameter, id);
}

void ShaderOneInterface::SetBillboard(V3f* verts) {
	cgGLSetParameterArray3f(bbVertexParameter, 0, 0, (float*)verts);
}

void ShaderOneInterface::SetEye(V3f &eye) {
	cgGLSetParameter3f(eyeParameter, eye[0], eye[1], eye[2]);
}

void ShaderOneInterface::SetSoftshadow(V3f *verts, int id) {	
	if (id == 0)
		cgGLSetParameterArray3f(box0vertsParameter, 0, 0, (float*)verts);
	else if (id == 1)
		cgGLSetParameterArray3f(box1vertsParameter, 0, 0, (float*)verts);
}

void ShaderOneInterface::SetFrameTime(int frame) {
	cgGLSetParameter1f(frameParameter, frame);
}

void ShaderOneInterface::SetShadowPass(float pass) {
	cgGLSetParameter1f(shadowParameter, pass);
}

void ShaderOneInterface::PerFrameDisable() {
}


void ShaderOneInterface::BindPrograms() {

#ifdef GEOM_SHADER
  cgGLBindProgram(geometryProgram);
#endif
  cgGLBindProgram(vertexProgram);
  cgGLBindProgram(fragmentProgram);

}

void CGInterface::DisableProfiles() {

  cgGLDisableProfile(vertexCGprofile);
#ifdef GEOM_SHADER
  cgGLDisableProfile(geometryCGprofile);
#endif
  cgGLDisableProfile(pixelCGprofile);

}

void CGInterface::EnableProfiles() {

  cgGLEnableProfile(vertexCGprofile);
#ifdef GEOM_SHADER
  cgGLEnableProfile(geometryCGprofile);
#endif
  cgGLEnableProfile(pixelCGprofile);

}

