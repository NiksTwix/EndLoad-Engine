#pragma once
#include <SpecialHeaders/GL.hpp>
#include "..\IGraphicsDevice.hpp"


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

			Definitions::uint vertices_count;
			Definitions::uint indices_count;
		};

		std::unordered_map<MeshID, MeshBuffersData> m_meshes;
		std::unordered_map<TextureID, GLuint> m_textures;
		std::unordered_map<ShaderID, Definitions::uint> m_shaders;
		static bool m_glewInitialized;


		Definitions::uint CompileShader(GLenum type, const std::string& source);

		GLuint GetGLBufferMode(GDBufferModes mode);
	public:

		OpenGLDevice();

		void Init() override;
		void ClearState() override;
		void ClearFrameBuffer(Math::Vector4 color) override;
		MeshID CreateMesh(const MeshData& data) override;
		void BindMesh(const MeshID& id) override;
		
		void DestroyMesh(const MeshID& id) override;
		TextureID CreateTexture(const TextureData& data) override;
		void BindTexture(Definitions::uint slot, const TextureID& id) override;
		void DestroyTexture(const TextureID& id) override;
		ShaderID CreateShader(const std::vector<ShaderSource>& sources) override;

		void SetUniform(ShaderID shader,const std::string& name, const UniformValue& value) override;

		void Draw(const RenderCommand& render_command) override;
		void DrawBatch(const std::vector<RenderCommand>& render_commands) override;

		void UpdateMesh(const MeshID& id, const MeshData& data) override;

		void SetRenderViewport(Viewports::Viewport* viewport) override;

		void Set(GDSettings setting_type, GDSettingsValues setting_value) override;

		// Унаследовано через IGraphicsDevice
		void BindShader(const ShaderID& id) override;

		// Унаследовано через IGraphicsDevice
		void DestroyShader(const ShaderID& id) override;

		// Унаследовано через IGraphicsDevice
		bool IsMeshValid(const MeshID& id) override;
		bool IsTextureValid(const TextureID& id) override;
		bool IsShaderValid(const ShaderID& id) override;
	};
}