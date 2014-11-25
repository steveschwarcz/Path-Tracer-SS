#pragma once
#include "Renderer.h"
class PathTracer :
	public Renderer
{
public:
	PathTracer(const Scene* scene, const Camera* camera, Image* image) : Renderer(scene, camera, image) {}
	~PathTracer();


private:
	bool trace(const Ray& ray, Color3& l_o, int depth = 1) const override;

	bool PathTracer::calculateIndirectLight(Ray const& ray, const SurfaceElement& surfel, const float fresnelReflective, const float cosI, const float sinT2, const float n, Color3& L_o, int depth) const;
};

