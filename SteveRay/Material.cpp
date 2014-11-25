#include "stdafx.h"
#include "Material.h"


Color3 Material::evaluateLambertianBSDF(const Vector3& w_i, const Vector3& w_o) const {
	return m_diffuseColor * (m_diffuseAverage / M_PI);
}

Color3 Material::evaluateGlossyBSDF(const Vector3& w_i, const Vector3& w_o, const Vector3& normal, float fresenel) const {
	
	const Vector3 w_h = (w_i + w_o).normalized();

	return (m_diffuseColor * m_diffuseAverage + m_glossyColor * (fresenel * m_glossyAverage) * ((m_glossyExponent + 8.0f) * powf(std::fmax(0.0f, w_h.dot(normal)), m_glossyExponent) / 8.0f)) / M_PI;
}


Material::~Material()
{
}
