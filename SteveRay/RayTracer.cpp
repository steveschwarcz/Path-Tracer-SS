#include "stdafx.h"
#include "RayTracer.h"
#include "Sphere.h"
#include <iostream>




RayTracer::~RayTracer()
{
}

bool RayTracer::trace(const Ray& ray, Radiance3& L_o, int depth) const
{
	float distance = INFINITY;
	bool result = false;
	L_o = m_skyColor;

	//don't recurse forever
	if (depth == 0) return false;

	SurfaceElement surfel;

	result = m_scene->intersectRay(ray, distance, surfel);

	if (!result)
	{
		return false;
	}

	//direction of outgoing ray
	const Vector3& w_o = ray.direction();

	//TODO MOVE
	bool inside = false;

	//find cos of incidence
	float cosI = - w_o.dot(surfel.shader().normal);

	//if cosI is positive, ray is inside of primitive 
	if (cosI < 0.0f)
	{
		inside = true;
	}

	//both indexes of refraction, n1 / n2, and sin T squared
	float n1, n2, n, sinT2;

	//compute values that will be needed later
	computeSinT2AndRefractiveIndexes(surfel, cosI, sinT2, n1, n2, n);

	//compute fresnel
	const float fresnelReflective = computeFresnelForReflectance(cosI, sinT2, n1, n2, n);
	

	//emit
	L_o += surfel.material()->emit();

	//shade the primitive if ray is outside
	if (!inside)
		L_o += calculateDirectLight(surfel, -1.0 * w_o, m_numShadowSamples, fresnelReflective, true);

	Color3 L_i(0, 0, 0);
	calculateIndirectLight(ray, surfel, fresnelReflective, cosI, sinT2, n, L_i, depth);
	L_o += L_i;

	return result;
}

bool RayTracer::calculateIndirectLight(Ray const& ray, const SurfaceElement& surfel, const float fresnelReflective, const float cosI, const float sinT2, const float n, Color3& L_o, int depth) const
{
	//is the primitive mirror reflective?
	bool mirrorReflectance = surfel.material()->glossyExponent() == INFINITY;

	//mirror reflection
	if (mirrorReflectance && surfel.material()->glossyAverage() > 0.0f)
	{
		Radiance3 L_r;

		//if nothing is hit, use default color
		trace(reflectedRay(ray.direction(), surfel, cosI), L_r, depth - 1);
		L_o += L_r.cwiseProduct(surfel.material()->glossyColor() * surfel.material()->glossyAverage() * fresnelReflective);
	}

	//refraction
	if (surfel.material()->refractiveAverage() > 0.0f)
	{
		const float fresnelRefractive = 1.0f - fresnelReflective;

		Radiance3 L_r;

		Ray refRay;

		//if nothing is hit, use default color
		if (refractedRay(ray.direction(), surfel, cosI, sinT2, n, refRay))
		{
			trace(refRay, L_r, depth - 1);
			L_o += (L_r * fresnelRefractive) * surfel.material()->refractiveAverage();
		}

	}

	return true;
}