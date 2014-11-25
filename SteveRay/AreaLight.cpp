#include "stdafx.h"
#include "AreaLight.h"

//
//AreaLight::AreaLight()
//{
//}


AreaLight::~AreaLight()
{
}

//TODO: Remove repeated code.  Perhaps with a template?
AreaLight* AreaLight::createRectangularLight(const Power3 power, AffineTransformation const& transformation, Material const* material)
{
	AreaLight * result = new AreaLight(power);

	initRectangularModel(transformation, material, result);

	result->calculateTotalArea();

	return result;
}

/// <summary>
/// Calculates and stores the total surface area of this light.
/// </summary>
void AreaLight::calculateTotalArea()
{
	m_surfaceArea = 0;
	
	//get the surface area of each triangle
	for (std::vector<Triangle *>::const_iterator it = m_triangles.begin(); it != m_triangles.end(); ++it)
	{
		m_surfaceArea += (*it)->area();
	}
}

//TODO: Refactor to take rng as param
/// <summary>
/// Samples a random point on the light uniformly with respect to area.
/// </summary>
/// <returns></returns>
SurfaceElement AreaLight::sampleRandomPoint(std::tr1::function<float(float, float)> rng)
{
	//double u = m_rng(0, m_surfaceArea - 0e-4f);
	double u = m_rng(0, m_surfaceArea - 1e-4f);

	Triangle *triangleToSample = *(m_triangles.end() - 1);

	//loop through triangles, subtracting the area until 0 is reached, at which point that triangle is used
	for (std::vector<Triangle *>::iterator it = m_triangles.begin(); it != m_triangles.end(); ++it)
	{
		u -= (*it)->area();

		if (u <= 0.0f)
		{
			triangleToSample = (*it);
			break;
		}
	}

	//sample the traingle randomly
	//TODO: Clean
	SurfaceElement result = triangleToSample->sampleRandomPoint(m_rng);
	return result;
}