/***********************************************************

	Starter code for Assignment 3

	Implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

void PointLight::shade(Ray3D& ray) {
	// TODO: implement this function to fill in values for ray.col
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray
	// is available.  So be sure that traverseScene() is called on the ray
	// before this function.

  //  --------- Potential risk : traverseScene()???

  // return if no intersection
  if (ray.intersection.none) return;

  // get the values at the intersection
  Material* mat = ray.intersection.mat;

  Vector3D n = ray.intersection.normal; // out point normal
  n.normalize();

  Vector3D s = pos - ray.intersection.point; // ray from intersection to the light
  s.normalize();

  Vector3D r = -s + 2 * n.dot(s) * n; // reflection ray
  r.normalize();

  Vector3D b = -ray.dir;
  b.normalize();

  // future work : need to check the shape of the object
  if (mat->texture != NULL){

    Point3D p = ray.intersection.model_point;
    int w = mat->texture->width;
    int h = mat->texture->height;
    int offset = 0;


    // texture on plane
    if (ray.intersection.shape == 0){
      int x = w * (p[0] + 0.5);
      int y = h * (p[1] + 0.5);
      offset = y * w + x;
    }

    // texture on Sphere
    else if(ray.intersection.shape == 1){
      int x = w * (atan2(p[0],p[2])/(2*M_PI) + 0.5);
      int y = h * (acos(-p[1])/M_PI);
      offset = y * w + x;
    }


    Color col(mat->texture->rarray[offset],mat->texture->garray[offset],mat->texture->barray[offset]);
    col = (1.0/255.0)*1.5 * col;
    // assign color based on the texture and light source, similar to diffuse
    // ray.col = ray.col + fmax(0,s.dot(n)) * col;
    ray.col = ray.col + col;
  }

  else{
    // compute for diffusion
    Color ambient = mat->ambient * col_ambient;
    Color diffusion = fmax(0,s.dot(n)) * mat->diffuse * col_diffuse;
    Color specular =  pow(fmax(0,r.dot(b)),mat->specular_exp) * mat->specular *  col_specular;
    ray.col = ray.col + ambient + diffusion + specular;
  }


  // ray.col.clamp();
}
