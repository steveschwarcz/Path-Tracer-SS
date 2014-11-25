#pragma once
#include "SurfaceElement.h"
#include "Ray.h"
#include "Camera.h"
#include "Scene.h"

//variables for debugging, to only render certain portions of the image
#define DEBUG_X 283 //352

#define DEBUG_Y 240 //142

#define DEBUG_WIDTH 2
#define DEBUG_HEIGHT 2

class Renderer
{
public:
	Renderer(const Scene* scene, const Camera* camera, Image* image) : m_scene(scene), m_camera(camera), m_image(image) { m_rng = rng_uniform(); };
	virtual ~Renderer();

	/// <summary>
	/// Renders the specified scene into an image.
	/// </summary>
	/// <param name="image">The image.</param>
	/// <param name="scene">The scene.</param>
	/// <param name="camera">The camera.</param>
	/// <param name="raysPerPixel">The numebr of rays to shoot per pixel.  If this number is not a square, it will be rounded down to the nearest square.</param>
	void render(int raysPerPixel = 1) const;



	//TODO: Clean
	const Vector3 cosHemiRandom(const Vector3& normal) const;
	const Vector3 cosHemiRandomPhong(const Vector3& w_o, float exponent) const;

	const Eigen::Quaternionf Renderer::rotateVectorToVector(Vector3 const& source, Vector3 const& target) const;

protected:
	Radiance3 calculateDirectLight(const SurfaceElement& surfel, const Vector3& w_o, const int numSamples = 1, const float fresnel = 1.0f, const bool allowGlossy = false) const;

	bool shade(const SurfaceElement& surfel, const Point3& lightPoint, const Power3& lightPower, const Vector3& w_o, const float fresnel, const bool allowGlossy, Radiance3& L_o) const;

	bool lineOfSight(const Point3& point0, const Point3& point1, Vector3& direction, float& distance2) const;

	
	Ray reflectedRay(const Vector3& incident, const SurfaceElement& surfel, const float cosI) const;
	bool refractedRay(const Vector3& incident, const SurfaceElement& surfel, const float cosI, const float sinT2, const float n, Ray& returnedRay) const;
	float computeFresnelForReflectance(const float cosI, const float sinT2, const float n1, const float n2, const float n) const;
	void computeSinT2AndRefractiveIndexes(SurfaceElement& surfel, float& cosI, float& sinT2, float& n1, float& n2, float& n) const;

	virtual bool trace(const Ray& ray, Color3& l_o, int depth = 1) const = 0;
	
	
	std::tr1::function<float(float, float)> m_rng;
	Radiance3 m_skyColor = Radiance3(0.0f, 0.0f, 0.0f);
	const Scene* m_scene;
	Image *m_image;
	const Camera* m_camera;
	int m_depth = -1;		//-1 means that there is no limit to depth
};

