#pragma once
#include <sstream> 
#include <vector>

namespace Math
{
	class Vector4;

	class Matrix4x4	//Обращение за границы [3][3] - неопределённое поведение
	{
	public:
		union {
			alignas(16) float values[4][4]; // Доступ по индексам [row][col]
			struct {
				float x0, x1, x2, x3; // Первая строка
				float y0, y1, y2, y3; // Вторая строка
				float z0, z1, z2, z3; // Третья строка
				float w0, w1, w2, w3; // Четвертая строка
			};
			
		};

		// Конструкторы
		Matrix4x4() :
			x0(1), x1(0), x2(0), x3(0),
			y0(0), y1(1), y2(0), y3(0),
			z0(0), z1(0), z2(1), z3(0),
			w0(0), w1(0), w2(0), w3(1) {} // Единичная матрица

		Matrix4x4(float x0, float x1, float x2, float x3,
			float y0, float y1, float y2, float y3,
			float z0, float z1, float z2, float z3,
			float w0, float w1, float w2, float w3) :
			x0(x0), x1(x1), x2(x2), x3(x3),
			y0(y0), y1(y1), y2(y2), y3(y3),
			z0(z0), z1(z1), z2(z2), z3(z3),
			w0(w0), w1(w1), w2(w2), w3(w3) {}

		Matrix4x4 operator+(const Matrix4x4& other) const;
		Matrix4x4 operator-(const Matrix4x4& other) const;
		Matrix4x4 operator*(const Matrix4x4& other) const;
		Matrix4x4 operator*(float scalar) const;

		Matrix4x4& operator+=(const Matrix4x4& other);
		Matrix4x4& operator-=(const Matrix4x4& other);
		Matrix4x4& operator*=(const Matrix4x4& other);
		Matrix4x4& operator*=(float scalar);

		Vector4 operator*(const Vector4& v) const;

		std::string ToString() const //Returns the line of values in row-major 
		{
			std::stringstream ss;
			ss << x0 << ";" << x1 << ";" << x2 << ";" << x3 << ";";
			ss << y0 << ";" << y1 << ";" << y2 << ";" << y3 << ";";
			ss << z0 << ";" << z1 << ";" << z2 << ";" << z3 << ";";
			ss << w0 << ";" << w1 << ";" << w2 << ";" << w3;
			return ss.str();							    
		}

		std::vector<float> ToArray() const
		{
			std::vector<float> result;

			result.push_back(x0);
			result.push_back(x1);
			result.push_back(x2);
			result.push_back(x3);

			result.push_back(y0);
			result.push_back(y1);
			result.push_back(y2);
			result.push_back(y3);

			result.push_back(z0);
			result.push_back(z1);
			result.push_back(z2);
			result.push_back(z3);

			result.push_back(w0);
			result.push_back(w1);
			result.push_back(w2);
			result.push_back(w3);

			return result;
		}

		static Matrix4x4 FromArray(const std::vector<float>& values)		//x_values, y_values,z_values,w_values
		{
			Matrix4x4 result;

			int x_index = 0;
			int y_index = 0;

			for (auto value : values) 
			{
				result.values[y_index][x_index] = value;
				if (y_index == 3 && x_index == 3) break;
				if (x_index == 3)
				{
					y_index++;
					x_index = 0;
				}
				
				x_index++;
			}
			return result;
		}
	};

}