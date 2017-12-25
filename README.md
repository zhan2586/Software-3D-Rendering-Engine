# Software 3D Rendering Engine 

This project is to demonstrate rasteraization 3D rendering pipeline in software. Every rendering stage of the project was implemented from scratch using conventional algorithms, for example, camera projection, texture lookup, framebuffer, lighting, shadow, 3D mathematics, .etc. Moreover, this project also has functions to put the software rendering pipeline to GPU using CG shader.

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
   * Additonal support for GPU view and projection matrix
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
   * Render mesh with / without lighting / textures (Model space / Screen space)
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
    <img src="https://drive.google.com/uc?id=1Jm3En5nuUAtB6TJhgittaeC2hzSiaIPo" width="400">    
  * Transparent texture projector  
    <img src="https://drive.google.com/uc?id=1aSjPMBsNdPHnufrCuzSfMtvXd7C0bO8d" width="400"> 
  
* https://youtu.be/sYHK0IVA1ag
  * Invisibility with projector   
    <img src="https://drive.google.com/uc?id=130u_4HKSQ-63vcI6aQ3g4NnoBwhIRf0U" width="400"> 

### Software Environment Mapping Demo
* https://youtu.be/Yn06Psa5j9w  
  * Per-pixel reflected ray look up for reflector
  * Eye-rays look up for environment  
    <img src="https://drive.google.com/uc?id=1TGB95UYqoKBY8_IYycixG74uU6gM-_Wu" width="400"> 
  * Refraction and fake Fresnel  
    <img src="https://drive.google.com/uc?id=1LfZt0hQusj9H3XF2MWgMte4zdiOFY4MJ" width="400"> 

### Hardware Extension using CG shader Demo  
* https://youtu.be/Ucnvez-XPNo  
  * A demonstration for hardware rendering support using CG shader.  
  * Reflection Approximation by Imposter. The reflection of the back teapot is rendered by approximating the front teapot with billboard. The texture of billboard is precomputed in main.cpp and looked up in GPU shader, besides I created a new window to display the texture of billboard.  
    <img src="https://drive.google.com/uc?id=1tFhve65x3A9-LhrNMTSvAM_K7mpVY9L1" width="400">
