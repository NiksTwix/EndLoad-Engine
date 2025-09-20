#pragma once
#include "..\GraphicsDevice\IGraphicsDevice.hpp"
#include <SpecialHeaders\GL.hpp>
#include <functional>

namespace Windows 
{
	using WindowID = Definitions::identificator;

	enum class WindowState 
	{
		Windowed,
		Fullscreen,
		Collapsed
	};

	//Standart values
	constexpr int standart_width = 800;
	constexpr int standart_height = 800;

	class Window 
	{
	protected:
		WindowID m_id = Definitions::InvalidID;

		using FocusCallback = std::function<void(WindowID, bool)>;
		FocusCallback m_focusCallback;

		static Definitions::identificator m_nextID;
		
		std::string m_title = "EL Window";

		Math::Vector2 m_resolution;
		Math::Vector2 m_prevresolution;		//Previous resolution for viewport's scaling
		Math::Vector2 m_position;

		std::shared_ptr<Graphics::IGraphicsDevice> m_graphics_device;

		bool m_isValid = false;

		bool m_closed = false;

		WindowState m_window_state = WindowState::Windowed;

		GLFWwindow* m_window;

		void SetCallbacks();

	public:
		bool Init();	//GLFW initialization
		Window();
		Window(Math::Vector2 resolution,std::string title) 
		{
			m_resolution = resolution;
			m_title = title;
		}

		void SetFocusCallback(FocusCallback callback);

		// При получении/потере фокуса вызываем колбэк
		void OnFocusChanged(bool focused);

		~Window();

		void SetTitle(std::string new_title);	//Setting title with glfw

		std::string GetTitle() const { return m_title; };

		void Update();	//PollEvents

		void SwapFrameBuffers();

		void MakeCurrent();

		void SetWindowState(WindowState new_state);

		bool IsValid() const { return m_isValid; }
		bool IsClosed() const { return m_closed; }
		WindowState GetWindowState() const { return m_window_state; }

		std::shared_ptr<Graphics::IGraphicsDevice> GetGraphicsDevice() const;

		virtual void SetGraphicsDevice(std::shared_ptr<Graphics::IGraphicsDevice> device);

		template<typename GraphicsDevice>
		void SetGraphicsDevice() 
		{
			static_assert(std::is_base_of_v<Graphics::IGraphicsDevice, GraphicsDevice>, "GraphicsDevice must inherit from IGraphicsDevice");

			std::shared_ptr<Graphics::IGraphicsDevice> device = std::make_shared<GraphicsDevice>();
			SetGraphicsDevice(device);
		}

		Definitions::identificator GetID() const { return m_id; }

		void Close();	//Context's shutdown (ClearState), window's closing

	    GLFWwindow* GetNativeHandle() const { return m_window; }; // For glfw events 
	};
}