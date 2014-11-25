// SteveRay.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RayTracer.h"
#include "Sphere.h"
#include "Material.h"
#include "PathTracer.h"
#include "PointLight.h"
#include <ctime>


void makeCornellBox(Scene& scene);
void addStanfordDragon(Scene& scene);
void addSpheres(Scene& scene);
void addStandingModel(Scene& scene);


Renderer *renderer;

/*
2 Optional Comand line arguments:
	Path/Ray: Path tracer or ray tracer
	numRays: Number of rays per pixel
*/

int _tmain(int argc, _TCHAR* argv[])
{
	bool useRay = true;
	int raysPerPixel = 4;
	//get command line arguments
	if (argc > 1 && _tcscmp(argv[1], _T("path")) == 0)
	{
		useRay = false;
		std::cout << "Render type: Path Tracing" << std::endl;
	}
	else
	{
		std::cout << "Render type: Ray Tracing" << std::endl;
	}
	if (argc > 2)
	{
		raysPerPixel = _tstoi(argv[2]);
		std::cout << "Ray per pixel: " << raysPerPixel << std::endl;
	}

	//start timer
	std::clock_t timer_start;
	double timer_duration;

	//start timer
	timer_start = std::clock();
	Image image = Image(800, 600, Color3(0.0f, 0.0f, 0.0f));
	Scene scene = Scene();
	Camera camera = Camera();

	//make floor and walls
	makeCornellBox(scene);
//	addSpheres(scene);
//	addStanfordDragon(scene);
	addStandingModel(scene);

	//end timer
	timer_duration = (std::clock() - timer_start) / static_cast<double>(CLOCKS_PER_SEC);
	std::cout << "Build scene time: " << timer_duration << '\n';
	

	timer_start = std::clock();
	scene.initBVH(8, Point3(0, 4, -4));
	timer_duration = (std::clock() - timer_start) / static_cast<double>(CLOCKS_PER_SEC);
	std::cout << "BVH construction time: " << timer_duration << std::endl;

	if (useRay)
		renderer = new RayTracer(&scene, &camera, &image, 1, 1);
	else
		renderer = new PathTracer(&scene, &camera, &image);


	timer_start = std::clock();
	renderer->render(raysPerPixel);
	//end timer
	timer_duration = (std::clock() - timer_start) / static_cast<double>(CLOCKS_PER_SEC);
	std::cout << "Render time: " << timer_duration << " for " << scene.boxes.size() << " primitives" << std::endl;

	image.save("image.ppm", 1.0f);
	
	return 0;
}

void addSpheres(Scene& scene)
{
	//create materials
	//glossy
	Material *glossyBlue = scene.addMaterial(new Material(0.15f, 0.65f, 0.0f, Color3(0.0f, 0.0f, 1.0f), Color3(0.0f, 0.0f, 1.0f), 200, 1.45f));
	//diffuse
	Material *diffuseWhite = scene.addMaterial(new Material(0.8f, 0.0f, 0.0f, Color3(1.0f, 1.0f, 1.0f), Color3(1, 1, 1), 100.0f, 1.3f));
	//reflective and transparent
	Material *reflRed = scene.addMaterial(new Material(0.3f, 0.5f, 0.0f, Color3(1.0f, 0.0f, 0.0f), Color3(1, 0, 0), INFINITY, 1.5f));
	//reflective and transparent - red
	Material *reflrefrRed = scene.addMaterial(new Material(0.0f, 0.8f, 0.8f, Color3(1.0f, 0.0f, 0.0f), Color3(1, 1, 1), INFINITY, 1.55f));

	//add light
//	scene.addPointLight(PointLight(Power3(200, 200, 200), Point3(0.0f, 2.8f, -0.5f)));

	scene.addSphere(new Sphere(Point3(-0.6f, 1.8f, -3.2f), 0.8f, reflrefrRed));
	scene.addSphere(new Sphere(Point3(2.0f, 2.4f, -2.15f), 0.49f, reflRed));
	scene.addSphere(new Sphere(Point3(0.0f, 3.8f, -4.1f), 0.9f, glossyBlue));
	scene.addSphere(new Sphere(Point3(0.0f, 1.5f, -4.1f), 0.4f, glossyBlue));

	std::cout << "Adding spheres..." << std::endl;
}

void addStars(Scene& scene) {
	Material *redRefrac = scene.addMaterial(new Material(0.0f, 0.8f, 0.8f, Color3(1.0f, 0.0f, 0.0f), Color3(1, 1, 1), 100.0f, 1.4f));

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			AffineTransformation trans =
				(Translation3f(Vector3(3 - (1 * 3), 3, -4)) *
				AngleAxisf(M_PI / 8, Vector3::UnitY()).matrix()) *
				Scaling(0.15f * 1.5f);

			scene.addModel(Model::createModelFromObj("test2.obj", trans, redRefrac));
		}
	}

}

