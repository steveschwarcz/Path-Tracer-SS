#pragma once
#include "stdafx.h"

#define RAY_BUMP_EPSILON 1e-3f

class Ray
{
private:
	Point3 m_origin_p;
	Vector3 m_direction_v;
	float m_lastRefractiveIndex;

public:
	Ray(const Point3& origin, const Vector3& direction, bool bump = false, float refractiveIndex = 1.0f) :
		m_origin_p(origin), m_direction_v(direction), m_lastRefractiveIndex(refractiveIndex) {
		if (bump) {
			m_origin_p += direction * RAY_BUMP_EPSILON;
		}
	}

	~Ray() {}
	Ray() {}

	Point3 origin() const { return m_origin_p; }
	Vector3 direction() const { return m_direction_v; }
	float lastRefractiveIndex() const { return m_lastRefractiveIndex; }
};

