#include <Math\MathFunctions.hpp>


namespace Math 
{
	Quaternion Quaternion::operator*(const Quaternion& other) const 
	{
		return Quaternion(
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y - x * other.z + y * other.w + z * other.x,
			w * other.z + x * other.y - y * other.x + z * other.w,
			w * other.w - x * other.x - y * other.y - z * other.z
		);
	}
	Vector3 Quaternion::operator*(const Vector3& vec) const 
	{
		Quaternion qVec(vec.x, vec.y, vec.z, 0);
		Quaternion result = (*this) * qVec * Conjugate(*this);
		return Vector3(result.x, result.y, result.z);
	}
	Quaternion Quaternion::operator*(float scalar) const
	{
		return Quaternion(
			x * scalar,
			y * scalar,
			z * scalar,
			w * scalar
		);
	}
}