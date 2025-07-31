#pragma once
#include <Core/GL.hpp>
#include <Components\Basic\AABB.hpp>
#include <Services\Debug\DebugDraw.hpp>
#include <Resources\GLShader.hpp>

class OGLDebugDraw : public DebugDraw 
{
public:
	OGLDebugDraw();
	~OGLDebugDraw();
	virtual void Render() override;
	virtual void Init() override;  // Опционально
	virtual void Shutdown() override;

	void SetCameraData(Math::Matrix4x4 projection, Math::Matrix4x4 view, Math::Vector3 pos_of_view);

	void DrawLine(Math::Vector3 start, Math::Vector3 end, Math::Vector4 color);
	void DrawGrid(float cellSize, int cellCount, Math::Vector4 color);	//Рисует стандартную сетку
	
	void DrawAABB(const AABB& aabb, Math::Vector4 color);


protected:
	GLShader* lineShader = nullptr;
	GLuint vao, vbo;
	//Camera
	Math::Matrix4x4 projection;
	Math::Matrix4x4 view;
	Math::Vector3 pos_of_view;
	
};