void addStandingModel(Scene& scene)
{
	Material *redRefl = scene.addMaterial(new Material(0.5f, 0.4f, 0.0f, Color3(1.0f, 0.0f, 0.0f), Color3(1, 1, 1), 160.0f, 1.4f));

	AffineTransformation trans =
		(Translation3f(Vector3(3 - (1 * 3), 3, -4)) *
		AngleAxisf(M_PI / 8, Vector3::UnitY()).matrix()) *
		Scaling(0.15f * 1.5f);


	scene.addModel(Model::createModelFromObj("man.obj", trans, redRefl));
	scene.addModel(Model::createModelFromObj("stick.obj", trans, redRefl));

	std::cout << "Adding figure..." << std::endl;
}

void addStanfordDragon(Scene& scene)
{
	Material *blueRefrac = scene.addMaterial(new Material(0.4f, 0.4f, 0.0f, Color3(0.1f, 0.1f, 1.0f), Color3(1, 1, 1), 100.0f, 1.4f));

	AffineTransformation trans =
		(Translation3f(Vector3(0, 0, -4)) *
		AngleAxisf(M_PI / 8, Vector3::UnitY()).matrix()) *
		Scaling(.48f);

	scene.addModel(Model::createModelFromObj("stanford_dragon.obj", trans, blueRefrac));

	std::cout << "Adding dragon..." << std::endl;
}

void makeCornellBox(Scene& scene) {
	Material *plainWall = scene.addMaterial(new Material(0.8f, 0.0f, 0.0f, Color3(1.0f, 1.0f, 0.8f), Color3(1, 1, 1), 100.0f, 1.3f));
	Material *redWall = scene.addMaterial(new Material(0.8f, 0.0f, 0.0f, Color3(1.0f, 0.0f, 0.0f), Color3(1, 1, 1), 100.0f, 1.3f));
	Material *greenWall = scene.addMaterial(new Material(0.8f, 0.0f, 0.0f, Color3(0.0f, 1.0f, 0.0f), Color3(1, 1, 1), 100.0f, 1.3f));
	Material *light = scene.addMaterial(new Material(0.0f, 0.0f, 0.0f, Color3(0.0f, 0.0f, 0.0f), Color3(1, 1, 1), 100.0f, 1.3f, Color3(2.0f, 2.0f, 2.0f)));

	const float wallSize = 8.0f;
	const float offset = wallSize / 2.0f;
	
	//floor
	AffineTransformation trans =
		(Translation3f(Vector3(0, 0, -offset)) *
		AngleAxisf(-M_PI / 2, Vector3::UnitX()).matrix()) *
		Scaling(wallSize);
	scene.addModel(Model::createRectangularModel(trans, plainWall));

	//ceiling
	trans =
		(Translation3f(Vector3(0, wallSize, -offset)) *
		AngleAxisf(M_PI / 2, Vector3::UnitX()).matrix()) *
		Scaling(wallSize);
	scene.addModel(Model::createRectangularModel(trans, plainWall));

	//left wall
	trans =
		(Translation3f(Vector3(-offset, offset, -offset)) *
		AngleAxisf(M_PI / 2, Vector3::UnitY()).matrix()) *
		Scaling(wallSize);
	scene.addModel(Model::createRectangularModel(trans, redWall));

	//right wall
	trans =
		(Translation3f(Vector3(offset, offset, -offset)) *
		AngleAxisf(- M_PI / 2, Vector3::UnitY()).matrix()) *
		Scaling(wallSize);
	scene.addModel(Model::createRectangularModel(trans, greenWall));

	//back wall
	trans =
		(Translation3f(Vector3(0, offset, -wallSize)) *
		//AngleAxisf(M_PI / 2, Vector3::UnitX()).matrix()) *
		Scaling(wallSize));
	scene.addModel(Model::createRectangularModel(trans, plainWall));

	//front wall
	trans =
		(Translation3f(Vector3(0, offset, 0)) *
		AngleAxisf(M_PI, Vector3::UnitX()).matrix()) *
		Scaling(wallSize);
	//scene.addModel(Model::createRectangularModel(trans,plainWall));

	//light at top
	trans =
		(Translation3f(Vector3(0, wallSize - 0.001f, -offset)) *
		AngleAxisf(M_PI / 2, Vector3::UnitX()).matrix()) *
		Scaling(1.5f);
	scene.addAreaLight(AreaLight::createRectangularLight(Power3(400, 400, 400), trans, light));
}