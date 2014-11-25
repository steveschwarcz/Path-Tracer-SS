#include "stdafx.h"
#include "Triangle.h"


Triangle::Triangle(Point3* vertex[3], Vector3* normal[3], const Material* material)
{
	m_vertex[0] = vertex[0];
	m_vertex[1] = vertex[1];
	m_vertex[2] = vertex[2];
	m_normal[0] = normal[0];
	m_normal[1] = normal[1];
	m_normal[2] = normal[2];

	setMaterial(material);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);
}


Triangle::~Triangle()
{
}

void Triangle::getUpperAndLowerBounds(Point3& upperBounds, Point3& LowerBounds) const
{
	using std::fmin;
	using std::fmax;

	LowerBounds = Vector3(fmin(vertex(0)->x(), fmin(vertex(1)->x(), vertex(2)->x())),
	                        fmin(vertex(0)->y(), fmin(vertex(1)->y(), vertex(2)->y())),
	                        fmin(vertex(0)->z(), fmin(vertex(1)->z(), vertex(2)->z())));
	upperBounds = Vector3(fmax(vertex(0)->x(), fmax(vertex(1)->x(), vertex(2)->x())),
	                        fmax(vertex(0)->y(), fmax(vertex(1)->y(), vertex(2)->y())),
	                        fmax(vertex(0)->z(), fmax(vertex(1)->z(), vertex(2)->z())));
}

/// <summary>
/// Samples a random point on the triangle.
/// </summary>
/// <param name="rng">The RNG.</param>
/// <returns>A surface element for a random point on the triangle</returns>
SurfaceElement Triangle::sampleRandomPoint(std::tr1::function<float(float, float)>& rng) const
{
	Vector3 random(rng(0, 1), rng(0, 1), rng(0, 1));
	Vector3 weights = random / random.sum();

	Vector3 normal = Vector3(*m_normal[0] * weights.x() +
		*m_normal[1] * weights.y() +
		*m_normal[2] * weights.z()).normalized();

	Point3 point = (*m_vertex[0] * weights.x()) + (*m_vertex[1] * weights.y()) + (*m_vertex[2] * weights.z()) + RAY_BUMP_EPSILON * normal;

	return SurfaceElement(point, normal, point, normal, material());
}

bool Triangle::intersectRay(const Ray& ray, float& distance, SurfaceElement& surfel) const
{
	float weight[3];

	const Vector3& e1 = *m_vertex[1] - *m_vertex[0];
	const Vector3& e2 = *m_vertex[2] - *m_vertex[0];
	const Vector3& q = ray.direction().cross(e2);

	const float a = e1.dot(q);

	const Vector3& s = ray.origin() - *m_vertex[0];
	const Vector3& r = s.cross(e1);

	weight[1] = s.dot(q) / a;
	weight[2] = ray.direction().dot(r) / a;
	weight[0] = 1.0f - (weight[1] + weight[2]);

	const float dist = e2.dot(r) / a;

	//epsilon to check bounds of a if a is close to 0
	static const float epsilon = 1e-7f;
	static const float epsilon2 = 1e-10f;

	if ((fabs(a) <= epsilon) || (weight[0] < -epsilon2) || (weight[1] < -epsilon2)
		|| (weight[2] < -epsilon2) || (dist <= 0.0f) || (dist > distance))
	{
		//ray is parellel to triangle, or behind triangle, or misses triangle
		return false;
	}

	Vector3 normalResult = Vector3(*normal(0) * weight[0] +
		*normal(1) * weight[1] +
		*normal(2) * weight[2]).normalized();

	Point3 intersectionPoint = ray.origin() + ray.direction() * dist;

	surfel = SurfaceElement(intersectionPoint, normalResult, intersectionPoint, normalResult, material());

	distance = dist;
	return true;
}