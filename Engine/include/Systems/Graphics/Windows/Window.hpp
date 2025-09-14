#pragma once
#include "..\GraphicsDevice\IGraphicsDevice.hpp"



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
	constexpr int standart_witdh = 800;
	constexpr int standart_height = 800;

	class Window 
	{
	protected:
		WindowID m_id = Definitions::InvalidID;

		static Definitions::identificator m_nextID;

		std::string m_title = "EL Window";

		Math::Vector2 m_resolution;
		Math::Vector2 m_prevresolution;		//Previous resolution for viewport's scaling
		Math::Vector2 m_position;

		std::shared_ptr<Graphics::IGraphicsDevice> m_graphics_device;

		WindowState m_window_state = WindowState::Windowed;

		void Init();	//Context's initialization, device's creating

		const void* GetNativeHandle() const; // For glfw events 

	public:
		Window();
		~Window();

		void SetTitle(std::string new_title);	//Setting title with glfw

		const std::string& GetTitle() const;

		void Update();	//PollEvents

		void SetWindowState(WindowState new_state);

		WindowState GetWindowState() const { return m_window_state; }

		std::shared_ptr<Graphics::IGraphicsDevice> GetGraphicsDevice() const;

		Definitions::identificator GetID() const { return m_id; }

		void Close();	//Context's shutdown (ClearState), window's closing
	};
}