#pragma once
#include <string>


namespace Math 
{
	class Vector3;

	class Quaternion 
	{
	public:
		float x, y, z, w;

		Quaternion() : x(0), y(0), z(0), w(1) {}
		Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}


		Quaternion operator*(const Quaternion& other) const;

		Quaternion operator*(float scalar) const;

		Vector3 operator*(const Vector3& vec) const;

		std::string ToString() { return std::to_string(x) + ";" + std::to_string(y) + ";" + std::to_string(z) + ";" + std::to_string(w); };	//x,y,z,w
	};

}