/***********************************************************

	Starter code for Assignment 3

	This file contains the interface and datastructures of the raytracer.
	Simple traversal and addition code to the datastructures are given to you.

***********************************************************/
#pragma once

#include "util.h"
#include "scene_object.h"
#include "light_source.h"


const double GLOBAL_DEDUCTION = 0.3;

class Raytracer {



public:
	// Renders 3D scene to an image given camera and lights setup.
	void render(Camera& camera, Scene& scene, LightList& light_list, Image& image);

	// ======================== PARAMS ==============================
	bool ANTI_ALIASING;
	bool SOFT_SHADOWS;

private:

	// Return the color of the ray after intersection and shading, call
	// this function recursively for reflection and refraction.
	Color shadeRay(Ray3D& ray, Scene& scene, LightList& light_list,int depth);

	// Traversal code for the scene, the ray is transformed into
	// the object space of each node where intersection is performed.
	void traverseScene(Scene& scene, Ray3D& ray);

	// After intersection, calculate the color of the ray by shading it
	// with all light sources in the scene.
	void computeShading(Ray3D& ray, LightList& light_list, Scene& scene);

	// Precompute the modelToWorld and worldToModel transformations for each
    // object in the scene.
	void computeTransforms(Scene& scene);


	// ================= Helper function and Params =======================
	Color Jitter_Anti_Aliasing(Point3D& imagePlane,double factor ,Scene& scene, LightList& light_list,const Matrix4x4& viewToWorld,int depth=3, int partition=2);


};



// ===================== Helper Functions =======================
void reflected(Ray3D& ray, Ray3D& reflectRay,double roughness);
Vector3D soft_shadow(double radius = 2.0);
bool reach_light_before_intersection(Ray3D& shadowRay,Point3D& light);
