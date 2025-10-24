#pragma once
#include "Window.hpp"
#include <Core\IServices.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services/ResourcesManager/ResourcesManager.hpp>




namespace Windows 
{
	class WindowsManager : public Core::ISystem 
	{
	private:
		std::unordered_map<WindowID, std::shared_ptr<Window>> m_windows;

		Window* m_focusedWindow = nullptr;
		Window* m_renderWindow = nullptr;

		// Колбэк для обновления фокуса
		void UpdateFocus(WindowID id, bool focused) {
			if (focused) {
				if (auto it = m_windows.find(id); it != m_windows.end()) {
					m_focusedWindow = it->second.get();
				}
			}
			else if (m_focusedWindow && m_focusedWindow->GetID() == id) {
				m_focusedWindow = nullptr; // Сброс фокуса
			}
		}

		std::vector<WindowID> m_closedWindows;	//Temp buffer for window closing

		void HandleClosedWindows();

		

	public:
		template<typename GraphicsDevice>
		std::shared_ptr<Window> CreateWindow(Math::Vector2 resolution, std::string title)
		{
			auto* resource_manager = Core::ServiceLocator::Get<Resources::ResourcesManager>();
			std::shared_ptr<Window> window = std::make_shared<Window>(resolution, title);

			window->SetGraphicsDevice<GraphicsDevice>();
			window->Init();

			WindowID id = window->GetID();
			m_windows[id] = window;

			window->SetFocusCallback([this](WindowID windowId, bool focused) {
				this->UpdateFocus(windowId, focused);
				});

			m_focusedWindow = window.get();
			if (resource_manager) resource_manager->SetActiveWindow(id);
			return window;
		}
		virtual Core::SystemPriority GetPriority() override { return Core::SystemPriority::WINDOW_MANAGER; }
		size_t GetWindowsCount() const { return m_windows.size(); }

		Window* GetFocusedWindow() const { return m_focusedWindow; }

		void SetRenderWindow(WindowID windowId);

		Window* GetRenderWindow() const { return m_renderWindow; }

		WindowID GetRenderWindowID() const 
		{
			return m_renderWindow ? m_renderWindow->GetID() : Definitions::InvalidID; 
		};

		Window* GetWindow(WindowID id) 
		{
			return m_windows.count(id) ? m_windows[id].get() : nullptr;
		}

		bool IsExists(WindowID id) 
		{
			return m_windows.count(id);
		}

		const std::unordered_map<WindowID, std::shared_ptr<Window>>& GetAllWindows() const {
			return m_windows;
		}

		void Update() override;
		void Shutdown() override;

		void RemoveInputFrame(WindowID window);
	};
}