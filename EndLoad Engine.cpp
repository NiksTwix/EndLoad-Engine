#include <iostream>
#include <Core\EngineCore.hpp>
#include <Core\ServiceLocator.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Systems\Graphics\GraphicsDevice\OpenGL\OpenGLDevice.hpp>
#include <ELMath\include\MathFunctions.hpp>
int main()
{
	Math::Vector2 vector = Math::Vector2(200,300);

	//auto length = Math::Length(vector);
	//std::cout << length;
	//
	//Core::EngineCore& core = Core::EngineCore::Get();
	//
	//auto window = Core::ServiceLocator::Get<Windows::WindowsManager>()->CreateWindow<Graphics::OpenGLDevice>(Math::Vector2(200),"title");
	//auto window1 = Core::ServiceLocator::Get<Windows::WindowsManager>()->CreateWindow<Graphics::OpenGLDevice>(Math::Vector2(200), "title1");
	//core.Start();

	Math::Matrix4x4 matrix = Math::Matrix4x4(100,456,1,9.234,
		100, 1, 2345, 23,
		6, 1000, 1, 100,
		100, 2, 5, 100);

	auto m = Math::Inverse(matrix);

	auto m1 = m * matrix;

	std::cout << m1.ToString() << "\n";
}


