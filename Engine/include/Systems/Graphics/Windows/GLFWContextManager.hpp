#pragma once
#include <Systems\Graphics\GraphicsDevice\GraphicsData.hpp>
#include <SpecialHeaders\GL.hpp>

namespace Windows {
    class GLFWContextManager {
    private:
        static bool& s_initialized() 
        {
            static bool s_initialized;
            return s_initialized;
        }

    public:
        static bool Initialize() {
            if (s_initialized()) return true;
            if (!glfwInit()) return false;
            s_initialized() = true;
            return true;
        }

        static void Shutdown() {
            if (s_initialized()) {
                glfwTerminate();
                s_initialized() = false;
            }
        }

        static void ApplyHints(Graphics::GraphicsAPI api) {
            switch (api) {
            case Graphics::GraphicsAPI::OpenGL:
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                break;
            case Graphics::GraphicsAPI::Vulkan:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
                // ...
            }
        }
        static bool IsInitialized() { return s_initialized(); }
    };
}