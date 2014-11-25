#pragma once
#include "Primitive.h"
#include "Uncopyable.h"

class Triangle :
	public Primitive,
	private Uncopyable
{
public:
	Triangle(Point3* vertex[3], Vector3* normal[3], const Material* material);

	Triangle()
	{
	};

	~Triangle();

	bool intersectRay(const Ray& ray, float& distance, SurfaceElement& surfel) const override;

	void getUpperAndLowerBounds(Point3& upperBounds, Point3& LowerBounds) const override;


	SurfaceElement sampleRandomPoint(std::tr1::function<float(float, float)>& rng) const;

	Point3* const vertex(int i) const
	{
		return m_vertex[i];
	}

	Vector3* const normal(int i) const
	{
		return m_normal[i];
	}

	const float area()
	{
		//initialize area if it is uninitialized
		if (m_area == -1)
		{
			Vector3 edge1 = *(m_vertex[1]) - *(m_vertex[0]);
			Vector3 edge2 = *(m_vertex[2]) - *(m_vertex[0]);
			
			m_area = edge1.cross(edge2).norm() / 2.0f;
		}

		return m_area;
	}

private:
	Point3* m_vertex[3];
	Vector3* m_normal[3];

	float m_area = -1;
};