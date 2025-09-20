#pragma once
#include <Core\IServices.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <ELMath\include\Vector2.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>

namespace UserInput
{
	

	class InputSystem: public Core::ISystem
	{
	public:
		InputSystem();
		~InputSystem();
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void Update() override;

		bool IsActionPressed(std::string key);
		bool IsMouseActionPressed(std::string key);

		Math::Vector2 GetMouseDelta();

		Math::Vector2 GetMousePosition();

		void MouseMovingCallback(GLFWwindow* window, double xpos, double ypos);//For glfw
		void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);//For glfw
		void MouseButtonCallback(GLFWwindow* window, int button, int action, int mode);

		bool IsShiftPressed();
		bool IsControlPressed();
		bool IsAltPressed();

		bool IsComboPressed(const std::vector<std::string>& keys);

		bool IsActionJustPressed(std::string key);
		bool IsActionJustReleased(std::string key);
		bool IsMouseActionJustPressed(std::string key);
		bool IsMouseActionJustReleased(std::string key);
		virtual Core::SystemPriority GetPriority() override { return Core::SystemPriority::INPUT; }
	private:
		std::unordered_map<std::string, int> GLFWKEYMAP;
		std::unordered_set<int> m_pressedKeys;  // Нажатые клавиши (коды GLFW)
		std::unordered_set<int> m_justPressedKeys;  // Клавиши, нажатые в текущем кадре
		std::unordered_set<int> m_justReleasedKeys; // Клавиши, отпущенные в текущем кадре

		std::unordered_set<int> m_pressedMouseButtons;
		std::unordered_set<int> m_justPressedMouseButtons;
		std::unordered_set<int> m_justReleasedMouseButtons;
		std::unordered_set<int> m_lastFrameKeys;    // Состояние клавиш на предыдущем кадре

		Windows::WindowsManager* m_window_manager;

		Windows::WindowID currentWindow;

		std::unordered_map<Windows::WindowID, Math::Vector2> m_lastPositions;	//Last position of cursor for every window
		Math::Vector2 current_mouse_pos;
	};
}
