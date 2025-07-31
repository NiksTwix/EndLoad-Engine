#pragma once

#include <Core/GL.hpp>
#include <Systems/Window/Window.hpp>


class GLFWWindow : public Window
{
public:
	GLFWWindow();
	GLFWWindow(int width,int height,std::string window_name);
	~GLFWWindow();

	virtual void SwapBuffers() const override;
	virtual void UpdateState() override;
	virtual void* GetNativeHandle() const override;
	virtual void Close() const override;

	virtual void MakeCurrent() override;

	void SetResolution(Math::Vector2 new_resolution) override;

private:
	GLFWwindow* m_window;
	bool Init(int width, int height, std::string window_name);
	
}; 