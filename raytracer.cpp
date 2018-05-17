/***********************************************************

	Starter code for Assignment 3

	Implementations of functions in raytracer.h,
	and the main function which specifies the scene to be rendered.

***********************************************************/


#include "raytracer.h"
#include <cmath>
#include <iostream>
#include <cstdlib>


void Raytracer::traverseScene(Scene& scene, Ray3D& ray)  {
	for (size_t i = 0; i < scene.size(); ++i) {
		SceneNode* node = scene[i];

		if (node->obj->intersect(ray, node->worldToModel, node->modelToWorld)) {
			ray.intersection.mat = node->mat;
		}
	}
}

void Raytracer::computeTransforms(Scene& scene) {
	// right now this method might seem redundant. But if you decide to implement
	// scene graph this is where you would propagate transformations to child nodes

	for (size_t i = 0; i < scene.size(); ++i) {
		SceneNode* node = scene[i];

		node->modelToWorld = node->trans;
		node->worldToModel = node->invtrans;
	}
}



void Raytracer::computeShading(Ray3D& ray, LightList& light_list,Scene& scene) {
	for (size_t  i = 0; i < light_list.size(); ++i) {
		LightSource* light = light_list[i];

		// Each lightSource provides its own shading function.
		// Implement shadows here if needed.


		// create ray from intersection to lightSource
		Ray3D shadowRay;
		Point3D light_pos = light->get_position();
		shadowRay.dir    = light_pos - ray.intersection.point;

		if (SOFT_SHADOWS){
			Vector3D randomed = soft_shadow(0.5);

			// make sure that the sampled light pos remained same in relative position
			Ray3D checker;
			checker.dir = 1.0 * randomed;
			checker.dir.normalize();
			checker.origin = light_pos + 0.00000001 * checker.dir;
			traverseScene(scene,checker);
			Point3D sampled = checker.origin + randomed;
			if (checker.intersection.none || reach_light_before_intersection(checker,sampled)){
				shadowRay.dir = shadowRay.dir + randomed;
				light_pos = checker.origin + randomed;
			}

		}

		shadowRay.dir.normalize();
		shadowRay.origin = ray.intersection.point + 0.000001 * shadowRay.dir;
		traverseScene(scene,shadowRay);

		// check the relative position of the light and intersection point
		if (shadowRay.intersection.none || reach_light_before_intersection(shadowRay,light_pos) ){
			light->shade(ray);
		}else if ((!shadowRay.intersection.none) && (shadowRay.intersection.mat->refraction_index > 0.1)){
			light->shade(ray);
			ray.col = (0.85) * ray.col;
		}
		else{
			light->shade(ray);
			ray.col = (0.6) * ray.col;
		}
	}
	ray.col = (1.0/light_list.size()) * ray.col;

}



Color Raytracer::shadeRay(Ray3D& ray, Scene& scene, LightList& light_list,int depth) {
	Color col(0.0, 0.0, 0.0);
	traverseScene(scene, ray); // find intersection

	// Don't bother shading if the ray didn't hit
	// anything.
	if (!ray.intersection.none) {
		computeShading(ray, light_list, scene);
		// ray.col = (1.0/light_list.size()) * ray.col;
		double reflection = ray.intersection.mat->reflection;
		double refraction_index = ray.intersection.mat->refraction_index;

		// You'll want to call shadeRay recursively (with a different ray,
		// of course) here to implement reflection/refraction effects.
		if (depth > 0){
			// refraction

			if(refraction_index > 0){

				Vector3D n = ray.intersection.normal;
				Vector3D I = ray.dir;

				double eta = 1.0/refraction_index;
				double cosi = -n.dot(ray.dir);
        Vector3D dir = eta * ray.dir + (eta * cosi - cosi) * n;

        //create new ray
        Ray3D new_ray;
        new_ray.dir = dir;
        new_ray.dir.normalize();
        new_ray.origin = ray.intersection.point + 0.01 * new_ray.dir;

        //shade ray
        Color refrCol = shadeRay(new_ray, scene, light_list, depth-1);
        col = col + refrCol;
			}


			// reflection
			Ray3D reflectRay;
			reflected(ray,reflectRay,reflection);
			Color reflectedColor = shadeRay(reflectRay,scene,light_list,depth-1);
			col = col + 0.8 * ((1.0 - reflection) * ray.col + reflection * reflectedColor);
		}
	}
	col.clamp();
	return col;
}



void Raytracer::render(Camera& camera, Scene& scene, LightList& light_list, Image& image) {
	computeTransforms(scene);

	Matrix4x4 viewToWorld;
	double factor = (double(image.height)/2)/tan(camera.fov*M_PI/360.0);

	viewToWorld = camera.initInvViewMatrix();

	// Construct a ray for each pixel.
	for (int i = 0; i < image.height; i++) {
		for (int j = 0; j < image.width; j++) {
			// Sets up ray origin and direction in view space,
			// image plane is at z = -1.
			Point3D origin(0, 0, 0);
			Point3D imagePlane;
			imagePlane[0] = (-double(image.width)/2 + 0.5 + j)/factor;
			imagePlane[1] = (-double(image.height)/2 + 0.5 + i)/factor;
			imagePlane[2] = -1;

			Color col;
			if (ANTI_ALIASING){
				// partition > 1,  do anti-aliasing
				col = Jitter_Anti_Aliasing(imagePlane,factor,scene,light_list,viewToWorld,3,2);
			}
			else{
				// partition == 1, shade without anti-aliasing
				col = Jitter_Anti_Aliasing(imagePlane,factor,scene,light_list,viewToWorld,3,1);
			}

			image.setColorAtPixel(i, j, col);
		}
	}
}


