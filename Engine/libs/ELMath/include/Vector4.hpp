#pragma once
#include <string>
#include <vector>

namespace Math 
{
	class Vector2;
	class Vector3;



	class Vector4
	{
	public:

		alignas(16) float x, y, z, w;


		Vector4() : x(0), y(0), z(0), w(0) {}
		explicit Vector4(float value) : x(value), y(value), z(value), w(value) {}
		Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		// Basic operations

		Vector4 operator*(float value) const;
		Vector4 operator/(float value) const;

		Vector4 operator+(const Vector4& other) const;
		Vector4 operator-(const Vector4& other) const;
		Vector4 operator*(const Vector4& other) const;
		Vector4 operator/(const Vector4& other) const;

		Vector4 operator-() const;

		void operator+=(const Vector4& other);
		void operator-=(const Vector4& other);
		void operator*=(const Vector4& other);
		void operator/=(const Vector4& other);

		void operator*=(float value);
		void operator/=(float value);

		bool operator==(const Vector4& other) const;

		bool operator!=(const Vector4& other) const;

		static Vector4 Forward() { return Vector4(0, 0, -1, 0); }	// 0,0,-1,0
		static Vector4 Right() { return Vector4(1, 0, 0, 0); }		// 1,0,0,0
		static Vector4 Up() { return Vector4(0, 1, 0, 0); }		// 0,1,0,0
		static Vector4 Backward() { return Vector4(0, 0, 1, 0); }	// 0,0,1,0
		static Vector4 Left() { return Vector4(-1, 0, 0, 0); }		// -1,0,0,0
		static Vector4 Down() { return Vector4(0, -1, 0, 0); }		// 0,-1,0,0
		static Vector4 XDir() { return Vector4(1, 0, 0, 0); }
		static Vector4 YDir() { return Vector4(0, 1, 0, 0); }
		static Vector4 ZDir() { return Vector4(0, 0, 1, 0); }
		static Vector4 WDir() { return Vector4(0, 0, 0, 1); }

		~Vector4() = default;

		operator Vector2();
		operator Vector3();

		std::string ToString() const { return std::to_string(x) + ";" + std::to_string(y) + ";" + std::to_string(z) + ";" + std::to_string(w); };

		std::vector<float> ToArray() const { return std::vector<float>({ x,y,z,w }); }

		static Vector4 FromArray(const std::vector<float>& array)
		{
			if (array.size() < 4) return Vector4();
			return Vector4(array[0], array[1], array[2], array[3]);
		}
	};
}