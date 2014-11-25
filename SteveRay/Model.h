#pragma once
#include "Triangle.h"
#include "Uncopyable.h"

class Model :
	private Uncopyable
{
public:
	Model();
	virtual ~Model();

	std::vector<Triangle*> triangles()
	{
		return m_triangles;
	}

	//TODO: Move to model builder class
	static Model * createRectangularModel(const AffineTransformation& transformation, const Material *material);
	static Model * createModelFromObj(const std::string objName, const AffineTransformation& transformation, const Material *material);

protected:
	std::vector<Point3* const> m_vertexes = std::vector<Point3* const>();
	std::vector<Vector3* const> m_normals = std::vector<Vector3* const>();

	std::vector<Triangle*> m_triangles = std::vector<Triangle*>();

	static void initRectangularModel(const AffineTransformation& transformation, const Material *material, Model* model);
	static void initModelFromObj(std::string const objName, AffineTransformation const& transformation, Material const* material, Model* model);
};

