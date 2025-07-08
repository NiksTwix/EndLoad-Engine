#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>
#include <Math/Vector4.hpp>


namespace Math
{
	// Basic operations

	Vector2 Vector2::operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}


	Vector2 Vector2::operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}


	Vector2 Vector2::operator*(const Vector2& other) const
	{
		return Vector2(x * other.x, y * other.y);
	}


	Vector2 Vector2::operator/(const Vector2& other) const
	{
		return Vector2(x / other.x, y / other.y);
	}

	Vector2 Vector2::operator-() const
	{
		return Vector2(-x, -y);
	}

	void Vector2::operator+=(const Vector2& other)
	{
		this->x += other.x;
		this->y += other.y;
	}


	void Vector2::operator-=(const Vector2& other)
	{
		this->x -= other.x;
		this->y -= other.y;
	}


	void Vector2::operator/=(const Vector2& other)
	{
		this->x /= other.x;
		this->y /= other.y;
	}


	void Vector2::operator*=(const Vector2& other)
	{
		this->x *= other.x;
		this->y *= other.y;

	}

	bool Vector2::operator==(const Vector2& other) const
	{
		return x == other.x && y == other.y;
	}

	bool Vector2::operator!=(const Vector2& other) const
	{
		return x != other.x || y != other.y;
	}

	Vector2::operator Vector4() { return Vector4(x, y, 0, 0); }
	Vector2::operator Vector3() { return Vector3(x, y, 0); }
}