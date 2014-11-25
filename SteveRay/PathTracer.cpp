#include "stdafx.h"
#include "PathTracer.h"
#include <random>




PathTracer::~PathTracer()
{
}


bool PathTracer::trace(Ray const& ray, Color3& L_o, int depth) const
{
	float distance = INFINITY;
	bool result = false;
	L_o = m_skyColor;

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
	float cosI = -w_o.dot(surfel.shader().normal);

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
		L_o +=  calculateDirectLight(surfel, -1.0 * w_o, 1, fresnelReflective, false);

	Color3 L_i(0,0,0);
	calculateIndirectLight(ray, surfel, fresnelReflective, cosI, sinT2, n, L_i, -1);
	L_o += L_i;


	return result;
}

bool PathTracer::calculateIndirectLight(Ray const& ray, const SurfaceElement& surfel, const float fresnelReflective, const float cosI, const float sinT2, const float n, Color3& L_o, int depth) const
{
	//get random value for russain roullete
	float r = m_rng(0, 1);

	const Material& material = *(surfel.material());

	//first check for diffuse indirect light
	if (material.diffuseAverage() > 0.0f)
	{
		//diffuse reflection
		r -= material.diffuseAverage();

		if (r < 0.0f)
		{
			Color3 L(0, 0, 0);
			Vector3 w_o = cosHemiRandom(surfel.shader().normal);


			trace(Ray(surfel.shader().position, w_o, true), L);

			L_o += material.diffuseColor().cwiseProduct(L);
			return true;
		}
	}
	if (material.glossyAverage() > 0.0f)
	{
		float specAverage = material.glossyAverage() * fresnelReflective;

		//glossy reflection
		r -= specAverage;

		if (r < 0.0f)
		{
			Color3 L(0, 0, 0);
			Ray reflRay = reflectedRay(ray.direction(), surfel, cosI);
			Vector3 w_o;
			//glossy phong scattering
			if (material.glossyExponent() != INFINITY) {
				w_o = cosHemiRandomPhong(reflRay.direction(), material.glossyExponent());
				trace(Ray(surfel.shader().position, w_o, true), L);
			}
			//mirror reflectance
			else
			{
				trace(reflRay, L);
			}

			L_o += material.glossyColor().cwiseProduct(L);
			return true;
		}
	}
	if (material.refractiveAverage() > 0.0f)
	{
		const float fresnelRefractive = 1.0f - fresnelReflective;

		//refraction
		r -= material.refractiveAverage() * fresnelRefractive;

		if (r < 0.0f)
		{
			Color3 L(0, 0, 0);
			Ray refrRay;
			refractedRay(ray.direction(), surfel, cosI, sinT2, n, refrRay);

			Vector3 w_o;
			trace(refrRay, L);

			//TODO: Apply Beer's law
			L_o += L;
			return true;
		}
	}

	//absorbed;
	return false;
}