#pragma once
#include "stdafx.h"
#include "Material.h"
#include <random>

class SurfaceElement
{
public:
	SurfaceElement(){};

	SurfaceElement(Point3 gPosition, Vector3 gNormal, Point3 sPosition, Vector3 sNormal, const Material* m) :
		m_shader(sPosition, sNormal), m_geometry(gPosition, gNormal), m_material(m) {}
	~SurfaceElement();

	struct Shader {
		Point3 position;
		Vector3 normal;

		Shader(Point3 pos, Vector3 norm) : position(pos), normal(norm) {}

		Shader(){};
	};

	struct Geometry {
		Point3 position;
		Vector3 normal;

		Geometry(){};

		Geometry(Point3 pos, Vector3 norm) : position(pos), normal(norm) {}
	};

	/*const Point3& position() const { return m_position; }
	const Vector3& normal() const { return m_normal; }*/
	//TODO: Make const
	Shader shader() const { return m_shader; }
	Geometry geometry() const  { return m_geometry; }
	const Material * const material () const { return m_material; }

	void switchNormal()
	{
		m_shader.normal = m_shader.normal * -1;
		m_geometry.normal = m_geometry.normal * -1;
	}


private:
	/*Point3 m_position;
	Vector3 m_normal;*/
	Shader m_shader;
	Geometry m_geometry;
	const Material* m_material;
	
};