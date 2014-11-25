#pragma once
#include "Light.h"
#include "Primitive.h"
#include "Sphere.h"
#include "Triangle.h"
#include "PointLight.h"
#include "Model.h"
#include "AreaLight.h"
#include "AABBox.h"
#include "BVH.h"

using Eigen::AngleAxisf;
using Eigen::Scaling;
using Eigen::Translation3f;
using Eigen::Matrix3f;

class Scene
{
public:
	Scene();
	~Scene();

	//TODO: replace with BVH
	std::vector<AABBox> boxes = std::vector<AABBox>();
	BVH bvh;

	void initBVH(const float sceneSize, const Point3& sceneCenter)
	{
		bvh.create(boxes, sceneSize, sceneCenter);
	}

	std::vector<PointLight> pointLights = std::vector<PointLight>();
	std::vector<AreaLight *> areaLights = std::vector<AreaLight *>();

	Material* addMaterial(Material* material)
	{
		//save the material
		m_materials.push_back(material);

		return material;
	}

	AreaLight* addAreaLight(AreaLight* light)
	{
		//save the model
		addModel(light);

		//add area light
		areaLights.push_back(light);

		return light;
	}

	Model* addModel(Model* model)
	{
		//save the model
		m_models.push_back(model);

		int i = 0;

		//temporary reference to the model's triangles
		std::vector<Triangle *> v = model->triangles();

		//iterate over triangles in model and add to scene
		for (std::vector<Triangle *>::iterator it = v.begin(); it != v.end(); ++it)
		{
			boxes.push_back(AABBox::getBoxBounds(*it));
		}

		return model;
	}

	Sphere* addSphere(Sphere* sphere)
	{
		//save the sphere
		m_spheres.push_back(sphere);

		//add the sphere to the scene
		boxes.push_back(AABBox::getBoxBounds(sphere));

		return sphere;
	}

	void addPointLight(PointLight light)
	{
		//save the sphere
		pointLights.push_back(light);
	}

	bool intersectRay(const Ray& ray, float& distance, SurfaceElement& surfel) const;

private:
	//these fields are to keep persistent memory of pointers that must live for the duration fo the scene
	std::vector<Model *> m_models = std::vector<Model *>();
	std::vector<Sphere *> m_spheres = std::vector<Sphere *>();
	std::vector<Material *> m_materials = std::vector<Material *>();
};