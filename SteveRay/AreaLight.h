#pragma once
#include "Light.h"
#include "Material.h"
#include "Model.h"
#include "Utils.h"

class AreaLight :
	public Light, 
	public Model
{
public:
	AreaLight(Power3 power) : Light(power) { m_rng = rng_uniform(); };

	~AreaLight();

	SurfaceElement sampleRandomPoint(std::tr1::function<float(float, float)> rng);

	static AreaLight* createRectangularLight(const Power3 power, AffineTransformation const& transformation, Material const* material);

private:
	std::tr1::function<float(float, float)> m_rng;

	float m_surfaceArea;
	void calculateTotalArea();
};