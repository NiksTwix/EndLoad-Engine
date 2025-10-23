#include <iostream>
#include <Core\EngineCore.hpp>
#include <Core\ServiceLocator.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Systems\Graphics\Rendering\RenderSystem.hpp>
#include <Services/Scenes/SceneManager.hpp>
#include <Systems\Graphics\GraphicsDevice\OpenGL\OpenGLDevice.hpp>
#include <ELMath\include\MathFunctions.hpp>
#include <Services/Importing/AssimpLoader.hpp>
#include <Resources/MeshResource.hpp>


int main()
{
	//Math::Vector2 vector = Math::Vector2(200,300);
	//
	//auto length = Math::Length(vector);
	//std::cout << length;
	//
	//Core::EngineCore& core = Core::EngineCore::Get();
	//
	//auto window = Core::ServiceLocator::Get<Windows::WindowsManager>()->CreateWindow<Graphics::OpenGLDevice>(Math::Vector2(500),"title");
	//
	//auto scene1 = Core::ServiceLocator::Get<Scenes::SceneManager>()->CreateContext("Scene1");
	//
	//Core::ServiceLocator::Get<Rendering::RenderSystem>()->AttachSceneToWindow(window->GetID(), scene1);
	//
	//window->SetTitle(Core::ServiceLocator::Get<Scenes::SceneManager>()->GetContext(scene1)->GetName());
	//
	//auto window1 = Core::ServiceLocator::Get<Windows::WindowsManager>()->CreateWindow<Graphics::OpenGLDevice>(Math::Vector2(200), "title1");
	//core.Start();

	//Math::Matrix4x4 matrix = Math::Matrix4x4(100,456,1,9.234,
	//	100, 1, 2345, 23,
	//	6, 1000, 1, 100,
	//	100, 2, 5, 100);
	//
	//auto m = Math::Inverse(matrix);
	//
	//auto m1 = m * matrix;
	//
	//std::cout << m1.ToString() << "\n";

	auto mesh = Resources::MeshResource();
	mesh.Load("C:\\Users\\korsi\\OneDrive\\Рабочий стол\\eldcl.txt");

	for (auto vertex : mesh.GetMeshData().vertices) 
	{
		std::cout << " P:" << vertex.position.ToString() << " N:" << vertex.normal.ToString() << " TC:" << vertex.texCoord.ToString() << " TA:" << vertex.tangent.ToString() << " BT:" << vertex.bitangent.ToString() << "\n";
	
	}

	//auto mesh = Importing::AssimpLoader::Get().LoadSingleMesh("D:\\EndLoadEngine\\EndLoad Engine\\TestAssets\\Untitled.obj");
	//auto str = Importing::AssimpELDCLConverter::Get().MIRtoELDCLText(mesh,true);
	//std::cout << str.first;
}


