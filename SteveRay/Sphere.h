#pragma once
#include "Primitive.h"
class Sphere :
	public Primitive
{
public:
	Sphere(Point3 center, float radius, const Material *material) : m_centroid(center), m_radius(radius) { setMaterial(material); }
	~Sphere();

	Point3 center() const { return m_centroid; }
	float radius() const { return m_radius; }

	bool intersectRay(const Ray& ray, float& distance, SurfaceElement& surfel) const override;

	void getUpperAndLowerBounds(Point3& upperBounds, Point3& LowerBounds) const override;
private:
	Point3 m_centroid;
	float m_radius;

	SurfaceElement getSurfaceElement(const Ray& ray, float distance) const;
};

