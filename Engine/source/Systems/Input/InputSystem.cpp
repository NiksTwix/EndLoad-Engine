#include <Core\ServiceLocator.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>
#include <Systems/Input\InputSystem.hpp>

namespace Input
{
    InputSystem::InputSystem() : m_windowManager(nullptr), m_currentFocusedWindow(0)
    {
        Init();
    }

    InputSystem::~InputSystem()
    {
    }

    void InputSystem::Init()
    {
        if (m_isValid) return;

        // Инициализация маппинга клавиш (как у тебя было)
        m_keyMapping = {
            {"SPACE",GLFW_KEY_SPACE},              {"[",GLFW_KEY_LEFT_BRACKET},
            {"'",GLFW_KEY_APOSTROPHE},             {"]",GLFW_KEY_RIGHT_BRACKET},
            {",",GLFW_KEY_COMMA},                  {"\\",GLFW_KEY_BACKSLASH},
            {"-",GLFW_KEY_MINUS},                  {"ESCAPE",GLFW_KEY_ESCAPE},
            {".",GLFW_KEY_PERIOD},                 {"ENTER",GLFW_KEY_ENTER},
            {"/",GLFW_KEY_SLASH},                  {"TAB",GLFW_KEY_TAB},
            {"0",GLFW_KEY_0},                      {"BACKSPACE",GLFW_KEY_BACKSPACE},
            {"1",GLFW_KEY_1},                      {"INSERT",GLFW_KEY_INSERT},
            {"2",GLFW_KEY_2},                      {"DELETE",GLFW_KEY_DELETE},
            {"3",GLFW_KEY_3},                      {"RIGHT",GLFW_KEY_RIGHT},
            {"4",GLFW_KEY_4},                      {"LEFT",GLFW_KEY_LEFT},
            {"5",GLFW_KEY_5},                      {"UP",GLFW_KEY_UP},
            {"6",GLFW_KEY_6},                      {"DOWN",GLFW_KEY_DOWN},
            {"7",GLFW_KEY_7},                      {"PAGE_UP",GLFW_KEY_PAGE_UP},
            {"8",GLFW_KEY_8},                      {"PAGE_DOWN",GLFW_KEY_PAGE_DOWN},
            {"9",GLFW_KEY_9},                      {"HOME",GLFW_KEY_HOME},
            {";",GLFW_KEY_SEMICOLON},              {"CAPS_LOCK",GLFW_KEY_CAPS_LOCK},
            {"=",GLFW_KEY_EQUAL},                  {"SCROLL_LOCK",GLFW_KEY_SCROLL_LOCK},
            {"A",GLFW_KEY_A},                      {"NUM_LOCK",GLFW_KEY_NUM_LOCK},
            {"B",GLFW_KEY_B},                      {"PRINT_SCREEN",GLFW_KEY_PRINT_SCREEN },
            {"C",GLFW_KEY_C},                      {"PAUSE",GLFW_KEY_PAUSE},
            {"D",GLFW_KEY_D},                      {"F1",GLFW_KEY_F1},
            {"E",GLFW_KEY_E},                      {"F2",GLFW_KEY_F2},
            {"F",GLFW_KEY_F},                      {"F3",GLFW_KEY_F3},
            {"G",GLFW_KEY_G},                      {"F4",GLFW_KEY_F4},
            {"H",GLFW_KEY_H},                      {"F5",GLFW_KEY_F5},
            {"I",GLFW_KEY_I},                      {"F6",GLFW_KEY_F6},
            {"J",GLFW_KEY_J},                      {"F7",GLFW_KEY_F7},
            {"K",GLFW_KEY_K},                      {"F8",GLFW_KEY_F8},
            {"L",GLFW_KEY_L},                      {"F9",GLFW_KEY_F9},
            {"M",GLFW_KEY_M},                      {"F10",GLFW_KEY_F10},
            {"N",GLFW_KEY_N},                      {"F11",GLFW_KEY_F11},
            {"O",GLFW_KEY_O},                      {"F12",GLFW_KEY_F12},
            {"P",GLFW_KEY_P},                      {"LEFT_SHIFT",GLFW_KEY_LEFT_SHIFT},
            {"Q",GLFW_KEY_Q},                      {"RIGHT_SHIFT",GLFW_KEY_RIGHT_SHIFT},
            {"R",GLFW_KEY_R},                      {"RMB",GLFW_MOUSE_BUTTON_RIGHT},
            {"S",GLFW_KEY_S},                      {"MMB",GLFW_MOUSE_BUTTON_MIDDLE},
            {"T",GLFW_KEY_T},                      {"LMB",GLFW_MOUSE_BUTTON_LEFT},
            {"U",GLFW_KEY_U},                      {"LEFT_CONTROL",GLFW_KEY_LEFT_CONTROL},
            {"U",GLFW_KEY_U},                      {"RIGHT_CONTROL",GLFW_KEY_RIGHT_CONTROL},
            {"V",GLFW_KEY_V},                      {"LEFT_ALT",GLFW_KEY_LEFT_ALT},
            {"W",GLFW_KEY_W},                      {"RIGHT_ALT",GLFW_KEY_RIGHT_ALT},
            {"X",GLFW_KEY_X},
            {"Y",GLFW_KEY_Y},
            {"Z",GLFW_KEY_Z},
        };

        m_windowManager = Core::ServiceLocator::Get<Windows::WindowsManager>();
        if (!m_windowManager)
        {
            Diagnostics::Logger::Get().SendMessage("(InputSystem) Window manager is invalid, initialization failed.", Diagnostics::MessageType::Error);
            m_isValid = false;
            return;
        }
        m_isValid = true;
    }

