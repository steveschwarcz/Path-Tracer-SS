#include "stdafx.h"
#include "Renderer.h"


Renderer::~Renderer()
{
}

void Renderer::render(int raysPerPixel) const
{
	//progress bar
	ProgressBar progressBar;


	//set height and width
#if _DEBUG
	const int height = DEBUG_HEIGHT;
	const int width = DEBUG_WIDTH;
#else
	const int height = m_image->height();
	const int width = m_image->width();
#endif

	const int dimJitter = static_cast<int>(sqrt(raysPerPixel));
	//doesn't matter what inverse is set to if dimJitter==1, since it won't be used
	const float inverseDimJitter = (dimJitter == 1) ? 0 : 1.0f / (dimJitter - 1);

	//iterate through each pixel
	const int max = width * height;

#if _DEBUG
	for (int y = DEBUG_Y; y < height + DEBUG_Y; ++y) {
		for (int x = DEBUG_X; x < width + DEBUG_X; ++x) {
#else
	//nonconstant time for each iteration - use dynamic scheduling
#pragma omp parallel for schedule(dynamic,1)
	for (int i = 0; i < max; i++)
	{
		int y = i / width;
		int x = i % width;
#endif

		Color3 pixel(0, 0, 0);
		for (int y1 = 0; y1 < dimJitter; ++y1)
		{
			for (int x1 = 0; x1 < dimJitter; ++x1)
			{
				Ray eyeRay;

				//cast a ray through the pixel
				if (dimJitter != 1)
				{ //
					eyeRay = m_camera->computeEyeRay(x + (x1 * inverseDimJitter), y + (y1 * inverseDimJitter), m_image);
				}
				else
				{
					eyeRay = m_camera->computeEyeRay(x + 0.5f, y + 0.5f, m_image);
				}

				//outgoing light at point
				Radiance3 L_o(0, 0, 0);

				//TODO: instead of looping through scene, construct a BVH and test it, removing this outer loop from the equasion
				trace(eyeRay, L_o, m_depth);
				pixel += L_o;
			}
		}

		m_image->setPixel(x, y, pixel / static_cast<float>(dimJitter * dimJitter));

#if _DEBUG
	}
#else
#pragma omp critical
		progressBar.updateProgress(static_cast<int>((static_cast<float>(i) / max) * 100));
#endif
	}

	progressBar.end();
}



bool Renderer::shade(const SurfaceElement& surfel, const Point3& lightPoint, const Power3& lightPower, const Vector3& w_o, const float fresnel, const bool allowGlossy, Radiance3& L_o) const
{
	Vector3 w_i;
	float distance2;

	if (lineOfSight(surfel.shader().position, lightPoint, w_i, distance2))
	{
		const Radiance3& L_i = lightPower / float(4 * M_PI * distance2);

		Radiance3 bsdfResult(0, 0, 0);

		//either shade as glossy or lambertian
		if (allowGlossy && surfel.material()->glossyExponent() != INFINITY)
		{
			bsdfResult = surfel.material()->evaluateGlossyBSDF(w_i, w_o, surfel.shader().normal, fresnel);
		}
		else
		{
			bsdfResult = surfel.material()->evaluateLambertianBSDF(w_i, w_o);
		}

		//scatter the light
		L_o += L_i.cwiseProduct(bsdfResult) *
			fmax(0.0f, surfel.geometry().normal.dot(w_i));

		return true;
	}

	return false;
}


/// <summary>
/// Shades a specified surface element by looping through all lights and calculating direct light.
/// </summary>
/// <param name="surfel">The syrface element.</param>
/// <param name="w_o">The w_o.</param>
/// <param name="numSamples">The number samples to take for area lights.</param>
/// <param name="fresnel">The fresnel.</param>
/// <param name="allowGlossy">If true, will allow glossy scattering to be exstimated by the BSDF.  In a global illumination renderer, this should be false.</param>
Radiance3 Renderer::calculateDirectLight(const SurfaceElement& surfel, const Vector3& w_o, const int numSamples, const float fresnel, const bool allowGlossy) const
{
	Radiance3 L_o(0, 0, 0);

	//for each direction to a light source
	for (std::vector<PointLight>::const_iterator it = m_scene->pointLights.begin(); it != m_scene->pointLights.end(); ++it)
	{
		shade(surfel, it->center(), it->power(), w_o, fresnel, allowGlossy, L_o);
	}

	float inverseNumSamples = 1.0f / static_cast<float>(numSamples);
	for (std::vector<AreaLight*>::const_iterator it = m_scene->areaLights.begin(); it != m_scene->areaLights.end(); ++it)
	{
		for (int i = 0; i < numSamples; i++)
		{
			SurfaceElement surfel2 = (*it)->sampleRandomPoint(m_rng);
			Radiance3 L(0, 0, 0);

			if (shade(surfel, surfel2.shader().position, (*it)->power(), w_o, fresnel, allowGlossy, L))
			{
				//TODO: Aggregate, and average at end
				L_o += L * inverseNumSamples;
			}
		}
	}

	return L_o;
}

/// <summary>
/// Reflects a ray about a normal.
/// </summary>
/// <param name="incident">The incident angle.</param>
/// <param name="surfel">The surface the ray hit.</param>
/// <param name="cosI">Cos i.</param>
/// <returns>A new ray, along the reflected direction.</returns>
Ray Renderer::reflectedRay(const Vector3& incident, const SurfaceElement& surfel, const float cosI) const
{
	Vector3 reflectDirection = incident - 2 * (-cosI) * surfel.shader().normal;

	return Ray(surfel.shader().position, reflectDirection, true);
}

/// <summary>
/// Refracts a ray about a normal.
/// </summary>
/// <param name="incident">The incident angle.</param>
/// <param name="surfel">The surface the ray hit.</param>
/// <param name="cosI">Cos i.</param>
/// <param name="sinT2">Sin t2.</param>
/// <param name="n">The n.</param>
/// <param name="returnedRay">The refracted ray.</param>
/// <returns>true if a ray was returned, false if there is no new ray due to TIR</returns>
bool Renderer::refractedRay(const Vector3& incident, const SurfaceElement& surfel, const float cosI, const float sinT2, const float n, Ray& returnedRay) const
{
	//http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf\

	//check for TIR
	if (sinT2 > 1.0f)
	{
		return false;
	}

	const float cosT = sqrt(1.0f - sinT2);

	Vector3 transmissiveDirection = n * incident + (n * cosI - cosT) * surfel.shader().normal;

	returnedRay = Ray(surfel.shader().position, transmissiveDirection.normalized(), true);

	return true;
}


/// <summary>
/// Computes the fresnel coefficient for reflectance.
/// </summary>
/// <param name="cosI">The cos i.</param>
/// <param name="sinT2">The sin t2.</param>
/// <param name="n1">The index of refraction on the incoming side.</param>
/// <param name="n2">The index of refraction on the outgoing side.</param>
/// <param name="n">n1 / n2.</param>
/// <returns>The fresnel coefficient.</returns>
float Renderer::computeFresnelForReflectance(const float cosI, const float sinT2, const float n1, const float n2, const float n) const
{
	//check for TIR
	if (sinT2 > 1.0f)
	{
		return 1.0;
	}

	const float cosT = sqrt(1.0f - sinT2);

	const float r_s = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
	const float r_p = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);

	return (r_s * r_s + r_p * r_p) * 0.5f;
}

