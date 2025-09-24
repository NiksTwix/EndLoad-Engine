#pragma once
#include <Core\IServices.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <ELMath\include\Vector2.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>

namespace UserInput
{
    struct InputFrame {
        std::unordered_set<int> pressedKeys;
        std::unordered_set<int> justPressedKeys;
        std::unordered_set<int> justReleasedKeys;

        std::unordered_set<int> pressedMouseButtons;
        std::unordered_set<int> justPressedMouseButtons;
        std::unordered_set<int> justReleasedMouseButtons;
        std::unordered_set<int> lastFrameKeys;

        Math::Vector2 lastMousePosition;
        Math::Vector2 currentMousePosition;
        Math::Vector2 mouseDelta;

        double scrollX, scrollY;

        InputFrame() : scrollX(0), scrollY(0) {}

        void UpdateFrame() {
            // Сохраняем состояние предыдущего кадра
            lastFrameKeys = pressedKeys;

            // Очищаем временные состояния для нового кадра
            justPressedKeys.clear();
            justReleasedKeys.clear();
            justPressedMouseButtons.clear();
            justReleasedMouseButtons.clear();

            // Обновляем дельту мыши
            mouseDelta = currentMousePosition - lastMousePosition;
            lastMousePosition = currentMousePosition;

            // Сбрасываем скролл
            scrollX = 0;
            scrollY = 0;
        }
    };

    class InputSystem : public Core::ISystem
    {
    public:
        InputSystem();
        ~InputSystem();
        virtual void Init() override;
        virtual void Shutdown() override;
        virtual void Update() override;

        // Методы для активного окна (удобства ради)
        bool IsActionPressed(const std::string& key);
        bool IsMouseActionPressed(const std::string& key);
        Math::Vector2 GetMouseDelta();
        Math::Vector2 GetMousePosition();
        bool IsShiftPressed();
        bool IsControlPressed();
        bool IsAltPressed();
        bool IsComboPressed(const std::vector<std::string>& keys);
        bool IsActionJustPressed(const std::string& key);
        bool IsActionJustReleased(const std::string& key);
        bool IsMouseActionJustPressed(const std::string& key);
        bool IsMouseActionJustReleased(const std::string& key);

        // Методы для конкретного окна
        bool IsActionPressed(Windows::WindowID windowId, const std::string& key);
        bool IsMouseActionPressed(Windows::WindowID windowId, const std::string& key);
        Math::Vector2 GetMouseDelta(Windows::WindowID windowId);
        Math::Vector2 GetMousePosition(Windows::WindowID windowId);
        bool IsActionJustPressed(Windows::WindowID windowId, const std::string& key);
        bool IsActionJustReleased(Windows::WindowID windowId, const std::string& key);
        bool IsMouseActionJustPressed(Windows::WindowID windowId, const std::string& key);
        bool IsMouseActionJustReleased(Windows::WindowID windowId, const std::string& key);

        // GLFW callbacks
        void MouseMovingCallback(GLFWwindow* window, double xpos, double ypos);
        void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
        void MouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
        void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

        virtual Core::SystemPriority GetPriority() override { return Core::SystemPriority::INPUT; }



    private:
        InputFrame& GetOrCreateFrame(Windows::WindowID windowId);
        Windows::WindowID GetWindowIdFromGLFW(GLFWwindow* window);

        std::unordered_map<Windows::WindowID, InputFrame> m_inputFrames;
        Windows::WindowsManager* m_windowManager;
        std::unordered_map<std::string, int> m_keyMapping;
        Windows::WindowID m_currentFocusedWindow;
    };
}