    void InputSystem::Shutdown()
    {
        m_isValid = false;
        m_inputFrames.clear();
    }

    void InputSystem::Update()
    {
        if (!m_isValid) return;

        // Обновляем фокус окна
        auto* focusedWindow = m_windowManager->GetFocusedWindow();
        if (focusedWindow) {
            m_currentFocusedWindow = focusedWindow->GetID();
        }

        // Обновляем все InputFrame'ы
        for (auto& [windowId, frame] : m_inputFrames) {
            frame.UpdateFrame();
        }
    }

    // Private helpers
    InputFrame& InputSystem::GetOrCreateFrame(Windows::WindowID windowId)
    {
        return m_inputFrames[windowId];
    }

    Windows::WindowID InputSystem::GetWindowIdFromGLFW(GLFWwindow* window)
    {
        for (auto& [id, win] : m_windowManager->GetAllWindows()) {
            if (win->GetNativeHandle() == window) {
                return id;
            }
        }
        return 0;
    }

    void InputSystem::RemoveFrame(Windows::WindowID windowId) {
        auto it = m_inputFrames.find(windowId);
        if (it != m_inputFrames.end()) {
            m_inputFrames.erase(it);
            Diagnostics::Logger::Get().SendMessage(
                "(InputSystem) Removed input frame for window: " + std::to_string(windowId)
            );
        }
        // Если удаляем текущее активное окно - сбрасываем фокус
        if (m_currentFocusedWindow == windowId) {
            m_currentFocusedWindow = Definitions::InvalidID;
        }
    }

    // Методы для активного окна
    bool InputSystem::IsActionPressed(const std::string& key)
    {
        return IsActionPressed(m_currentFocusedWindow, key);
    }

    bool InputSystem::IsActionPressed(Windows::WindowID windowId, const std::string& key)
    {
        if (!m_isValid || !m_keyMapping.count(key)) return false;

        auto it = m_inputFrames.find(windowId);
        if (it == m_inputFrames.end()) return false;

        return it->second.pressedKeys.count(m_keyMapping[key]);
    }

    bool InputSystem::IsMouseActionPressed(const std::string& key)
    {
        return IsMouseActionPressed(m_currentFocusedWindow, key);
    }

    bool InputSystem::IsMouseActionPressed(Windows::WindowID windowId, const std::string& key)
    {
        if (!m_isValid || !m_keyMapping.count(key)) return false;

        auto it = m_inputFrames.find(windowId);
        if (it == m_inputFrames.end()) return false;

        return it->second.pressedMouseButtons.count(m_keyMapping[key]);
    }

    Math::Vector2 InputSystem::GetMouseDelta()
    {
        return GetMouseDelta(m_currentFocusedWindow);
    }

    Math::Vector2 InputSystem::GetMouseDelta(Windows::WindowID windowId)
    {
        auto it = m_inputFrames.find(windowId);
        if (it != m_inputFrames.end()) {
            return it->second.mouseDelta;
        }
        return Math::Vector2(0);
    }

    Math::Vector2 InputSystem::GetMousePosition()
    {
        return GetMousePosition(m_currentFocusedWindow);
    }

    Math::Vector2 InputSystem::GetMousePosition(Windows::WindowID windowId)
    {
        auto it = m_inputFrames.find(windowId);
        if (it != m_inputFrames.end()) {
            return it->second.currentMousePosition;
        }
        return Math::Vector2(0);
    }

    // GLFW Callbacks
    void InputSystem::MouseMovingCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Windows::WindowID windowId = GetWindowIdFromGLFW(window);
        if (windowId == 0) return;

