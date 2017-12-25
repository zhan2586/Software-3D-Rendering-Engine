# Software 3D Rendering Engine 

A software implementation of 3D rasterization rendering pipeline, for example, pinhole camera, frame buffer, texture lookup, lighting, 3D mathematics, .etc.

## Environment
* Windows 10 x64  
* Visual Studio 2017 Release x64  

## Relevant Headers
~~~
  AABB.h // Axis aligned bounding box  
  Camera.h // Software implementation of planar pinhole camera  
  cubemap.h // Software implementation of cubemap  
  framebuffer.h // Software implementation of framebuffer  
  light.h // lighting model  
  m33.h // matrix and relevant operation implementation  
  mesh.h // binary triangle mesh support  
  scene.h // scene manager  
  v3f.h // float3 vector and relevant operation implementation  

  // additional hardware implementation  
  CGInterface.h // GPU support using CG shader  
~~~
## Demos

### Guide  
* In main.cpp, change current demo by changing the following line:  
~~~
#define USING_DEMO SOFTWARE_SM_PTM_DEMO  
~~~
* Then Build and run under specified environment  

### SOFTWARE SHADOW MAPPING AND PROJECTIVE TEXTURE MAPPING DEMO  

* Shadow mapping  
https://youtu.be/CttNZ9FeePo   
* Porjective texture mapping (Transparent texture projector and Invisibility with projector)  
https://youtu.be/sYHK0IVA1ag  

### SOFTWARE ENVIRONMENT MAPPING DEMO  
* https://youtu.be/Yn06Psa5j9w  
* Per-pixel reflected ray look up for reflector
* Eye-rays look up for environment
* Refraction and fake Fresnel

### HARDWARE RENDERING DEMO  
* https://youtu.be/Ucnvez-XPNo  
* A demonstration for hardware rendering support using CG shader.  
* Reflection Approximation by Imposter. The reflection of the back teapot is rendered by approximating the front teapot with billboard. The texture of billboard is precomputed in main.cpp and looked up in GPU shader, besides I created a new window to display the texture of billboard.  
