#include <Systems\Graphics\Windows\Window.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>
#include <Core\ServiceLocator.hpp>
#include <Systems\Graphics\Windows\GLFWContextManager.hpp>
#include <Systems/Input/InputSystem.hpp>

namespace Windows 
{
	WindowID Window::m_nextID = 0;

	Window::Window() 
	{
		m_resolution = Math::Vector2(standart_width, standart_height);

	}

	bool Window::Init()
	{
		if (m_isValid || m_closed) return false;

		if (m_graphics_device == nullptr) 
		{
			Diagnostics::Logger::Get().SendMessage("(Window) Invalid graphics device.", Diagnostics::MessageType::Error);
			return false;
		}

		if (m_resolution.x <= 0 || m_resolution.y <= 0)
		{
			Diagnostics::Logger::Get().SendMessage("(Window) Window hasnt created because width or height equals negative or null.", Diagnostics::MessageType::Error);
			return false;
		}
		if (!GLFWContextManager::Initialize()) {
			Diagnostics::Logger::Get().SendMessage("(Window) GLFW initialization failed.", Diagnostics::MessageType::Error);
			return false;
		}
		if (m_graphics_device->GetAPIType() != Graphics::GraphicsAPI::None)GLFWContextManager::ApplyHints(m_graphics_device->GetAPIType());
		else 
		{
			Diagnostics::Logger::Get().SendMessage("(Window) Invalid API (None) of graphics device.", Diagnostics::MessageType::Error);
			return false;
		}
		
		m_window = glfwCreateWindow(m_resolution.x, m_resolution.y, m_title.c_str(), NULL, NULL);

		if (!m_window)
		{
			Diagnostics::Logger::Get().SendMessage("(Window): Window hasnt created.", Diagnostics::MessageType::Error);
			return false;
		}

		m_id = m_nextID++;

		m_prevresolution = m_resolution;

		//Attach window class to glfw window
		glfwSetWindowUserPointer(m_window, this);


		//Callback

		SetCallbacks();

		m_isValid = true;
		return true;
	}

	void Window::SetCallbacks()
	{
		glfwSetWindowFocusCallback(m_window, [](GLFWwindow* glfwWin, int focused) {
			auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWin));

			if (window) window->OnFocusChanged(focused);
			});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mode)
			{
				Input::InputSystem* Input_ = Core::ServiceLocator::Get<Input::InputSystem>();

				if (Input_)Input_->KeyCallback(window, key, scancode, action, mode);

			});
		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
			{
				Input::InputSystem* Input_ = Core::ServiceLocator::Get<Input::InputSystem>();


				if (Input_)Input_->MouseMovingCallback(window, xpos, ypos);

			});
		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mode)
			{
				Input::InputSystem* Input_ = Core::ServiceLocator::Get<Input::InputSystem>();


				if (Input_)Input_->MouseButtonCallback(window, button, action, mode);
			});

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
			{
				auto w = glfwGetWindowUserPointer(window);
				//if (width == 0 || height == 0) w->collapsed = true;
				//else
				//{
				//	w->collapsed = false;
				//	w->SetResolution(Math::Vector2(width, height));
				//}
			}
		);
		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
			{
				//WindowEvent we(static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window))->GetID(), WindowEvent::WindowEventType::CLOSE);
				//ServiceLocator::Get<EventManager>()->Emit<WindowEvent>(we);
			});
	}

	std::shared_ptr<Graphics::IGraphicsDevice> Window::GetGraphicsDevice() const
	{
		return m_graphics_device;
	}

	void Window::SetGraphicsDevice(std::shared_ptr<Graphics::IGraphicsDevice> device)
	{
		// 1. Destroy old window (if existed)
		// 2. Apply hints for new API
		// 3. Recreate window with new settings
		// 4. Init new graphics device (after it's resource clearing)

		if (m_graphics_device == nullptr) 
		{
			///if (!device->IsValid()) device->ClearState();
			if (!device->IsValid())device->Init();

			m_graphics_device = device;
		}

		//TODO end this method
	}

	void Window::Close() 
	{
		if (m_closed) return;
		//Resource's clearing

		//Call ResourcesManager to resource clear of this window


		glfwDestroyWindow(m_window);
		m_isValid = false;
		m_closed = true;
	}

	void Window::SwapFrameBuffers() 
	{
		if (m_isValid) glfwSwapBuffers(m_window);
	}

	void Window::MakeCurrent()
	{
		glfwMakeContextCurrent(m_window);
	}

	void Window::SetWindowState(WindowState new_state)
	{
		switch (new_state)
		{
		case Windows::WindowState::Windowed:
			break;
		case Windows::WindowState::Fullscreen:
			break;
		case Windows::WindowState::Collapsed:
			break;
		default:
			break;
		}
	}

	void Window::SetFocusCallback(FocusCallback callback)
	{
		m_focusCallback = callback;
	}

	void Window::OnFocusChanged(bool focused)
	{
		if (m_focusCallback) {
			m_focusCallback(m_id, focused);
		}
	}

	Window::~Window()
	{
		if (m_isValid) Close();
	}

	void Window::SetTitle(std::string new_title)
	{
		glfwSetWindowTitle(m_window, new_title.c_str());
	}

	void Window::Update() 
	{
		glfwPollEvents();
	}

}