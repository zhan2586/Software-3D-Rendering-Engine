#pragma once

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <Cg/cgGL.h>
#include <Cg/cg.h>
#include <vector>
using namespace std;
#include "framebuffer.h"
#include "mesh.h"


// two classes defining the interface between the CPU and GPU

// models part of the CPU-GPU interface that is independent of specific shaders
class CGInterface {

public:
  CGprofile   vertexCGprofile; // vertex shader profile of GPU
  CGprofile   pixelCGprofile; // pixel shader profile of GPU
  CGcontext  cgContext;
  void PerSessionInit(); // per session initialization
  CGInterface(); // constructor
  void EnableProfiles(); // enable GPU rendering
  void DisableProfiles(); // disable GPU rendering
};


// models the part of the CPU-GPU interface for a specific shader
//        here there is a single shader "ShaderOne"
//  a shader consists of a vertex, a geometry, and a pixel (fragment) shader
// fragment == pixel; shader == program; e.g. pixel shader, pixel program, fragment shader, fragment program, vertex shader, etc.
class ShaderOneInterface {
  CGprogram vertexProgram;
  CGprogram fragmentProgram;
  // uniform parameters, i.e parameters that have the same value for all geometry rendered
  CGparameter vertexModelViewProj; // a matrix combining projection and modelview matrices
  CGparameter textureParameter;
  CGparameter billboardParameter;
  CGparameter passParameter;
  CGparameter bbVertexParameter;
  CGparameter eyeParameter;
  CGparameter depthBillboardParameter;
  CGparameter cubeTextureParameter;
  CGparameter box0vertsParameter;
  CGparameter box0triParameter;
  CGparameter box1vertsParameter;
  CGparameter frameParameter;
  CGparameter shadowParameter;
  
public:
  ShaderOneInterface() : faces_(0) {};
  bool PerSessionInit(CGInterface *cgi, vector<Framebuffer*> &textures); // per session initialization
  void BindPrograms(); // enable geometryProgram, vertexProgram, fragmentProgram
  void PerFrameInit(); // set uniform parameter values, etc.
  void PerFrameDisable(); // disable programs
  void EnableTexture();
  void DisableTexture();
  void SetPass(float id);
  void SetBillboard(V3f* verts);
  void SetEye(V3f &eye);
  void SetSoftshadow(V3f *verts, int id);
  void SetFrameTime(int frame);
  void SetShadowPass(float pass);
  vector<Framebuffer *> faces_;
};
