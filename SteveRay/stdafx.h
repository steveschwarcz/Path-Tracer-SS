// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cfloat>
#include <algorithm>

#include <Eigen/Dense>

//types for points and vectors
typedef Eigen::Vector3f Point3;
typedef Eigen::Vector3f Vector3;
typedef Eigen::Vector4f Point4;
typedef Eigen::Vector4f Vector4;
typedef Eigen::Matrix4f Matrix4;

typedef Eigen::Vector2f Vector2;

typedef Eigen::Transform<float, 3, Eigen::Affine> AffineTransformation;


using std::tr1::shared_ptr;