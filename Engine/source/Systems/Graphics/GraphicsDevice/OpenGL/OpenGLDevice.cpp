#include <Systems\Graphics\GraphicsDevice\OpenGL\OpenGLDevice.hpp>


namespace Graphics 
{

	bool OpenGLDevice::m_glewInitialized = false;

	OpenGLDevice::OpenGLDevice()
	{
		api = GraphicsAPI::OpenGL;
	}

	void OpenGLDevice::Init()
	{
	}
	void OpenGLDevice::ClearState()
	{
	}
	void OpenGLDevice::ClearFrameBuffer(Math::Vector4 color)
	{
	}
	MeshID OpenGLDevice::CreateMesh(const MeshData& data)
	{
		return MeshID();
	}
	void OpenGLDevice::BindMesh(const MeshID& id)
	{
	}
	void OpenGLDevice::DeleteMesh(const MeshID& id)
	{
	}
	TextureID OpenGLDevice::CreateTexture(const TextureData& data)
	{
		return TextureID();
	}
	void OpenGLDevice::BindTexture(const TextureID& id)
	{
	}
	void OpenGLDevice::DeleteTexture(const TextureID& id)
	{
	}
	ShaderID OpenGLDevice::CreateShader(const std::string& name, const std::vector<ShaderSource>& sources)
	{
		return ShaderID();
	}
	void OpenGLDevice::Draw(const RenderCommand& render_command)
	{
	}
	void OpenGLDevice::DrawBatch(const std::vector<RenderCommand>& render_commands)
	{
	}
	void OpenGLDevice::UpdateMesh(const MeshID& id, const MeshData& data)
	{
	}
}