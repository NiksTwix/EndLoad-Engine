#include <Math\Vector4.hpp>
#include <Math\Vector3.hpp>
#include <Math\Vector2.hpp>

namespace Math
{
	// Basic operations

	Vector3 Vector3::operator+(const Vector3& other) const
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}


	Vector3 Vector3::operator-(const Vector3& other) const
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}


	Vector3 Vector3::operator*(const Vector3& other) const
	{
		return Vector3(x * other.x, y * other.y, z * other.z);
	}


	Vector3 Vector3::operator/(const Vector3& other) const
	{
		return Vector3(x / other.x, y / other.y, z / other.z);
	}

	Vector3 Vector3::operator-() const
	{
		return Vector3(-x, -y, -z);
	}


	void Vector3::operator+=(const Vector3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
	}


	void Vector3::operator-=(const Vector3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
	}


	void Vector3::operator/=(const Vector3& other)
	{
		this->x /= other.x;
		this->y /= other.y;
		this->z /= other.z;
	}


	void Vector3::operator*=(const Vector3& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
	}

	bool Vector3::operator==(const Vector3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	bool Vector3::operator!=(const Vector3& other) const
	{
		return x != other.x || y != other.y || z != other.z;
	}

	Vector3::operator Vector2() { return Vector2(x, y); }
	Vector3::operator Vector4() { return Vector4(x, y, z, 0); }
}