#include <iostream>
#include <ELMath\include\MathFunctions.hpp>
int main()
{
	Math::Vector2 vector = Math::Vector2(200,300);

	auto length = Math::Length(vector);
	std::cout << length;
}


