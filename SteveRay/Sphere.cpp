#include "stdafx.h"
#include "Sphere.h"

Sphere::~Sphere()
{
}

bool Sphere::intersectRay(const Ray& ray, float& distance, SurfaceElement& surfel) const
{
	Vector3 v = ray.origin() - m_centroid;


	//a = 1, since all direction vectors are normal
	float b = ray.direction().dot(v) * 2;
	float c = v.dot(v) - (m_radius * m_radius);

	float discriminent = (b * b) - 4 * c;

	//negative discriminent = no real roots
	if (discriminent < 0)
		return false;

	discriminent = std::sqrt(discriminent);
	float t0 = (-b + discriminent) / 2;
	float t1 = (-b - discriminent) / 2;

	// make sure t0 is smaller than t1
	if (t0 > t1)
	{
		float temp = t0;
		t0 = t1;
		t1 = temp;
	}

	if (t1 < 0)
	{
		//misses sphere
		return false;
	}

	// intersects at t1, and ray is inside sphere
	if (t0 < 0)
	{
		//sphere too far away
		if (t1 > distance)
		{
			return false;
		}

		distance = t1;
		surfel = getSurfaceElement(ray, distance);
		return true;
	}
	// intersects at t0
	else
	{
		//sphere too far away
		if (t0 > distance)
		{
			return false;
		}

		distance = t0;
		surfel = getSurfaceElement(ray, distance);
		return true;
	}
}

SurfaceElement Sphere::getSurfaceElement(const Ray& ray, float distance) const
{
	Point3 intersectionPoint = ray.origin() + ray.direction() * distance;
	Vector3 normal = (intersectionPoint - m_centroid).normalized();

	return SurfaceElement(intersectionPoint, normal, intersectionPoint, normal, material());
}

void Sphere::getUpperAndLowerBounds(Point3& upperBounds, Point3& LowerBounds) const
{
	LowerBounds = Point3(m_centroid.x() - m_radius,
	                     m_centroid.y() - m_radius,
	                     m_centroid.z() - m_radius);
	upperBounds = Point3(m_centroid.x() + m_radius,
	                     m_centroid.y() + m_radius,
	                     m_centroid.z() + m_radius);
}