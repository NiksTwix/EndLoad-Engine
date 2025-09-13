#pragma once
#include <string>
#include <vector>
namespace Math
{
	class Vector3;
	class Vector4;

	class Vector2
	{
	public:
		
		float x, y;


		Vector2() : x(0), y(0) {}
		explicit Vector2(float value) : x(value), y(value) {}
		Vector2(float x, float y) : x(x), y(y) {}

		// Basic operations

		Vector2 operator*(float value) const;
		Vector2 operator/(float value) const;


		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;
		Vector2 operator*(const Vector2& other) const;
		Vector2 operator/(const Vector2& other) const;

		Vector2 operator-() const;

		void operator+=(const Vector2& other);
		void operator-=(const Vector2& other);
		void operator*=(const Vector2& other);
		void operator/=(const Vector2& other);

		void operator*=(float value);
		void operator/=(float value);

		bool operator==(const Vector2& other) const;

		bool operator!=(const Vector2& other) const;

		static Vector2 Left() { return Vector2(-1, 0); }	// -1, 0
		static Vector2 Right() { return Vector2(1, 0); }	// 1, 0
		static Vector2 Down() { return Vector2(0, -1); }	// 0, -1
		static Vector2 Up() { return Vector2(0, 1); }		// 0, 1
		static Vector2 XDir() { return Vector2(1, 0); }
		static Vector2 YDir() { return Vector2(0, 1); }

		~Vector2() = default;

		operator Vector3();
		operator Vector4();

		std::string ToString() const { return std::to_string(x) + ";" + std::to_string(y); };

		std::vector<float> ToArray() const { return std::vector<float>({ x,y }); }

		static Vector2 FromArray(const std::vector<float>& array) 
		{
			if (array.size() < 2) return Vector2();
			return Vector2(array[0], array[1]);
		}
	};
}