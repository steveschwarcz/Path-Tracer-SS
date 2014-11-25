#pragma once
#include "stdafx.h"

/// <summary>
/// A most basic scene object, implying a postion.  Note that destructor is NOT virtual,
/// because this should never be used directly, even polymorphically.
/// </summary>
class SceneObject
{
public:
	SceneObject()
	{
	}

	virtual ~SceneObject()
	{
	}

	const Point3& position() const { return m_position; }

	void setPosition(Point3 position) { m_position = position; }

private:
	Point3 m_position = Point3(0.0f, 0.0f, 0.0f);
};

