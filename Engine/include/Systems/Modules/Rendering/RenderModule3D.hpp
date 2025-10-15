#pragma once
#include <Core\IServices.hpp>
#include <Services/Scenes/SceneManager.hpp>
#include <Systems\Graphics\GraphicsDevice\GraphicsData.hpp>
namespace Windows 
{
	class WindowsManager;
}

namespace Components 
{
	class MaterialComponentService;
	class TransformComponentService;
	class MeshComponentService;
}

namespace Rendering 
{
	class RenderModule3D : public Core::ISystemModule	//Meshes
	{
	public:
		void Update() override;
	private:
		void BuildCommands(); //std::vector<Graphics::RenderCommand>
		void UpdateState();
		void RenderCommands();
		void SetUpContext();
		Scenes::SceneContext* m_currentScene;
		Windows::WindowsManager* m_windowManager; //Updates once every frame
		std::vector<Graphics::RenderCommand> m_currentRenderCommands;

		Components::MeshComponentService* m_meshService;
		Components::MaterialComponentService* m_materialService;
		Components::TransformComponentService* m_transformService;
		Components::CameraComponentService* m_cameraService;
	};
}