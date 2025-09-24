#pragma once
#include <Core\IServices.hpp>
#include <Systems\Graphics\Viewports\Viewport.hpp>

namespace Scenes 
{
	using SceneID = Definitions::identificator;
	class SceneContext;
}
namespace Windows
{
	using WindowID = Definitions::identificator;
	class Window;
}

namespace Rendering 
{
	class RenderSystem : public Core::ISystem
	{
	public:
		RenderSystem();
		~RenderSystem();

		void Init() override;
		void Shutdown() override;
		Core::SystemPriority GetPriority() override { return Core::SystemPriority::RENDER; }
		void Update() override;

		void AttachSceneToWindow(Windows::WindowID window, Scenes::SceneID scene);
		void DetachSceneFromWindow(Windows::WindowID window);
		Scenes::SceneID GetAttachedScene(Windows::WindowID window);
	private:
		bool m_firstFrame = true;
		bool m_scenes_changed = false;

		std::unordered_map<Windows::WindowID, Scenes::SceneID> m_scenes;

		void FirstFrameInitialization();
		void RenderScene(Viewports::Viewport* viewport, Scenes::SceneContext* scene);
	};
}