#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{
}


Scene::~Scene()
{
	for (size_t i = 0; i < m_spheres.size(); i++)
		delete m_spheres[i];

	for (size_t i = 0; i < m_models.size(); i++)
		delete m_models[i];

	for (size_t i = 0; i < m_materials.size(); i++)
		delete m_materials[i];
}

bool Scene::intersectRay(const Ray& ray, float& distance, SurfaceElement& surfel) const
{
	return bvh.intersectRay(ray, distance, surfel);
}