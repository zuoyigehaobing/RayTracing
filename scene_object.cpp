/***********************************************************

	Starter code for Assignment 3

	Implements scene_object.h

***********************************************************/

#include <stdlib.h>
#include "scene_object.h"

bool UnitSquare::intersect(Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0),
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point,
	// intersection.normal, intersection.none, intersection.t_value.
	//
	// HINT: Remember to first transform the ray into object space
	// to simplify the intersection test.

	Point3D origin = worldToModel * ray.origin;
	Vector3D direction = worldToModel * ray.dir;
	double t = -origin[2]/direction[2];

	// invalid intersection
	if (t <= 0 || direction[2] == 0) return false;

	Point3D p = origin + t * direction;
	Vector3D normal = Vector3D(0.0,0.0,1.0);

	// change t to worldspace
	t = t_value_in_wordSpace(ray.origin,modelToWorld * p,ray.dir);
	bool update = ray.intersection.none || t < ray.intersection.t_value;

	if (abs(p[0]) <= 0.5 && abs(p[1]) <= 0.5 && update){
		ray.intersection.normal = transNorm(worldToModel,normal);
		ray.intersection.normal.normalize();
		ray.intersection.none = false;
		ray.intersection.t_value = t;
		ray.intersection.point = modelToWorld * p;
		ray.intersection.model_point = p;
		ray.intersection.shape = 0;
		return true;
	}

	return false;
}


bool UnitSphere::intersect(Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld) {
	// TODO: implement intersection code for UnitSphere, which is centred
	// on the origin.
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point,
	// intersection.normal, intersection.none, intersection.t_value.
	//
	// HINT: Remember to first transform the ray into object space
	// to simplify the intersection test.

	Point3D origin = worldToModel * ray.origin;
	Vector3D direction = worldToModel * ray.dir;
	Vector3D e_minus_c = Vector3D(origin[0],origin[1],origin[2]);

	double A = direction.dot(direction);
	double B = 2*direction.dot(e_minus_c);
	double C = e_minus_c.dot(e_minus_c) - 1.0;
	double delta = B*B - 4*A*C;

	if(delta >= 0){
		double t1 = (-B + sqrt(delta))/(2*A); // larger root
		double t2 = (-B - sqrt(delta))/(2*A); // smaller root

		double t;
		if (t1 <= 0) return false; // both negative roots
		else if (t2 <= 0) t = t1;
		else t = t2;

		// intersection
		Point3D intersection = origin + t * direction;
		Vector3D normal = Vector3D(intersection[0],intersection[1],intersection[2]);

		// update for small t_value or DNE
		// change t to worldspace
		t = t_value_in_wordSpace(ray.origin,modelToWorld * intersection,ray.dir);
		if (ray.intersection.none || t < ray.intersection.t_value){
			ray.intersection.none = false;
			ray.intersection.t_value = t;
			ray.intersection.point = modelToWorld * intersection;
			ray.intersection.normal = transNorm(worldToModel,normal);
			ray.intersection.normal.normalize();
			ray.intersection.model_point = intersection;
			ray.intersection.shape = 1;
			return true;
		}
	}
	return false;
}


bool UnitCylinder::intersect(Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld) {
	// TODO: implement intersection code for UnitCylinder, which is centred
	// on the origin.
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point,
	// intersection.normal, intersection.none, intersection.t_value.
	//
	// HINT: Remember to first transform the ray into object space
	// to simplify the intersection test.

	Point3D origin = worldToModel * ray.origin;
	Vector3D direction = worldToModel * ray.dir;
	bool updated = false;
	// check top-surface intersection t values
	double t_top = (0.5 - origin[2])/direction[2];
	Point3D intersect1 = origin + t_top * direction;
	bool inCircle = pow(intersect1[0],2) + pow(intersect1[1],2) <= 1;


	t_top = t_value_in_wordSpace(ray.origin,modelToWorld * intersect1,ray.dir);
	bool update = ray.intersection.none || t_top < ray.intersection.t_value;
	Vector3D normal = Vector3D(0.0,0.0,1.0);

	if (inCircle && update && t_top>0){
		ray.intersection.none = false;
		ray.intersection.t_value = t_top;
		ray.intersection.point = modelToWorld * intersect1;
		ray.intersection.normal = transNorm(worldToModel,normal);
		ray.intersection.normal.normalize();
		ray.intersection.model_point = intersect1;
		updated = true;
	}

	// check bot-surface intersection t values
	double t_bot = (-0.5 - origin[2])/direction[2];
	Point3D intersect2 = origin + t_bot * direction;
	inCircle = pow(intersect2[0],2) + pow(intersect2[1],2) <= 1;

  t_bot = t_value_in_wordSpace(ray.origin,modelToWorld * intersect2,ray.dir);

	update = ray.intersection.none || t_bot < ray.intersection.t_value;
	normal = Vector3D(0.0,0.0,-1.0);

	if (inCircle && update && t_bot>0){
		ray.intersection.none = false;
		ray.intersection.t_value = t_bot;
		ray.intersection.point = modelToWorld * intersect2;
		ray.intersection.normal = transNorm(worldToModel,normal);
		ray.intersection.normal.normalize();
		ray.intersection.model_point = intersect2;
		updated = true;
	}

	// check side-surface intersections
	double A = pow(direction[0],2) + pow(direction[1],2);
	double B = 2 * (origin[0]*direction[0]+origin[1]*direction[1]);
	double C = pow(origin[0],2) + pow(origin[1],2) - 1.0;
	double delta = B*B - 4*A*C;

	if(delta >= 0){
		double t1 = (-B + sqrt(delta))/(2*A); // larger root
		double t2 = (-B - sqrt(delta))/(2*A); // smaller root

		double t;
		if (t1 < 0) return updated; // both negative roots
		else if (t2 < 0) t = t1;
		else t = t2;

		// intersection
		Point3D intersection = origin + t * direction;
		normal = Vector3D(intersection[0],intersection[1],0);

		// make sure that the intersection happens in the Cylinder
		if (abs(intersection[2]) >= 0.5) return updated;

		// update for small t_value or DNE
		t = t_value_in_wordSpace(ray.origin,modelToWorld * intersection,ray.dir);

		if (ray.intersection.none || t < ray.intersection.t_value){
			ray.intersection.none = false;
			ray.intersection.t_value = t;
			ray.intersection.point = modelToWorld * intersection;
			ray.intersection.normal = transNorm(worldToModel,normal);
			ray.intersection.normal.normalize();
			ray.intersection.model_point = intersection;
			updated = true;
		}
	}

	return updated;
}