// =============================== Helper Functions ============================

// make sure to use this after traverseScene
bool reach_light_before_intersection(Ray3D& shadowRay,Point3D& light){
	double check_t_val = 0;
	bool yes = false;

	double dx = shadowRay.dir[0];
	double dy = shadowRay.dir[1];
	double dz = shadowRay.dir[2];

	double ox = shadowRay.origin[0];
	double oy = shadowRay.origin[1];
	double oz = shadowRay.origin[2];

	double px = light[0];
	double py = light[1];
	double pz = light[2];

	if (dx != 0) check_t_val = (px-ox)/dx;
	else if(dy != 0) check_t_val = (py-oy)/dy;
	else if(dz != 0) check_t_val = (pz-oz)/dz;
	else yes = true; // intersect at source point

	yes = yes || check_t_val < shadowRay.intersection.t_value;
  // if (check_t_val < 0){
	// 	printf("Origin : [%f,%f,%f]\n", ox,oy,oz);
	// 	printf("direct : [%f,%f,%f]\n", dx,dy,dz);
	// 	printf("inters : [%f,%f,%f]\n", px,py,pz);
	// 	exit(0);
	// }
	return yes && check_t_val >= 0;
}


// reflection:if roughness!=0 then do glossy reflection
void reflected(Ray3D& ray, Ray3D& reflectRay,double roughness){
	Vector3D incident = ray.dir;
	Vector3D normal = ray.intersection.normal;
	normal.normalize();
	Vector3D R = 2 * normal.dot(-incident) * normal + incident;
	R.normalize();

	if (roughness > 0.1)
	{
		Vector3D u = R.cross(normal);
		if (u.length() == 0){
			normal = normal + Vector3D(0,0,0.01);
			normal.normalize();
			u = R.cross(normal);
		}
		u.normalize();
		Vector3D v = R.cross(u);
		v.normalize();

		double theta = acos(pow( (((float) rand()) / ((float)RAND_MAX)), 1.0 / (ray.intersection.mat->specular_exp + 1)));
		double phi = 2 * M_PI *  (((float) rand()) / ((float)RAND_MAX));
		double x = sin(theta) * cos(phi);
		double y = sin(theta) * sin(phi);
		double z = cos(theta);

		R = x * u + y * v + z * R;
		R.normalize();

	}

	reflectRay.dir = R;
	reflectRay.origin = ray.intersection.point + 0.0001 * reflectRay.dir;
}

// soft shadows
Vector3D soft_shadow(double radius){
	double rand2 = ((float) rand()) / ((float)RAND_MAX);
	double rand3 = ((float) rand()) / ((float)RAND_MAX);

	double r = radius * (((float) rand()) / ((float)RAND_MAX) - 0.5);
	double theta = 2 * M_PI * pow(rand2,3);
	double phi = 2 * M_PI * pow(rand3,3);

	Vector3D rand_sampled(r*cos(theta)*sin(phi),r*sin(theta)*sin(phi),r*cos(phi));
	return rand_sampled;
}



// anti-aliasing
Color Raytracer::Jitter_Anti_Aliasing(Point3D& imagePlane,double factor ,Scene& scene, LightList& light_list,const Matrix4x4& viewToWorld,int depth, int partition){

	Color col(0.0,0.0,0.0);

	// if partition == 1, perform shading without anti-aliasing
	if(partition <= 1)
	{
		// TODO: Convert ray to world space
		Ray3D ray;
		Point3D origin(0, 0, 0);
		ray.origin = viewToWorld * origin;
		ray.dir = viewToWorld * (imagePlane - origin);

		ray.dir.normalize();
		int depth = 3;
		col = shadeRay(ray, scene, light_list,depth);
	}
	// else do Jitter_Anti_Aliasing
	else
	{
		// de-factor : get upper-left, i.e, start point of the current pixel
		Point3D start;
		start[0] = imagePlane[0] * factor - 0.5;
		start[1] = imagePlane[1] * factor - 0.5;
		start[2] = -1;

		// partition
		Point3D current;
		double step = 1.0/partition;
		for (int i = 0; i < partition; i++){
			for(int j = 0; j < partition; j++){
				double rand1 = ((float) rand()) * step / ((float)RAND_MAX);
				double rand2 = ((float) rand()) * step / ((float)RAND_MAX);
				current[0] = (start[0] + i*step + rand1) / factor;
				current[1] = (start[1] + j*step + rand2) / factor;
				current[2] = -1;

				Point3D origin(0, 0, 0);
				// TODO: Convert ray to world space
				Ray3D ray;

				ray.origin = viewToWorld * origin;
				ray.dir = viewToWorld * (current - origin);
				ray.dir.normalize();
				col = col + shadeRay(ray, scene, light_list,depth);
			}
		}
		col = ((1.0)/(partition*partition)) * col;
	}


	col.clamp();
	return col;
}
