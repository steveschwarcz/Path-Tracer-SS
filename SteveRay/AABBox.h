#pragma once
#include "stdafx.h"
#include "Primitive.h"

/// <summary>
/// Encapsulates an axis-aligned bounding box
/// </summary>
class AABBox

	//TODO: DOC
{
public:
	AABBox(const Point3& upper, const Point3& lower, const Primitive* primitive) :
		m_upperCorner(upper), m_lowerCorner(lower), m_centroid((upper + lower) / 2), m_primitive(primitive){};

	~AABBox() {

	}

	const Point3& centroid() const {
		return m_centroid;
	};

	const Point3& upperCorner() const {
		return m_upperCorner;
	}

	const Point3& lowerCorner() const {
		return m_lowerCorner;
	}

	const uint32_t& mortonCentroid() const {
		return m_mortonCentroid;
	}

	void setMortonCentroid(uint32_t morten)
	{
		m_mortonCentroid = morten;
	}

	const Primitive* const primitive() const { return m_primitive; };

	void setPrimitive(const Primitive* primitive) { 
		m_primitive = primitive; }

	bool intersectRay(const Ray& ray, float& distance) const;

	static AABBox getBoxBounds(const Primitive *primitive);

private:
	Point3 m_upperCorner;
	Point3 m_lowerCorner;

	Point3 m_centroid;

	uint32_t m_mortonCentroid;

	const Primitive* m_primitive;
};

