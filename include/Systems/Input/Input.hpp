#pragma once
#include <Core/ISystem.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <Math\Vector2.hpp>
#include <Systems/Render/Contexts/OpenGLContext.hpp>

using WindowID = int;

class WindowManager;


class Input : public ISystem
{
public:
	Input();
	~Input();
	virtual void Init() override;
	virtual void Shutdown() override;

	virtual void Update() override;

	bool IsActionPressed(std::string key);
	bool IsMouseActionPressed(std::string key);

	Math::Vector2 GetMouseDelta();

	void MouseMovingCallback(GLFWwindow* window, double xpos, double ypos);//For glfw
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);//For glfw

	virtual SystemPriority GetPriority() override { return SystemPriority::INPUT; }

	bool IsShiftPressed();
	bool IsControlPressed();
	bool IsAltPressed();

	bool IsComboPressed(const std::vector<std::string>& keys);

private:
	std::unordered_map<std::string, int> GLFWKEYMAP;
	std::unordered_set<int> m_pressedKeys;  // Нажатые клавиши (коды GLFW)
	Math::Vector2 current_mouse_pos;
	Math::Vector2 last_mouse_pos;

	WindowManager* m_window_manager;
};