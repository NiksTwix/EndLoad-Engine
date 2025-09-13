#pragma once
#include <string>
#include <vector>

namespace Math
{
	class Vector2;
	class Vector4;



	class Vector3
	{
	public:
		  
		float x, y, z;
 
		

		Vector3() : x(0), y(0), z(0){}
		explicit Vector3(float value) : x(value), y(value), z(value) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		// Basic operations

		Vector3 operator*(float value) const;
		Vector3 operator/(float value) const;
		Vector3 operator+(const Vector3& other) const;
		Vector3 operator-(const Vector3& other) const;
		Vector3 operator*(const Vector3& other) const;
		Vector3 operator/(const Vector3& other) const;

		Vector3 operator-() const;

		void operator+=(const Vector3& other);
		void operator-=(const Vector3& other);
		void operator*=(const Vector3& other);
		void operator/=(const Vector3& other);

		void operator*=(float value);
		void operator/=(float value);

		bool operator==(const Vector3& other) const;

		bool operator!=(const Vector3& other) const;

		static Vector3 Forward() { return Vector3(0, 0, -1); }	// 0,0,-1,0
		static Vector3 Right() { return Vector3(1, 0, 0); }		// 1,0,0,0
		static Vector3 Up() { return Vector3(0, 1, 0); }		// 0,1,0,0
		static Vector3 Backward() { return Vector3(0, 0, 1); }	// 0,0,1,0
		static Vector3 Left() { return Vector3(-1, 0, 0); }		// -1,0,0,0
		static Vector3 Down() { return Vector3(0, -1, 0); }		// 0,-1,0,0
		static Vector3 XDir() { return Vector3(1, 0, 0); }
		static Vector3 YDir() { return Vector3(0, 1, 0); }
		static Vector3 ZDir() { return Vector3(0, 0, 1); }
		~Vector3() = default;

		operator Vector2();
		operator Vector4();

		std::string ToString() const { return std::to_string(x) + ";" + std::to_string(y) + ";" + std::to_string(z); };

		std::vector<float> ToArray() const { return std::vector<float>({ x,y,z }); }

		static Vector3 FromArray(const std::vector<float>& array)
		{
			if (array.size() < 3) return Vector3();
			return Vector3(array[0], array[1],array[2]);
		}
	};
}