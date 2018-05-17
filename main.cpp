/***********************************************************

	Starter code for Assignment 3

***********************************************************/

#include <cstdlib>
#include "raytracer.h"
#include <stdlib.h>


	/*
		Remain to be done:
		1. Implementation for cube  -> today
		2. Plane TEXTURE  				  -> today
		3. Cube TEXTURE							-> today
		4. refraction								-> 4.6
		5. glossy reflection				-> 4.6

		*. Design the light.				-> 4.6
		*. Design glass cup and water, pencial for refraction demo ->4,6
		*. Design table in the room
		*. Design more complex objects
		6. Motion blur
		7. BSP algorithm
		8. Depth of field
		9. Mesh geometry
		10. Import Dragon and Teapot, etc.
		11. Find pictures for environmental mapping.

	*/



int main(int argc, char* argv[])
{
	// Build your scene and setup your camera here, by calling
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the
	// assignment.
	Raytracer raytracer;

	raytracer.ANTI_ALIASING = true;
	raytracer.SOFT_SHADOWS = true;

	LightList light_list;
	Scene scene;

	// int width = 320;
	// int height = 240;
	int resolution_factor = 2;
	int width = 320 * resolution_factor;
	int height = 240 * resolution_factor;

	if (argc == 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}


	// =======================================================================
	// ||                 Basic Settings				 														||
	// =======================================================================

	double box_length,box_width,box_height;
	box_length = 4.8;
	box_width = 3;
	box_height = 3.5;

	Vector3D viewdir(0 ,0,-1);
	viewdir.normalize();
	Point3D eye(0.3,0,0.49 * box_width );
	double fov = 110;

	Vector3D viewdir2 = viewdir;
	Point3D eye2 = Point3D(0,-0.49*box_height,0);
	double fov2 = 100;

	// =======================================================================
	// ||                				 Make a light																||
	// =======================================================================
	Point3D light_pos = Point3D(0.5,box_height*0.4,0.1 * box_width);
	PointLight* pLight = new PointLight(light_pos, Color(1.2,1.2,1.2));
	light_list.push_back(pLight);


	Material bulb_mat(Color(0.9,0.9,0.7), Color(0.9,0.9,0.7),
                Color(0,0,0), 10);

	SceneNode* bulb = new SceneNode(new UnitSquare(), &bulb_mat);
	scene.push_back(bulb);
	double bulb_factor[3] = {1.5, 0.7 , 1};
	bulb->translate(Vector3D(0,0.4999 * box_height,-0.25 * box_width));
	bulb ->rotate('x',90);
	bulb->scale(Point3D(0, 0, 0), bulb_factor);



	// =======================================================================
	// ||                 ENVIRONMENTAL MAPPING															||
	// =======================================================================

	Material grey(Color(0, 0, 0), Color(0.4, 0.4, 0.4),
	Color(0.3, 0.3, 0.3), 5);
	Material ceilling(Color(0.1, 0.1, 0.1), Color(0.2, 0.2, 0.2),
	Color(0.3, 0.3, 0.3), 5);
	Material ground(Color(0.1, 0.1, 0.1), Color(0.2, 0.2, 0.2),
	Color(0.3, 0.3, 0.3), 50000,0.8);
	Material red(Color(0, 0, 0), Color(0.7, 0.3, 0.1),
	Color(0.3, 0.3, 0.3), 50);
	Material blue(Color(0, 0, 0), Color(0.1, 0.7, 0.4),
	Color(0.3, 0.3, 0.3), 50);








	Material evn_back(Color(0, 0, 0), Color(0.4, 0.4, 0.4),
	Color(0.3, 0.3, 0.3), 5);
	// paint the TEXTURE on materials
	unsigned long int evn_back_texture_width;
	long int evn_back_texture_height;
	unsigned char*  evn_back_rarray;
	unsigned char*  evn_back_garray;
	unsigned char*  evn_back_barray;

	bmp_read("./environment/back.bmp", &evn_back_texture_width, &evn_back_texture_height, &evn_back_rarray, &evn_back_garray, &evn_back_barray);
	Texture texture_env_back(true,evn_back_texture_width,evn_back_texture_height,evn_back_rarray,evn_back_garray,evn_back_barray);
	evn_back.texture = &texture_env_back;







	Material evn_front(Color(0, 0, 0), Color(0.4, 0.4, 0.4),
	Color(0.3, 0.3, 0.3), 5);
	// paint the TEXTURE on materials
	unsigned long int evn_front_texture_width;
	long int evn_front_texture_height;
	unsigned char*  evn_front_rarray;
	unsigned char*  evn_front_garray;
	unsigned char*  evn_front_barray;

	bmp_read("./environment/front.bmp", &evn_front_texture_width, &evn_front_texture_height, &evn_front_rarray, &evn_front_garray, &evn_front_barray);
	Texture texture_env_front(true,evn_front_texture_width,evn_front_texture_height,evn_front_rarray,evn_front_garray,evn_front_barray);
	evn_front.texture = &texture_env_front;









	Material evn_left(Color(0, 0, 0), Color(0.4, 0.4, 0.4),
	Color(0.3, 0.3, 0.3), 5);
	// paint the TEXTURE on materials
	unsigned long int evn_left_texture_width;
	long int evn_left_texture_height;
	unsigned char*  evn_left_rarray;
	unsigned char*  evn_left_garray;
	unsigned char*  evn_left_barray;

	bmp_read("./environment/left.bmp", &evn_left_texture_width, &evn_left_texture_height, &evn_left_rarray, &evn_left_garray, &evn_left_barray);
	Texture texture_env_left(true,evn_left_texture_width,evn_left_texture_height,evn_left_rarray,evn_left_garray,evn_left_barray);
	evn_left.texture = &texture_env_left;










	Material evn_right(Color(0, 0, 0), Color(0.4, 0.4, 0.4),
	Color(0.3, 0.3, 0.3), 5);
	// paint the TEXTURE on materials
	unsigned long int evn_right_texture_width;
	long int evn_right_texture_height;
	unsigned char*  evn_right_rarray;
	unsigned char*  evn_right_garray;
	unsigned char*  evn_right_barray;

	bmp_read("./environment/right.bmp", &evn_right_texture_width, &evn_right_texture_height, &evn_right_rarray, &evn_right_garray, &evn_right_barray);
	Texture texture_env_right(true,evn_right_texture_width,evn_right_texture_height,evn_right_rarray,evn_right_garray,evn_right_barray);
	evn_right.texture = &texture_env_right;










	Material evn_bot(Color(0, 0, 0), Color(0.4, 0.4, 0.4),
	Color(0.3, 0.3, 0.3), 5);
	// paint the TEXTURE on materials
	unsigned long int evn_bot_texture_width;
	long int evn_bot_texture_height;
	unsigned char*  evn_bot_rarray;
	unsigned char*  evn_bot_garray;
	unsigned char*  evn_bot_barray;

	bmp_read("./environment/bot.bmp", &evn_bot_texture_width, &evn_bot_texture_height, &evn_bot_rarray, &evn_bot_garray, &evn_bot_barray);
	Texture texture_env_bot(true,evn_bot_texture_width,evn_bot_texture_height,evn_bot_rarray,evn_bot_garray,evn_bot_barray);
	evn_bot.texture = &texture_env_bot;











	Material evn_top(Color(0, 0, 0), Color(0.4, 0.4, 0.4),
	Color(0.3, 0.3, 0.3), 5);
	// paint the TEXTURE on materials
	unsigned long int evn_top_texture_width;
	long int evn_top_texture_height;
	unsigned char*  evn_top_rarray;
	unsigned char*  evn_top_garray;
	unsigned char*  evn_top_barray;

	bmp_read("./environment/top.bmp", &evn_top_texture_width, &evn_top_texture_height, &evn_top_rarray, &evn_top_garray, &evn_top_barray);
	Texture texture_env_top(true,evn_top_texture_width,evn_top_texture_height,evn_top_rarray,evn_top_garray,evn_top_barray);
	evn_top.texture = &texture_env_top;







	SceneNode* top = new SceneNode(new UnitSquare(), &evn_top);
	scene.push_back(top);




	SceneNode* bot = new SceneNode(new UnitSquare(), &evn_bot);
	scene.push_back(bot);

	SceneNode* left = new SceneNode(new UnitSquare(), &evn_left);
	scene.push_back(left);

	SceneNode* right = new SceneNode(new UnitSquare(), &evn_right);
	scene.push_back(right);

	SceneNode* front = new SceneNode(new UnitSquare(), &evn_front);
	scene.push_back(front);

	SceneNode* back = new SceneNode(new UnitSquare(), &evn_back);
	scene.push_back(back);

	// Starter code rotate the axis as well, which is pretty bad!!!!!!!!!
	front->translate(Vector3D(0,0,-0.5 * box_width));
	back ->translate(Vector3D(0,0,0.5 * box_width));
	back ->rotate('y',-180);

	left ->translate(Vector3D(-0.5 * box_length,0,0));
	left ->rotate('y',90);

	right ->translate(Vector3D(0.5 * box_length,0,0));
	right ->rotate('y',-90);

	top ->translate(Vector3D(0,0.5 * box_height,0));
	top ->rotate('x',90);

	bot ->translate(Vector3D(0,-0.5 * box_height,0));
	bot ->rotate('x',-90);

	double front_factor[3] = { box_length, box_height, 1 };
	front->scale(Point3D(0, 0, 0), front_factor);
	back->scale(Point3D(0, 0, 0), front_factor);

	double left_right_factor[3] = { box_width, box_height, 1 };
	left->scale(Point3D(0, 0, 0), left_right_factor);
	right->scale(Point3D(0, 0, 0), left_right_factor);

	double top_bot_factor[3] = {box_length, box_width, 1};
	top->scale(Point3D(0, 0, 0), top_bot_factor);
	bot->scale(Point3D(0, 0, 0), top_bot_factor);

	// =======================================================================
	// ||              		  	 Glossy reflection														||
	// =======================================================================

	// mirror_ball
	Material mirror(Color(0.1, 0.1, 0.1), Color(0.65164, 0.50648, 0.12648),
                Color(0.828281, 0.155802, 0.566065), 100000,0.8,0);

	SceneNode* mirror_ball = new SceneNode(new UnitSphere(), &mirror);
	scene.push_back(mirror_ball);
	double mirror_ball_factor[3] = {0.7, 0.7, 0.27};
	mirror_ball->translate(Vector3D(-0.45 * box_length + mirror_ball_factor[0] + 0.1,-0.5*box_height+mirror_ball_factor[1]+0.01 + 0.4,-0.5 * box_width +mirror_ball_factor[2] + 0.5));
	mirror_ball->scale(Point3D(0, 0, 0), mirror_ball_factor);


	// =======================================================================
	// ||              		  	 	Non-TRIVIAL																	||
	// =======================================================================

	Material gold(Color(0.3, 0.3, 0.3), Color(0.75164,0.60648,0.22648),
		Color(0.628281, 0.555802, 0.366065),
		51.2);

	SceneNode* cone = new SceneNode(new UnitCone(), &gold);
	scene.push_back(cone);
	double con_factor[3] = {0.3, 0.12, 0.5};
	cone->translate(Vector3D(-1.9,-1.33,-0.1));
	cone->rotate('x',90);
	cone->scale(Point3D(0,0,0),con_factor);

	// SceneNode* cone2 = new SceneNode(new UnitCone(), &gold);
	// scene.push_back(cone2);
	// double con_factor2[3] = {0.3, 0.12, 1.2};
	// cone2->translate(Vector3D(1.2,-0.5,-1.0));
	// cone2->rotate('x',90);
	// cone2->scale(Point3D(0,0,0),con_factor2);

	SceneNode* cylinder = new SceneNode(new UnitCylinder(), &gold);
	scene.push_back(cylinder);
	double cyl_factor[3] = {0.15, 0.15, 1.2};
	cylinder->translate(Vector3D(0.45,-1.55,-0.2));
	cylinder->rotate('y',125);
	cylinder->scale(Point3D(0,0,0),cyl_factor);


	SceneNode* cylinder2 = new SceneNode(new UnitCylinder(), &gold);
	scene.push_back(cylinder2);
	double cyl_factor2[3] = {0.3, 0.15, 0.4};
	cylinder2->translate(Vector3D(-0.45 * box_length + mirror_ball_factor[0] + 0.1,-1.55,-0.5 * box_width +mirror_ball_factor[2] + 0.5));
	cylinder2->rotate('x',90);
	cylinder2->scale(Point3D(0,0,0),cyl_factor2);



	SceneNode* cylinder3 = new SceneNode(new UnitCylinder(), &gold);
	scene.push_back(cylinder3);
	double cyl_factor3[3] = {0.2, 0.1, 2.0};
	cylinder3->translate(Vector3D(1.75,-0.7,-0.9));
	cylinder3->rotate('x',90);
	cylinder3->rotate('y',-30);
	cylinder3->scale(Point3D(0,0,0),cyl_factor3);


	// =======================================================================
	// ||                 					Texture																	||
	// =======================================================================

	Material globe_mat(Color(0.3, 0.3, 0.3), Color(0.75164,0.60648,0.22648),
		Color(0.628281, 0.555802, 0.366065),
		51.2);
	unsigned long int globe_texture_width;
	long int globe_texture_height;
	unsigned char*  globe_rarray;
	unsigned char*  globe_garray;
	unsigned char*  globe_barray;

	bmp_read("./textures/earthmap.bmp", &globe_texture_width, &globe_texture_height, &globe_rarray, &globe_garray, &globe_barray);
	Texture texture_globe(true,globe_texture_width,globe_texture_height,globe_rarray,globe_garray,globe_barray);
	globe_mat.texture = &texture_globe;

	SceneNode* globe = new SceneNode(new UnitSphere(), &globe_mat);
	scene.push_back(globe);
	double globe_factor[3] = {0.4, 0.4, 0.11};
	globe->translate(Vector3D(-1.9,-1.0,-0.1));
	globe->scale(Point3D(0,0,0),globe_factor);


	// =======================================================================
	// ||                 	   	 	refraction																||
	// =======================================================================
	// crystal
	Material glass(Color(0.1, 0.1, 0.1), Color(0.4, 0.4, 0.4),
                Color(0.828281, 0.155802, 0.566065), 50,0,1.3);

	SceneNode* glass_ball = new SceneNode(new UnitSphere(), &glass);
	scene.push_back(glass_ball);
	double glass_ball_factor[3] = {0.4, 0.4, 0.2};
	glass_ball->translate(Vector3D(1.75,0,-0.5));
	glass_ball->scale(Point3D(0, 0, 0), glass_ball_factor);







	// =======================================================================
	// ||                 	   	 	Camera Settings														||
	// =======================================================================

	// Render the scene, feel free to make the image smaller for
	// testing purposes.
	Camera camera1(eye, viewdir, Vector3D(0, 1, 0), fov);
	Image image1(width, height);
	raytracer.render(camera1, scene, light_list, image1); //render 3D scene to image
	image1.flushPixelBuffer("view1.bmp"); //save rendered image to file

	// Render it from a different point of view.
	Camera camera2(eye2, viewdir2, Vector3D(0, 1, 0), fov2);
	Image image2(width, height);
	raytracer.render(camera2, scene, light_list, image2);
	image2.flushPixelBuffer("view2.bmp");

	// Free memory
	for (size_t i = 0; i < scene.size(); ++i) {
		delete scene[i];
	}

	for (size_t i = 0; i < light_list.size(); ++i) {
		delete light_list[i];
	}

	return 0;
}
