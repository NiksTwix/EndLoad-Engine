#pragma once
#include <memory>
#include <Systems\Render\Viewport.hpp>
#include <Math\MathFunctions.hpp>


using SceneID = size_t;
class Window;
struct Camera;

enum class RCSettings 
{
	DEPTH_TEST,
	BLEND,
	BLEND_MODE,
	POLYGON_MODE
};
enum class RCSettingsValues 
{
	TRUE,
	FALSE,

	//BLEND_MODE
	SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA,
	//POLYGON_MODE
	POINTS,
	LINES,
	FILL
};



class IRenderContext 
{
public:
	virtual ~IRenderContext() = default;
	virtual void Init() = 0;
	virtual void Update(Viewport* viewport,SceneID scene) = 0;
	virtual void Shutdown() = 0;
	bool IsValid() { return is_valid; }

	

	//Render, замена методу Update

	//virtual void SetRenderScene(Viewport* viewport, SceneID scene) = 0;


	//Новые функции
	virtual void ClearBuffers() = 0;
	virtual void SetViewport(Viewport* viewport) = 0;
	virtual Viewport* GetViewport() = 0;
	virtual unsigned int SetupMesh(std::vector<Math::Vertex> vertices, std::vector<unsigned int> indices) = 0;
	virtual void DestroyMesh(unsigned int vao) = 0;
	virtual void DrawElements(unsigned int vao, int index_count) = 0;
 
	virtual void BindTexture(unsigned int tblock, unsigned int value) = 0;

	virtual void ResetBuffers() = 0;			//Микроочистка

	virtual void Set(RCSettings set, RCSettingsValues value) = 0;


protected:
	bool first_frame = false;

	bool is_valid = false;
};