#pragma once
#include "stdafx.h"

//class Color3 {
//public:
//	float r, g, b;
//
//	Color3(float r = 0, float g = 0, float b = 0) :
//		r(r), g(g), b(b) {}
//
//	//--------------------------------------------------
//	//			Assignment Operators
//	//--------------------------------------------------
//
//	Color3& operator*=(const Color3& rhs){
//		this->r *= rhs.r;
//		this->g *= rhs.g;
//		this->b *= rhs.b;
//
//		return *this;
//	}
//
//	Color3& operator+=(const Color3& rhs){
//		this->r += rhs.r;
//		this->g += rhs.g;
//		this->b += rhs.b;
//
//		return *this;
//	}
//
//	Color3 operator*(const Color3& rhs) const {
//		return Color3(this->r * rhs.r,
//			this->g * rhs.g,
//			this->b * rhs.b);
//	}
//
//	Color3 operator+(const Color3& rhs) const {
//		return Color3(this->r + rhs.r,
//			this->g + rhs.g,
//			this->b + rhs.b);
//	}
//
//	Color3 operator*(const float& rhs) const{
//		return Color3(this->r * rhs,
//			this->g * rhs,
//			this->b * rhs);
//	}
//
//	Color3 operator/(const Color3& rhs) const{
//		return Color3(this->r / rhs.r,
//			this->g / rhs.g,
//			this->b / rhs.b);
//	}
//
//	Color3 operator/(const float& rhs) const{
//		return Color3(this->r / rhs,
//			this->g / rhs,
//			this->b / rhs);
//	}
//};

typedef Eigen::Vector3f Color3;
typedef Color3 Radiance3;
typedef Color3 Power3;