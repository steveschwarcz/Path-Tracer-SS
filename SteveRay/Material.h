#pragma once
#include "Color.h"

class Material
{
public:
	Material(float diffuseAverage, float glossyAverage, float refractiveAverage, Color3 diffuseColor, Color3 glossyColor, float glossyExponent, float refractiveIndex, Color3 emitance = Color3(0,0,0)) :
		m_diffuseAverage(diffuseAverage), m_glossyAverage(glossyAverage),
		m_refractiveAverage(refractiveAverage),
		m_diffuseColor(diffuseColor), m_glossyColor(glossyColor),
		m_glossyExponent(glossyExponent), m_refractiveIndex(refractiveIndex),
		m_emitance(emitance){}
	~Material();

	Color3 evaluateLambertianBSDF(const Vector3& w_i, const Vector3& w_o) const;
	Color3 evaluateGlossyBSDF(const Vector3& w_i, const Vector3& w_o, const Vector3& normal, float fresenel) const;

	Color3 diffuseColor() const { return m_diffuseColor; };
	Color3 glossyColor() const { return m_glossyColor; };
	float glossyExponent() const { return m_glossyExponent; };
	float refractiveIndex() const { return m_refractiveIndex; };
	float diffuseAverage() const { return m_diffuseAverage; };
	float glossyAverage() const { return m_glossyAverage; };
	float refractiveAverage() const { return m_refractiveAverage; };

	Color3 emit() const
	{
		return m_emitance;
	};

private:
	Color3 m_diffuseColor;
	Color3 m_glossyColor;
	Color3 m_emitance;
	float m_glossyExponent;
	float m_refractiveIndex;
	float m_diffuseAverage;
	float m_glossyAverage;
	float m_refractiveAverage;
};

