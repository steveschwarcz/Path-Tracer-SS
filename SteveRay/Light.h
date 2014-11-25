#pragma once
#include "SceneObject.h"
#include "Color.h"

class Light
{
public:
	explicit Light(Power3 power) : m_phi(power)
	{
	};

	virtual ~Light();

	const Power3 power() const
	{
		return m_phi;
	};

private:
	Power3 m_phi;
};