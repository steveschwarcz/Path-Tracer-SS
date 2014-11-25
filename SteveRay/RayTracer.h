#pragma once
#include "Image.h"
#include "Scene.h"
#include "Camera.h"
#include "Renderer.h"

class RayTracer :
	public Renderer
{
public:
	RayTracer(const Scene* scene, const Camera* camera, Image* image, int depth, int shadowSamples) : 
		Renderer(scene, camera, image), m_numShadowSamples(shadowSamples) { m_rng = rng_uniform(); m_depth = depth; };
	~RayTracer();

private:
	int m_numShadowSamples;
	bool trace(const Ray& ray, Color3& l_o, int depth) const override;

	bool RayTracer::calculateIndirectLight(Ray const& ray, const SurfaceElement& surfel, const float fresnelReflective, const float cosI, const float sinT2, const float n, Color3& L_o, int depth) const;
};

