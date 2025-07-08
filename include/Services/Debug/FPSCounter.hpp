#pragma once
#include <Core/IServices.hpp>
#include <chrono>


class FPSCounter : public IHelperService 
{
public:
    FPSCounter() = default;
    ~FPSCounter() = default;
    void Update() {
        auto now = std::chrono::steady_clock::now();
        auto delta = now - lastTime;
        lastTime = now;

        frameTime = std::chrono::duration<float>(delta).count();
        fps = 1.0f / frameTime;

        smoothFPS = smoothFPS * 0.9f + fps * 0.1f; // —глаживание
    }

    float GetFPS() const { return smoothFPS; }
    float GetMSFrameTime() const { return frameTime * 1000; } // в мс
    float GetFrameTime() const { return frameTime; }
private:
    std::chrono::time_point<std::chrono::steady_clock> lastTime;
    float fps = 0.0f;
    float smoothFPS = 60.0f;
    float frameTime = 0.0f;
};