        InputFrame& frame = GetOrCreateFrame(windowId);
        frame.currentMousePosition = Math::Vector2(xpos, ypos);
    }

    void InputSystem::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        Windows::WindowID windowId = GetWindowIdFromGLFW(window);
        if (windowId == 0) return;

        InputFrame& frame = GetOrCreateFrame(windowId);
        
        if (action == GLFW_PRESS) {
            frame.pressedKeys.insert(key);
            frame.justPressedKeys.insert(key);
        }
        else if (action == GLFW_RELEASE) {
            frame.pressedKeys.erase(key);
            frame.justReleasedKeys.insert(key);
        }
    }

    void InputSystem::MouseButtonCallback(GLFWwindow* window, int button, int action, int mode)
    {
        Windows::WindowID windowId = GetWindowIdFromGLFW(window);
        if (windowId == Definitions::InvalidID) return;

        InputFrame& frame = GetOrCreateFrame(windowId);

        if (action == GLFW_PRESS) {
            frame.pressedMouseButtons.insert(button);
            frame.justPressedMouseButtons.insert(button);
        }
        else if (action == GLFW_RELEASE) {
            frame.pressedMouseButtons.erase(button);
            frame.justReleasedMouseButtons.insert(button);
        }
    }

    void InputSystem::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        Windows::WindowID windowId = GetWindowIdFromGLFW(window);
        if (windowId == 0) return;

        InputFrame& frame = GetOrCreateFrame(windowId);
        frame.scrollX = xoffset;
        frame.scrollY = yoffset;
    }

    // Остальные методы (IsShiftPressed, IsActionJustPressed и т.д.) адаптируются аналогично
    bool InputSystem::IsShiftPressed()
    {
        return IsActionPressed("LEFT_SHIFT") || IsActionPressed("RIGHT_SHIFT");
    }

    bool InputSystem::IsControlPressed()
    {
        return IsActionPressed("LEFT_CONTROL") || IsActionPressed("RIGHT_CONTROL");
    }

    bool InputSystem::IsAltPressed()
    {
        return IsActionPressed("LEFT_ALT") || IsActionPressed("RIGHT_ALT");
    }

    bool InputSystem::IsComboPressed(const std::vector<std::string>& keys)
    {
        auto it = m_inputFrames.find(m_currentFocusedWindow);
        if (it == m_inputFrames.end()) return false;

        for (const auto& keyName : keys) {
            auto keyIt = m_keyMapping.find(keyName);
            if (keyIt == m_keyMapping.end() ||
                it->second.pressedKeys.find(keyIt->second) == it->second.pressedKeys.end()) {
                return false;
            }
        }
        return true;
    }

    bool InputSystem::IsActionJustPressed(const std::string& key)
    {
        return IsActionJustPressed(m_currentFocusedWindow, key);
    }

    bool InputSystem::IsActionJustPressed(Windows::WindowID windowId, const std::string& key)
    {
        if (!m_isValid || !m_keyMapping.count(key)) return false;

        auto it = m_inputFrames.find(windowId);
        if (it == m_inputFrames.end()) return false;

        return it->second.justPressedKeys.count(m_keyMapping[key]);
    }

    bool InputSystem::IsActionJustReleased(const std::string& key)
    {
        return IsActionJustReleased(m_currentFocusedWindow, key);
    }

    bool InputSystem::IsActionJustReleased(Windows::WindowID windowId, const std::string& key)
    {
        if (!m_isValid || !m_keyMapping.count(key)) return false;

        auto it = m_inputFrames.find(windowId);
        if (it == m_inputFrames.end()) return false;

        return it->second.justReleasedKeys.count(m_keyMapping[key]);
    }

    bool InputSystem::IsMouseActionJustPressed(const std::string& key)
    {
        return IsMouseActionJustPressed(m_currentFocusedWindow, key);
    }

    bool InputSystem::IsMouseActionJustPressed(Windows::WindowID windowId, const std::string& key)
    {
        if (!m_isValid || !m_keyMapping.count(key)) return false;

        auto it = m_inputFrames.find(windowId);
        if (it == m_inputFrames.end()) return false;

        return it->second.justPressedMouseButtons.count(m_keyMapping[key]);
    }

    bool InputSystem::IsMouseActionJustReleased(const std::string& key)
    {
        return IsMouseActionJustReleased(m_currentFocusedWindow, key);
    }

    bool InputSystem::IsMouseActionJustReleased(Windows::WindowID windowId, const std::string& key)
    {
        if (!m_isValid || !m_keyMapping.count(key)) return false;

        auto it = m_inputFrames.find(windowId);
        if (it == m_inputFrames.end()) return false;

        return it->second.justReleasedMouseButtons.count(m_keyMapping[key]);
    }
}