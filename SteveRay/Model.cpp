#include "stdafx.h"
#include "Model.h"
#include "../../../../../../TemporaryFileTransfer/tiny_obj_loader.h"
#include <fstream>


Model::Model()
{
}


Model::~Model()
{
	//delete vertexes
	for (size_t i = 0; i < m_vertexes.size(); i++)
		delete m_vertexes[i];

	//delete normals
	for (size_t i = 0; i < m_normals.size(); i++)
		delete m_normals[i];

	//delete triangles
	for (size_t i = 0; i < m_triangles.size(); i++)
		delete m_triangles[i];
}

/// <summary>
/// Creates a rectangular model.  Good for area lights, floors, and walls.  The untransformed rectangle has corners at x,y = -0.5,0.5, with a normal facing along the z axis.
/// Not responsible for deleting the model
/// </summary>
/// <param name="transformation">The transformation to apply to the rectangle.</param>
/// <param name="material">The material.</param>
/// <returns>The new rectangular model</returns>
Model * Model::createRectangularModel(const AffineTransformation& transformation, const Material *material)
{
	Model * result = new Model();

	initRectangularModel(transformation, material, result);

	return result;
}

Model* Model::createModelFromObj(std::string const objName, AffineTransformation const& transformation, Material const* material)
{
	Model * result = new Model();

	initModelFromObj(objName, transformation, material, result);

	return result;
}

void Model::initModelFromObj(std::string const objName, AffineTransformation const& transformation, Material const* material, Model* model)
{

	std::ifstream inFile(objName.c_str());

	if (!inFile)
	{
		std::cerr << "unable to open input file: "
			<< objName << " --bailing out! \n";
		return;
	}

	std::string line;
	std::string vertexPrefix("v ");
	std::string normalPrefix("vn ");
	//		std::string uvPrefix("uv ");
	std::string facePrefix("f ");

	int i = 0;

	//TODO: MUCH better parsing
	while (getline(inFile, line))   {
//		std::cout << "line " << i << std::endl;

		std::string input;
		std::stringstream iss(line);
		
		//vertex
		if (!line.compare(0, vertexPrefix.size(), vertexPrefix)) {
			getline(iss, input, ' ');
			getline(iss, input, ' ');

			float x = atof(input.c_str());

			getline(iss, input, ' ');
			float y = atof(input.c_str());

			getline(iss, input);
			float z = atof(input.c_str());


//			printf("  v = (%f, %f, %f)\n", x, y, z);

			//create and transform new vertex
			Point3 *vertex = new Point3(x, y, z);
			*vertex = transformation * (*vertex);

			model->m_vertexes.push_back(vertex);


		}

		//normal
		if (!line.compare(0, normalPrefix.size(), normalPrefix)) {
			getline(iss, input, ' ');
			getline(iss, input, ' ');

			float x = atof(input.c_str());

			getline(iss, input, ' ');
			float y = atof(input.c_str());

			getline(iss, input);
			float z = atof(input.c_str());


			//create and transform new normal
			Vector3 *normal = new Vector3(x, y, z);
			*normal = transformation.linear().inverse().transpose() * (*normal);

			model->m_normals.push_back(normal);
		}

		//face
		if (!line.compare(0, facePrefix.size(), facePrefix)) {
			Point3* vertexes[3];
			Vector3* normals[3];
			
			getline(iss, input, ' ');

			getline(iss, input, ' ');

			std::stringstream facess(input);

			getline(facess, input, '/');

			vertexes[0] = model->m_vertexes[atoi(input.c_str()) - 1];
			getline(facess, input, '/');
			getline(facess, input);

			normals[0] = model->m_normals[atoi(input.c_str()) - 1];

			getline(iss, input, ' ');

			facess = std::stringstream(input);

			getline(facess, input, '/');
			vertexes[1] = model->m_vertexes[atoi(input.c_str()) - 1];
			getline(facess, input, '/');
			getline(facess, input);
			normals[1] = model->m_normals[atoi(input.c_str()) - 1];

			getline(iss, input, ' ');

			facess = std::stringstream(input);

			getline(facess, input, '/');
			vertexes[2] = model->m_vertexes[atoi(input.c_str()) - 1];
			getline(facess, input, '/');
			getline(facess, input);
			normals[2] = model->m_normals[atoi(input.c_str()) - 1];


			model->m_triangles.push_back(new Triangle(vertexes, normals, material));

			getline(iss, input);

			facess = std::stringstream(input);

			getline(facess, input, '/');
			vertexes[1] = model->m_vertexes[atoi(input.c_str()) - 1];
			getline(facess, input, '/');
			getline(facess, input);
			normals[1] = model->m_normals[atoi(input.c_str()) - 1];


//			std::cout << "f = (" << (*vertexes[0]).x() << " " << (*vertexes[1]).x() << " " << (*vertexes[2]).x() << ")" << std::endl;

			model->m_triangles.push_back(new Triangle(vertexes, normals, material));
		}

		i++;
	}
}

void Model::initRectangularModel(const AffineTransformation& transformation, const Material *material, Model* model)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			//transform the new point
			Point3 *newVertex = new Point3(i - 0.5f, j - 0.5f, 0.0f);
			*newVertex = transformation * (*newVertex);

			//add them to the vertexes array
			model->m_vertexes.push_back(newVertex);
		}
	}

	Vector3 *newNormal = new Vector3(0.0f, 0.0f, 1.0f);
	//get the inverse-transpose of the transformation matrix, to properly transform the normals
	*newNormal = transformation.linear().inverse().transpose() * (*newNormal);

	//normalize the new normal
	newNormal->normalize();

	//add normal
	model->m_normals.push_back(newNormal);

	//get the vertexes and normals for the triangles
	Point3 *vertexes1[] = { model->m_vertexes[0], model->m_vertexes[1], model->m_vertexes[2] };
	Point3 *vertexes2[] = { model->m_vertexes[3], model->m_vertexes[1], model->m_vertexes[2] };
	Vector3 *normals[] = { model->m_normals[0], model->m_normals[0], model->m_normals[0] };

	//create the triangles
	model->m_triangles.push_back(new Triangle(vertexes1, normals, material));
	model->m_triangles.push_back(new Triangle(vertexes2, normals, material));
}