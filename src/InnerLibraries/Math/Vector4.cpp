#include <Math\Vector2.hpp>
#include <Math\Vector3.hpp>
#include <Math\Vector4.hpp>

namespace Math
{
	// Basic operations

	Vector4 Vector4::operator+(const Vector4& other) const
	{
		return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
	}


	Vector4 Vector4::operator-(const Vector4& other) const
	{
		return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
	}


	Vector4 Vector4::operator*(const Vector4& other) const
	{
		return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
	}


	Vector4 Vector4::operator/(const Vector4& other) const
	{
		return Vector4(x / other.x, y / other.y, z / other.z, w / other.w);
	}

	Vector4 Vector4::operator-() const 
	{
		return Vector4(-x, -y, -z, -w);
	}

	void Vector4::operator+=(const Vector4& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		this->w += other.w;
	}


	void Vector4::operator-=(const Vector4& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		this->w -= other.w;
	}


	void Vector4::operator/=(const Vector4& other)
	{
		this->x /= other.x;
		this->y /= other.y;
		this->z /= other.z;
		this->w /= other.w;
	}


	void Vector4::operator*=(const Vector4& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
		this->w *= other.w;
	}

	bool Vector4::operator==(const Vector4& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool Vector4::operator!=(const Vector4& other) const
	{
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}

	Vector4::operator Vector2() { return Vector2(x, y); }
	Vector4::operator Vector3() { return Vector3(x, y, z); }
}