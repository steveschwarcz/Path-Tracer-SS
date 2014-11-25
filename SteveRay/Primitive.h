#pragma once
#include "Ray.h"
#include "SceneObject.h"
#include "SurfaceElement.h"


class Primitive
{
public:
	Primitive()
	{
	};

	virtual ~Primitive() {}

	/// <summary>
	/// Checks to see if the object intersects a ray.
	/// </summary>
	/// <param name="ray">The ray.</param>
	/// <param name="distance">The distance along the ray at which the intersection occurs, as well as the maximum distance of the intersection.</param>
	/// <param name="surfel">The surface element at the point of intersection.</param>
	/// <returns>true on intersection, with an updated surface element and distance, false otherwise.</returns>
	virtual bool intersectRay(const Ray& ray, float& distance, SurfaceElement& surfel) const = 0;

	//TODO: Doc
	virtual void getUpperAndLowerBounds(Point3& upperBounds, Point3& LowerBounds) const = 0;

	const Material * const material() const { return m_material; }

private:
	const Material* m_material;

protected:

	void setMaterial(const Material* material) { m_material = material; }
};

