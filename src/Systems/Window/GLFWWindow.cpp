#include <Systems\Window\GLFWWindow.hpp>
#include <Core/ServiceLocator.hpp>
#include <Systems/Window/WindowManager.hpp>
#include <Core/Logger.hpp>
#include <Systems/Input/Input.hpp>

GLFWWindow::GLFWWindow()
{
	Init(600, 600, "ELWindow");
}

GLFWWindow::GLFWWindow(int width, int height, std::string window_name)
{
	Init(width, height, window_name);
}

GLFWWindow::~GLFWWindow()
{
	if (m_window) {
		glfwDestroyWindow(m_window);  
		m_window = nullptr;
	}
}

void GLFWWindow::SwapBuffers() const
{
	glfwSwapBuffers(static_cast<GLFWwindow*>(GetNativeHandle()));
}

void GLFWWindow::UpdateState()
{
	glfwPollEvents();
}

void* GLFWWindow::GetNativeHandle() const
{
	return m_window;
}

void GLFWWindow::Close() const
{
	void* v = GetNativeHandle();
	if (v == nullptr) return;
	glfwDestroyWindow(static_cast<GLFWwindow*>(v));
}

void GLFWWindow::MakeCurrent()
{
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(GetNativeHandle()));
}

void GLFWWindow::SetResolution(Math::Vector2 new_resolution)
{
	width = new_resolution.x;
	height = new_resolution.y;
}

bool GLFWWindow::Init(int width, int height, std::string window_name)
{
	auto logger = ServiceLocator::Get<Logger>();
	if (!glfwInit())
	{
		logger->Log("(GLFWWindow): GLFW hasnt inited.", Logger::Level::Error);
		return false;
	}

	if (width <= 0 || height <= 0) 
	{
		logger->Log("(GLFWWindow): window hasnt created because width or height equals negative or null", Logger::Level::Error);
		return false;
	}

	//Настройка GLFW
	//Задается минимальная требуемая версия OpenGL. 
	//Мажорная 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//Минорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Установка профиля, для которого создается контекст
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Включение возможности изменения размера окна
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	//Создание окна
	m_window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);



	if (!m_window)
	{
		logger->Log("(GLFWWindow): window hasnt created.", Logger::Level::Error);

		return false;
	}

	

	this->width = width;
	this->height = height;
	this->name = window_name;

	glfwMakeContextCurrent(m_window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		// ... ошибка инициализации GLEW ...
		auto logger = ServiceLocator::Get<Logger>();
		logger->Log("(GLFWWindow): Glew hasnt inited", Logger::Level::Error);
		return false;
	}

	// Привязываем класс-обёртку к окну, чтобы позже его использовать через static_cast
	glfwSetWindowUserPointer(m_window, this);

	// Колбэк для изменения фокуса
	glfwSetWindowFocusCallback(m_window, [](GLFWwindow* glfwWin, int focused) {
		auto* window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(glfwWin));
		auto wm = ServiceLocator::Get<WindowManager>();

		if (focused) {
			wm->SetFocusedWindow(window);
		}
		else if (wm->GetFocusedWindow() == window) {
			wm->SetFocusedWindow(nullptr);
		}
		});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mode) 
		{
			Input* Input_ = ServiceLocator::Get<Input>();

			if(Input_)Input_->KeyCallback(window, key, scancode, action, mode);
		
		});
	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
		{
			Input* Input_ = ServiceLocator::Get<Input>();


			if (Input_)Input_->MouseMovingCallback(window, xpos, ypos);

		});


	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window))->SetResolution(Math::Vector2(width,height));
		}
	);

	auto wm = ServiceLocator::Get<WindowManager>();
	wm->SetFocusedWindow(this);
	glfwSetInputMode(static_cast<GLFWwindow*>(GetNativeHandle()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;

}
