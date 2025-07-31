#pragma once
#include <string>
#include <memory>
#include <Math\MathFunctions.hpp>

using WindowID = int;

class SceneContext;


class Window 
{
public:
	Window();
	virtual ~Window() = default;
	virtual void SwapBuffers() const = 0;
	virtual void UpdateState() = 0; //Poll Events
	virtual void Close() const = 0;
	virtual void* GetNativeHandle() const = 0; //Returns glfw window and etc

	WindowID GetID() { return CurrentID; };

	virtual void MakeCurrent() = 0;

	void SetScene(SceneContext* scene) { m_currentScene = scene; }
	SceneContext* GetScene() { return m_currentScene; }

	int GetWidth() { return width; };
	int GetHeight() { return height; };

	virtual void SetResolution(Math::Vector2 new_resolution) { width = new_resolution.x, height = new_resolution.y; };

private:
	static WindowID globalID;
	WindowID CurrentID = -1;
protected:
	int width = 0;
	int height = 0;
	std::string name;

	SceneContext* m_currentScene;
};