bool UnitCone::intersect(Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld) {
	// TODO: implement intersection code for UnitCylinder, which is centred
	// on the origin.
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point,
	// intersection.normal, intersection.none, intersection.t_value.
	//
	// HINT: Remember to first transform the ray into object space
	// to simplify the intersection test.

	Point3D origin = worldToModel * ray.origin;
	Vector3D direction = worldToModel * ray.dir;

	bool updated = false;


	// check top-surface intersection t values
	double t = (1.0 - origin[2])/direction[2];
	Point3D intersect1 = origin + t * direction;
	bool inCircle = pow(intersect1[0],2) + pow(intersect1[1],2) <= 1;

	t = t_value_in_wordSpace(ray.origin,modelToWorld * intersect1,ray.dir);

	bool update = ray.intersection.none || t < ray.intersection.t_value;
	Vector3D normal = Vector3D(0.0,0.0,1.0);

	if (inCircle && update && t>0){
		ray.intersection.none = false;
		ray.intersection.t_value = t;
		ray.intersection.point = modelToWorld * intersect1;
		ray.intersection.normal = transNorm(worldToModel,normal);
		ray.intersection.normal.normalize();
		ray.intersection.model_point = intersect1;
		updated = true;
	}

	// check for side-surface:
	double A = pow(direction[0],2) + pow(direction[1],2) - pow(direction[2],2);
	double B = 2 * (direction.dot(origin - Point3D(0,0,0)) - 2 * direction[2]*origin[2]);
	double C = pow(origin[0],2) + pow(origin[1],2) - pow(origin[2],2);
	double delta = B*B - 4*A*C;

	if(delta >= 0){
		double t1 = (-B + sqrt(delta))/(2*A); // larger root
		double t2 = (-B - sqrt(delta))/(2*A); // smaller root

		if (t1 < 0) return updated; // both negative roots
		else if (t2 < 0) t = t1;
		else t = t2;

		// intersection
		Point3D intersection = origin + t * direction;
		normal = Vector3D(intersection[0],intersection[1],-intersection[2]);

		// check the range of z:
		if (intersection[2] < 0 || intersection[2] >= 1) return updated;

		// update for small t_value or DNE
		t = t_value_in_wordSpace(ray.origin,modelToWorld * intersection,ray.dir);

		if (ray.intersection.none || t < ray.intersection.t_value){
			ray.intersection.none = false;
			ray.intersection.t_value = t;
			ray.intersection.point = modelToWorld * intersection;
			ray.intersection.normal = transNorm(worldToModel,normal);
			ray.intersection.normal.normalize();
			ray.intersection.model_point = intersection;
			updated = true;
		}

	}

	return updated;
}



void SceneNode::rotate(char axis, double angle) {
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;

	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		if (i == 0) {
			this->trans = this->trans*rotation;
			angle = -angle;
		}
		else {
			this->invtrans = rotation*this->invtrans;
		}
	}
}

void SceneNode::translate(Vector3D trans) {
	Matrix4x4 translation;

	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	this->trans = this->trans*translation;
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	this->invtrans = translation*this->invtrans;
}

void SceneNode::scale(Point3D origin, double factor[3] ) {
	Matrix4x4 scale;

	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	this->trans = this->trans*scale;
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	this->invtrans = scale*this->invtrans;
}



double t_value_in_wordSpace(Point3D origin, Point3D intersection, Vector3D dir){
	// direction
	double dx = dir[0];
	double dy = dir[1];
	double dz = dir[2];
	// origin
	double ox = origin[0];
	double oy = origin[1];
	double oz = origin[2];
	// intersection
	double px = intersection[0];
	double py = intersection[1];
	double pz = intersection[2];

	double check_t_val = 0;

	if (dx != 0) check_t_val = (px-ox)/dx;
	else if(dy != 0) check_t_val = (py-oy)/dy;
	else check_t_val = (pz-oz)/dz;

	return check_t_val;
}
