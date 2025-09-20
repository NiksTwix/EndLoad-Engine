#pragma once
#include "..\IGraphicsDevice.hpp"
#include <SpecialHeaders/GL.hpp>

#define GL_CHECK_ERROR() { \
    GLenum err = glGetError(); \
    while (err != GL_NO_ERROR) { \
        std::cout << "OpenGL error: " << err << std::endl; \
        err = glGetError(); \
    } \
}

namespace Graphics 
{
	class OpenGLDevice:	public IGraphicsDevice 
	{
	private:
		struct MeshBuffersData 
		{
			GLuint vao;
			std::vector<GLuint> vbos;
			GDBufferModes buffer_mode = GDBufferModes::STATIC;
		};

		std::unordered_map<MeshID, MeshBuffersData> m_meshes;
		std::unordered_map<MeshID, GLuint> m_textures;

		static bool m_glewInitialized;

	public:

		OpenGLDevice();

		void Init() override;
		void ClearState() override;
		void ClearFrameBuffer(Math::Vector4 color) override;
		MeshID CreateMesh(const MeshData& data) override;
		void BindMesh(const MeshID& id) override;
		void DeleteMesh(const MeshID& id) override;
		TextureID CreateTexture(const TextureData& data) override;
		void BindTexture(const TextureID& id) override;
		void DeleteTexture(const TextureID& id) override;
		ShaderID CreateShader(const std::string& name, const std::vector<ShaderSource>& sources) override;
		void Draw(const RenderCommand& render_command) override;
		void DrawBatch(const std::vector<RenderCommand>& render_commands) override;

		// Унаследовано через IGraphicsDevice
		void UpdateMesh(const MeshID& id, const MeshData& data) override;
	};
}