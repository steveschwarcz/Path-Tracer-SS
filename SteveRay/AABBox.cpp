#include "stdafx.h"
#include "AABBox.h"
#include "Ray.h"

bool AABBox::intersectRay(const Ray& ray, float& distance) const {
	//TODO: this can be precomputed
	//the inverse of the values in the ray's direction vector
	Vector3 inverseDirection(1.0f / ray.direction().x(),
		1.0f / ray.direction().y(),
		1.0f / ray.direction().z());

	//find the intersection with the closest and farthest lines on all three axes
	float tx1 = (m_lowerCorner.x() - ray.origin().x()) * inverseDirection.x();
	float tx2 = (m_upperCorner.x() - ray.origin().x()) * inverseDirection.x();
	float ty1 = (m_lowerCorner.y() - ray.origin().y()) * inverseDirection.y();
	float ty2 = (m_upperCorner.y() - ray.origin().y()) * inverseDirection.y();
	float tz1 = (m_lowerCorner.z() - ray.origin().z()) * inverseDirection.z();
	float tz2 = (m_upperCorner.z() - ray.origin().z()) * inverseDirection.z();

	//the highest closer intersection
	float tNear(fmax(fmax(fmin(tx1, tx2), fmin(ty1, ty2)), fmin(tz1, tz2)));

	//the lowest farther intersection
	float tFar(fmin(fmin(fmax(tx1, tx2), fmax(ty1, ty2)), fmax(tz1, tz2)));

	//missed
	if (tNear > tFar) {
		return false;
	}
	//behind ray
	if (tFar < 0) {
		return false;
	}
	//distance of box is more than max distance
	if (tNear > distance) {
		return false;
	}

	return true;
}

AABBox AABBox::getBoxBounds(const Primitive* primitive)
{
	Point3 upper;
	Point3 lower;

	primitive->getUpperAndLowerBounds(upper, lower);

	return AABBox(upper, lower, primitive);
}