/// <summary>
/// Computes Sin t2 and refractive indexes of a ray intersection.
/// </summary>
/// <param name="surfel">The surface the ray hit.</param>
/// <param name="cosI">Cos i.  Will be flipped to become positive if necessary</param>
/// <param name="sinT2">Sin t2.</param>
/// <param name="n1">The index of refraction on the incoming side.</param>
/// <param name="n2">The index of refraction on the outgoing side.</param>
/// <param name="n">n1 / n2.</param>
void Renderer::computeSinT2AndRefractiveIndexes(SurfaceElement& surfel, float& cosI, float& sinT2, float& n1, float& n2, float& n) const
{
	if (cosI > 0)
	{
		n2 = surfel.material()->refractiveIndex();
		n1 = 1.0f;
	}
	else
	{
		//make sure cos I positive
		cosI = -cosI;

		n1 = surfel.material()->refractiveIndex();
		n2 = 1.0f;
	}

	n = n1 / n2;

	sinT2 = n * n * (1.0f - cosI * cosI);
}

/// <summary>
/// Determines if a line of sight exists between two points
/// </summary>
/// <param name="point0">The first point.</param>
/// <param name="point1">The second point.</param>
/// <param name="direction">The direction of the ray from point0 to point1.</param>
/// <param name="distance2">The distance squared between the two points.</param>
/// <returns>true if a line of sight exists, false otherwise</returns>
bool Renderer::lineOfSight(const Point3& point0, const Point3& point1, Vector3& direction, float& distance2) const
{
	const Vector3& offset = point1 - point0;
	distance2 = offset.squaredNorm();
	float distance = sqrt(distance2);
	direction = offset / distance;

	const Ray losRay = Ray(point0, direction, true);

	//shorten distance.
	distance -= RAY_BUMP_EPSILON;

	//loop through all primitives, seeing if any intersections occur
	SurfaceElement surfel;

	return !(m_scene->intersectRay(losRay, distance, surfel));
}

const Vector3 Renderer::cosHemiRandom(Vector3 const& normal) const
{
	float theta = m_rng(0, 2 * M_PI);
	float s = m_rng(0, 1);
	float y = sqrt(s);
	float r = sqrt(1 - y * y);

	Vector3 sample(r * cos(theta), y, r * sin(theta));
	Eigen::Quaternionf rot = rotateVectorToVector(Vector3::UnitY(), normal);

	return rot * sample;
}

const Vector3 Renderer::cosHemiRandomPhong(const Vector3& w_o, float exponent) const
{
	float theta = m_rng(0, 2 * M_PI);
	float s = m_rng(0, 1);
	float y = pow(s, 1 / (exponent + 1));
	float r = sqrt(1 - y * y);

	Vector3 sample(r * cos(theta), y, r * sin(theta));
	Eigen::Quaternionf rot = rotateVectorToVector(Vector3::UnitY(), w_o);

	return rot * sample;
}

const Eigen::Quaternionf Renderer::rotateVectorToVector(Vector3 const& source, Vector3 const& target) const
{
	Vector3 axis = source.cross(target);
	Eigen::Quaternionf rotation = Eigen::Quaternionf(1.0f + source.dot(target), axis.x(), axis.y(), axis.z());
	return rotation.normalized();
}