#pragma once
#include "Light.h"
class PointLight :
	public Light
{
public:
	PointLight(Power3 power, Point3 center) : Light(power), m_center(center){};
	~PointLight();

	const Point3 center() const
	{
		return m_center;
	}

private:
	Point3 m_center;
};

