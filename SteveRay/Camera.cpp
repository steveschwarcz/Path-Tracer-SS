#include "stdafx.h"
#include "Camera.h"

Camera::~Camera()
{
}

//FIXME: Clean up this implementation, and pre-compute necessary values
Ray Camera::computeEyeRay(float xPos, float yPos, const Image* image) const {
	const static int width = image->width();
	const static int height = image->height();

	const static float aspectRatio = float(height) / width;

	// Compute the side of a square at z = -1 (the far clipping plane) based on the 
	// horizontal left-edge-to-right-edge field of view

	//multiplying by negative 2 offsets the -.5 in the next step
	const static float s = -2 * tan(fieldOfViewX * 0.5f);

	// xPos / image.width() : map from 0 - 1 where the pixel is on the image
	// <above> * s : TODO
	const Vector3& start = Vector3(((xPos / width) - 0.5f) * s,
		-1 * ((yPos / height) - 0.5f) * s * aspectRatio,
		1.0f)
		* zNear;


//	AffineTransformation trans = Eigen::AngleAxisf(M_PI / 4, Vector3::UnitX()).matrix()

	return Ray(position(), (m_rotation.matrix() * start).normalized());// -position);
}