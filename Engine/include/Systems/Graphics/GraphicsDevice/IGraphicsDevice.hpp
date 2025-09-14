#pragma once
#include "GraphicsData.hpp"



namespace Graphics 
{
	

	class Viewport;	//He is linked with ECS::CameraComponent, for clearly structure i use premature declaration

	class IGraphicsDevice 
	{
	protected:
		bool m_IsValid = false;
		std::string m_Name = "GraphicsDevice";

		Viewport* m_CurrentViewport;		//Current render viewport. Pointer is observer

		std::unordered_map<GDSettings, GDSettingsValues> m_Settings;		//Settings of render
		std::unordered_map<ShaderID, ShaderPtr> m_Shaders;
	public:

		~IGraphicsDevice() = default;

		virtual void Init() = 0;

		bool IsValid() const { return m_IsValid; }
		const std::string& GetName() const { return m_Name; }

		//State

		virtual void ClearState() = 0; // Clear all data
		virtual void ClearFrameBuffer(Math::Vector4 color = Math::Vector4(0)) = 0; //Clear and fill frame buffer
		
		virtual void Set(GDSettings setting_type, GDSettingsValues setting_value) { m_Settings[setting_type] = setting_value; }		//Is virtual because context can handle invalid values
		virtual GDSettingsValues Get(GDSettings setting_type) {
			auto it = m_Settings.find(setting_type);
			return (it != m_Settings.end()) ? it->second : GDSettingsValues::NONE;
		}	//If not exists returns NONE
		//Mesh working

		virtual MeshID CreateMesh(const MeshData& data) = 0;
		virtual void BindMesh(const MeshID& id) = 0;
		virtual void UpdateMesh(const MeshID& id, const MeshData& data) = 0;
		virtual void DeleteMesh(const MeshID& id) = 0;

		//Texture working

		virtual TextureID CreateTexture(const TextureData& data) = 0;
		virtual void BindTexture(const TextureID& id) = 0;
		virtual void DeleteTexture(const TextureID& id) = 0;

		//Shader working

		virtual ShaderID CreateShader(const std::string& name, const std::vector<ShaderSource>& sources) = 0;
		virtual ShaderPtr GetShader(ShaderID id) const {
			auto it = m_Shaders.find(id);
			return (it != m_Shaders.end()) ? it->second : nullptr;
		}
		virtual bool DeleteShader(const ShaderID& id) {
			return m_Shaders.erase(id) > 0;		//shader clear his resources automaticaly (shader_ptr is very good thing)
		}
		virtual void BindShader(const ShaderID& id) 
		{
			auto shader = GetShader(id);
			if (shader != nullptr) {
				shader->Bind();
			}
		};
		//Viewport working

		void SetRenderViewport(Viewport* viewport) { m_CurrentViewport = viewport; }
		Viewport* GetRenderViewport() const { return m_CurrentViewport; }

		//Draw

		virtual void Draw(const RenderCommand& render_command) = 0;		//Single draw
		virtual void DrawBatch(const std::vector<RenderCommand>& render_commands) = 0;		//Batch draw with sorting
	};
}

