#include <Systems/Input/Input.hpp>
#include <Systems/Render/Contexts/OpenGLContext.hpp>
#include <Core/ServiceLocator.hpp>
#include <Core/Logger.hpp>
#include <Systems/Window/WindowManager.hpp>


Input::Input()
{
    Init();
}

Input::~Input()
{
}

void Input::Init()
{
    if (m_isValid) return;
    GLFWKEYMAP = {
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
    m_window_manager = ServiceLocator::Get<WindowManager>();
    if (!m_window_manager) 
    {
        ServiceLocator::Get<Logger>()->Log("(Input): window manager is invalid, initialization failed.", Logger::Level::Error);
        m_isValid = false;
        return;
    }
    m_isValid = true;
}

void Input::Shutdown()
{
    m_isValid = false;
}

void Input::Update()
{
    if (!m_isValid) return;
    if (current_mouse_pos != last_mouse_pos) last_mouse_pos = current_mouse_pos;
  
}

bool Input::IsActionPressed(std::string key)
{
    if (!m_isValid) return false;
    auto w = m_window_manager->GetFocusedWindow();
    if (!w || !GLFWKEYMAP.count(key) || w != ServiceLocator::Get<WindowManager>()->GetRenderWindow()) return false;
    GLint state = glfwGetKey(static_cast<GLFWwindow*>(w->GetNativeHandle()), GLFWKEYMAP[key]);
    if (state == GLFW_PRESS) 
        return true;
    
    return false;
}

bool Input::IsMouseActionPressed(std::string key)
{
    if (!m_isValid) return false;
    auto w = m_window_manager->GetFocusedWindow();
    if (!w || !GLFWKEYMAP.count(key) || w != ServiceLocator::Get<WindowManager>()->GetRenderWindow()) return false;
    GLint state = glfwGetMouseButton(static_cast<GLFWwindow*>(w->GetNativeHandle()), GLFWKEYMAP[key]);
    if (state == GLFW_PRESS)return true;
    return false;
}

Math::Vector2 Input::GetMouseDelta()
{
    auto w = m_window_manager->GetFocusedWindow();
    auto w1 = m_window_manager->GetRenderWindow();
    if (w == nullptr) return 0;
    if (w != w1) return 0;
    return current_mouse_pos - last_mouse_pos;
}

void Input::MouseMovingCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto w = m_window_manager->GetFocusedWindow();
    if (!w || w->GetNativeHandle() != window) return;

    last_mouse_pos = current_mouse_pos;

    current_mouse_pos.x = xpos;
    current_mouse_pos.y = ypos;
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    auto w = m_window_manager->GetFocusedWindow();
    if (!w || w->GetNativeHandle() != window) return;

    if (action == GLFW_PRESS) {
        m_pressedKeys.insert(key);
    }
    else if (action == GLFW_RELEASE) {
        m_pressedKeys.erase(key);
    }
}

bool Input::IsShiftPressed()
{
    return IsActionPressed("LEFT_SHIFT") || IsActionPressed("RIGHT_SHIFT");
}

bool Input::IsControlPressed()
{
    return IsActionPressed("LEFT_CONTROL") || IsActionPressed("RIGHT_CONTROL");
}

bool Input::IsAltPressed()
{
    return IsActionPressed("LEFT_ALT") || IsActionPressed("RIGHT_ALT");
}

bool Input::IsComboPressed(const std::vector<std::string>& keys)
{
    auto w = m_window_manager->GetFocusedWindow();
    for (const auto& keyName : keys) {
        auto it = GLFWKEYMAP.find(keyName);
        if (it == GLFWKEYMAP.end() || m_pressedKeys.find(it->second) == m_pressedKeys.end()) {
            return false;
        }
    }
    return true;
}
