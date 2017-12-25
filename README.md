# Software 3D Rendering Engine 

A software implementation of 3D rasterization rendering pipeline, for example, pinhole camera, frame buffer, texture lookup, lighting, 3D mathematics, .etc.

## Environment
* Windows 10 x64  
* Visual Studio 2017 Release x64  

## Relevant Headers
~~~
  /* Axis Aligned Bounding Box */
  AABB.h 
  /* 
   * Constructor for software pinhole camera  
   * Project and unproject function
   * Translation, pan, tilt, roll function
   * Camera Interposlation
   * Support for GPU view and projection matrix
  */
  Camera.h 
  /* 
   * Constructor for cube map  
   * Bilinear look up for cube map
  */
  cubemap.h  
  /* 
   * Constructor for framebuffer 
   * Zbuffer, Color buffer, Alpha buffer set and clear
   * Load and save frambuffer
   * Nearest lookup and Bilinear lookup
   * Use as depth buffer, texture buffer or display image
  */
  framebuffer.h  
  /* 
   * Constructor for point light
   * Light camera
  */
  light.h
  /* 3D mathematic funcitons */
  m33.h
  v3f.h
  /* 
   * Constructor for triangle mesh
   * Load triangle mesh from binary model
   * Scale, rotate, and translate mesh
   * Render mesh in fill mode / wireframe
   * Render mesh with / without lighting / textures
  */
  mesh.h
  /* scene manager */
  scene.h
  /* GPU interface */  
  CGInterface.h
~~~
## Demos

### Guide  
* In main.cpp, change current demo by changing the following line:  
~~~
#define USING_DEMO SOFTWARE_SM_PTM_DEMO  
~~~
* Then Build and run under specified environment  

### Software Shadow Mapping and Projective Texture Mapping Demo  

* https://youtu.be/CttNZ9FeePo 
  * Shadow mapping  
* https://youtu.be/sYHK0IVA1ag
  * Porjective texture mapping (Transparent texture projector and Invisibility with projector)  
  

### Software Environment Mapping Demo
* https://youtu.be/Yn06Psa5j9w  
  * Per-pixel reflected ray look up for reflector
  * Eye-rays look up for environment
  * Refraction and fake Fresnel

### Hardware Extension using CG shader  
* https://youtu.be/Ucnvez-XPNo  
  * A demonstration for hardware rendering support using CG shader.  
  * Reflection Approximation by Imposter. The reflection of the back teapot is rendered by approximating the front teapot with billboard. The texture of billboard is precomputed in main.cpp and looked up in GPU shader, besides I created a new window to display the texture of billboard.  
