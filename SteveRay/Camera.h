#pragma once
#include "Ray.h"
#include "SceneObject.h"
#include "Image.h"

//TODO: Create camera with a transformation
/// <summary>
/// Camera for scene.  Has a pinhole aperture, an instantaneous shutter, 
/// and artifical near and far planes of negative z values. By default located at origin
/// and faces along the z-axis
/// </summary>
class Camera :
	public SceneObject
{
public:
	~Camera();

	float zNear;
	float zFar;
	float fieldOfViewX;

	Camera() : zNear(-0.1f), zFar(-100.0f), fieldOfViewX(float(M_PI) / 2.0f)
		//FIXME: This is obviously not how this should be done
	{
		setPosition(Point3(0.0f, 4.0f, 1.8f));
	};

	/// <summary>
	/// Computes the eye ray from the camera through the center of a pixel.
	/// </summary>
	/// <param name="xPos">The x position on screen space.</param>
	/// <param name="yPos">The y position on screen space.</param>
	/// <param name="image">The image.</param>
	/// <returns>A ray fromt he camera through a point on the screen</returns>
	Ray computeEyeRay(float xPos, float yPos, const Image* image) const;

private:
	Eigen::AngleAxisf m_rotation = Eigen::AngleAxisf(0, Vector3::UnitX());
};

