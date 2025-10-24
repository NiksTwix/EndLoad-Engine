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
#include <Resources/ShaderResource.hpp>
#include <Resources/MaterialResource.hpp>
#include <Components/Components.hpp>

int main()
{
	Math::Vector2 vector = Math::Vector2(200,300);
	
	auto length = Math::Length(vector);
	std::cout << length;
	
	Core::EngineCore& core = Core::EngineCore::Get();
	
	for (int i = 0; i < 6; i++) 
	{
		auto window = Core::ServiceLocator::Get<Windows::WindowsManager>()->CreateWindow<Graphics::OpenGLDevice>(Math::Vector2(500), "title");

		auto scene = Core::ServiceLocator::Get<Scenes::SceneManager>()->CreateContext("Scene1");
		auto mesh = std::make_shared<Resources::MeshResource>();
		mesh->Load("D:\\EndLoadEngine\\EndLoad Engine\\TestAssets\\mesh.dcl");
		auto shader = std::make_shared<Resources::ShaderResource>();
		shader->Load("D:\\EndLoadEngine\\EndLoad Engine\\TestAssets\\meshshader.dcl");
		auto material = std::make_shared<Resources::MaterialResource>();
		material->Load("D:\\EndLoadEngine\\EndLoad Engine\\TestAssets\\meshmaterial.dcl");
		scene->AttachStaticResource(mesh);
		scene->AttachStaticResource(shader);
		scene->AttachStaticResource(material);
		auto Viewport = scene->GetViewportService().Create(Math::Vector2(500));
		Viewport->SetBackgroundColor(Math::Vector4(1.0, 0.8, 0.8, 1.0f));

		auto entity1 = scene->GetEntitySpace().CreateEntity();
		auto camera = scene->GetEntitySpace().GetServiceLocator().GetByService<Components::CameraComponentService>()->CreateCamera(500, 500, Components::ProjectionType::Perspective);
		Components::LocalTransformComponent ltc;
		scene->GetEntitySpace().AddComponent<Components::CameraComponent>(entity1, camera);
		scene->GetEntitySpace().AddComponent<Components::LocalTransformComponent>(entity1, ltc);

		auto entity2 = scene->GetEntitySpace().CreateEntity();
		Core::ServiceLocator::Get<Rendering::RenderSystem>()->AttachSceneToWindow(window->GetID(), scene->GetID());
		Components::MeshComponent meshc;
		scene->GetEntitySpace().GetServiceLocator().GetByService<Components::MeshComponentService>()->SetResourceData(meshc, mesh->GetID()); //Only after scene attachment
		scene->GetEntitySpace().GetServiceLocator().GetByService<Components::MeshComponentService>()->BuildAABB(meshc);
		Components::MaterialComponent mat;
		scene->GetEntitySpace().GetServiceLocator().GetByService<Components::MaterialComponentService>()->SetResourceShaderData(mat, shader->GetID());	//Only after scene attachment
		scene->GetEntitySpace().GetServiceLocator().GetByService<Components::MaterialComponentService>()->SetResourceMaterialData(mat, material->GetID());	//Only after scene attachment
		scene->GetEntitySpace().AddComponent<Components::MaterialComponent>(entity2, mat);
		scene->GetEntitySpace().AddComponent<Components::MeshComponent>(entity2, meshc);
		ltc.origin.z = -10;

		scene->GetEntitySpace().AddComponent<Components::LocalTransformComponent>(entity2, ltc);

		Viewport->SetCamera(scene->GetEntitySpace().GetRegistry(), entity1);

		scene->GetEntitySpace().AddEntity(entity1);
		scene->GetEntitySpace().AddEntity(entity2);

		window->SetTitle(scene->GetName());
	}
	

	core.Start();


	int i = 0;
	Core::ServiceLocator::Get<Rendering::RenderSystem>()->DetachSceneFromWindow(1);	//Resource must be free after work. WM offen does it
	//Detaching is very important. Without him resource free error (in reosource Unnit method)


	//auto mesh = Importing::AssimpLoader::Get().LoadSingleMesh("D:\\EndLoadEngine\\EndLoad Engine\\TestAssets\\Untitled.obj");
	//auto str = Importing::AssimpELDCLConverter::Get().MIRtoELDCLText(mesh,true);
	//std::cout << str.first;
}


