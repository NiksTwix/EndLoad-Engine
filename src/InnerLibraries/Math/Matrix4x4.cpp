#include <Math\Vector4.hpp>
#include <Math\Matrix4x4.hpp>


namespace Math 
{
	Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const {
		return Matrix4x4(
			x0 + other.x0, x1 + other.x1, x2 + other.x2, x3 + other.x3,
			y0 + other.y0, y1 + other.y1, y2 + other.y2, y3 + other.y3,
			z0 + other.z0, z1 + other.z1, z2 + other.z2, z3 + other.z3,
			w0 + other.w0, w1 + other.w1, w2 + other.w2, w3 + other.w3
		);
	}

	// Вычитание матриц
	Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const {
		return Matrix4x4(
			x0 - other.x0, x1 - other.x1, x2 - other.x2, x3 - other.x3,
			y0 - other.y0, y1 - other.y1, y2 - other.y2, y3 - other.y3,
			z0 - other.z0, z1 - other.z1, z2 - other.z2, z3 - other.z3,
			w0 - other.w0, w1 - other.w1, w2 - other.w2, w3 - other.w3
		);
	}

	// Умножение матриц
	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
		return Matrix4x4(
			x0 * other.x0 + x1 * other.y0 + x2 * other.z0 + x3 * other.w0,
			x0 * other.x1 + x1 * other.y1 + x2 * other.z1 + x3 * other.w1,
			x0 * other.x2 + x1 * other.y2 + x2 * other.z2 + x3 * other.w2,
			x0 * other.x3 + x1 * other.y3 + x2 * other.z3 + x3 * other.w3,

			y0 * other.x0 + y1 * other.y0 + y2 * other.z0 + y3 * other.w0,
			y0 * other.x1 + y1 * other.y1 + y2 * other.z1 + y3 * other.w1,
			y0 * other.x2 + y1 * other.y2 + y2 * other.z2 + y3 * other.w2,
			y0 * other.x3 + y1 * other.y3 + y2 * other.z3 + y3 * other.w3,

			z0 * other.x0 + z1 * other.y0 + z2 * other.z0 + z3 * other.w0,
			z0 * other.x1 + z1 * other.y1 + z2 * other.z1 + z3 * other.w1,
			z0 * other.x2 + z1 * other.y2 + z2 * other.z2 + z3 * other.w2,
			z0 * other.x3 + z1 * other.y3 + z2 * other.z3 + z3 * other.w3,

			w0 * other.x0 + w1 * other.y0 + w2 * other.z0 + w3 * other.w0,
			w0 * other.x1 + w1 * other.y1 + w2 * other.z1 + w3 * other.w1,
			w0 * other.x2 + w1 * other.y2 + w2 * other.z2 + w3 * other.w2,
			w0 * other.x3 + w1 * other.y3 + w2 * other.z3 + w3 * other.w3
		);
	}

	// Умножение на скаляр
	Matrix4x4 Matrix4x4::operator*(float scalar) const {
		return Matrix4x4(
			x0 * scalar, x1 * scalar, x2 * scalar, x3 * scalar,
			y0 * scalar, y1 * scalar, y2 * scalar, y3 * scalar,
			z0 * scalar, z1 * scalar, z2 * scalar, z3 * scalar,
			w0 * scalar, w1 * scalar, w2 * scalar, w3 * scalar
		);
	}

	// Составные операторы присваивания
	Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& other) {
		*this = *this + other;
		return *this;
	}

	Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& other) {
		*this = *this - other;
		return *this;
	}

	Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other) {
		*this = *this * other;
		return *this;
	}

	Matrix4x4& Matrix4x4::operator*=(float scalar) {
		*this = *this * scalar;
		return *this;
	}

	// Умножение матрицы на вектор
	Vector4 Matrix4x4::operator*(const Vector4& v) const {
		return Vector4(
			x0 * v.x + x1 * v.y + x2 * v.z + x3 * v.w,
			y0 * v.x + y1 * v.y + y2 * v.z + y3 * v.w,
			z0 * v.x + z1 * v.y + z2 * v.z + z3 * v.w,
			w0 * v.x + w1 * v.y + w2 * v.z + w3 * v.w
		);
	}
}