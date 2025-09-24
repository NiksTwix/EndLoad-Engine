#include <Systems\Graphics\GraphicsDevice\OpenGL\OpenGLDevice.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>
#include <Systems\Graphics\Viewports\Viewport.hpp>

namespace Graphics 
{

	bool OpenGLDevice::m_glewInitialized = false;

	Definitions::uint OpenGLDevice::CompileShader(GLenum type, const std::string& source)
	{
		GLuint shader = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::stringstream ss;
			ss << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
				<< " shader compilation failed: " << infoLog;

			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Shader's compilation failed: " + ss.str() + ".", Diagnostics::MessageType::Error);
			return Definitions::InvalidID;
		}
		return shader;
	}

	GLuint OpenGLDevice::GetGLBufferMode(GDBufferModes mode)
	{
		switch (mode)
		{
		case Graphics::GDBufferModes::STATIC:
			return GL_STATIC_DRAW;
		case Graphics::GDBufferModes::DYNAMIC:
			return GL_DYNAMIC_DRAW;
		case Graphics::GDBufferModes::STREAM:
			return GL_STREAM_DRAW;
		default:
			return GL_STATIC_DRAW;
			break;
		}
		return GL_STATIC_DRAW;
	}

	OpenGLDevice::OpenGLDevice()
	{
		api = GraphicsAPI::OpenGL;
	}

	void OpenGLDevice::Init()
	{
		if (m_IsValid) return;
		if (!m_glewInitialized) 
		{
			if (!glewInit()) 
			{
				 
				return;
			}
		}
		m_IsValid = true;
	}
	void OpenGLDevice::ClearState()
	{
		if (!CheckValid()) return;
		//mesh clearing

		std::vector<Definitions::identificator> keys;

		for (auto& [id, mesh_data] : m_meshes) 
		{
			keys.push_back(id);
		}
		for (auto id : keys) 
		{
			DestroyMesh(id);
		}
		keys.clear();
		for (auto& [id, texture_data] : m_textures)
		{
			keys.push_back(id);
		}
		for (auto id : keys)
		{
			DestroyTexture(id);
		}
	}
	void OpenGLDevice::ClearFrameBuffer(Math::Vector4 color)
	{
		if (!CheckValid()) return;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	MeshID OpenGLDevice::CreateMesh(const MeshData& data)
	{
		if (!m_IsValid) return Definitions::InvalidID;
		GLuint VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		auto mode_ = GL_STATIC_DRAW;

		switch (data.buffer_mode)
		{
		case GDBufferModes::STATIC:
			mode_ = GL_STATIC_DRAW;
			break;
		case GDBufferModes::DYNAMIC:
			mode_ = GL_DYNAMIC_DRAW;
			break;
		case GDBufferModes::STREAM:
			mode_ = GL_STREAM_DRAW;
			break;
		}

		glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Math::Vertex), data.vertices.data(), mode_);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(GLuint), data.indices.data(), mode_);

		// Атрибуты вершин
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Math::Vertex), (void*)offsetof(Math::Vertex, position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Math::Vertex), (void*)offsetof(Math::Vertex, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Math::Vertex), (void*)offsetof(Math::Vertex, texCoord));

		glBindVertexArray(0);

		auto error = glGetError();

		if (error != 0)
		{
			std::stringstream ss;
			ss << glewGetErrorString(error);
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Creating mesh failed. OpenGL error:" + ss.str() + ".", Diagnostics::MessageType::Error);
		}

		auto key_id = GetNextID(IDType::Mesh);
		MeshBuffersData mbd;

		mbd.indices_count = data.indices.size();
		mbd.vertices_count = data.vertices.size();

		mbd.buffer_mode = data.buffer_mode;
		mbd.vao = VAO;
		mbd.vbos.push_back(VBO);
		mbd.vbos.push_back(EBO);
		m_meshes[key_id] = mbd;
		return key_id;
	}
	void OpenGLDevice::BindMesh(const MeshID& id)
	{
		if (!CheckValid()) return;
		if (!m_meshes.count(id))
		{
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Binding mesh failed: invalid mesh id.", Diagnostics::MessageType::Error);
			return;
		}

		glBindVertexArray(m_meshes[id].vao);
	}
	void OpenGLDevice::DestroyMesh(const MeshID& id)
	{
		if (!CheckValid()) return;
		if (!m_meshes.count(id)) 
		{
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Destroing mesh failed: invalid mesh id.", Diagnostics::MessageType::Error);
			return;
		}
		auto mbd = m_meshes[id];
		for (auto& b : mbd.vbos)
		{
			glDeleteBuffers(1, &b);
		}
		glDeleteVertexArrays(1, &mbd.vao);
		m_meshes.erase(id);		
	}
	TextureID OpenGLDevice::CreateTexture(const TextureData& data)
	{
		if (!CheckValid()) return Definitions::InvalidID;
		if (data.size.x <= 0 || data.size.y <= 0) {
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Creating texture failed: invalid texture size.", Diagnostics::MessageType::Error);
			return 0;
		}

		GLuint id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		GLenum format;
		switch (data.bytes_per_pixel) {
		case 1: format = GL_RED; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default:
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Creating texture failed: invalid texture \"bytes per pixel\" parameter.", Diagnostics::MessageType::Error);
			glDeleteTextures(1, &id);
			return 0;
		}

		// Для 1-байтных текстур устанавливаем специальные параметры
		if (data.bytes_per_pixel == 1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			format, // Внутренний формат
			static_cast<GLsizei>(data.size.x),
			static_cast<GLsizei>(data.size.y),
			0,
			format, // Формат данных
			GL_UNSIGNED_BYTE,
			data.pixels
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::stringstream ss;
			ss << "OpenGL error: 0x" << std::hex << error;
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Creating texture failed. OpenGL error:" + ss.str() + ".", Diagnostics::MessageType::Error);
			glDeleteTextures(1, &id);
			return 0;
		}
		auto key_id = GetNextID(IDType::Texture);
		m_textures[key_id] = id;
		return key_id;
	}
	void OpenGLDevice::BindTexture(Definitions::uint slot, const TextureID& id)
	{
		if (!CheckValid()) return;
		if (slot >= GL_MAX_TEXTURE_UNITS)
		{
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Binding texture failed: slot is greater than max value.", Diagnostics::MessageType::Error);
			return;
		}
		if (!m_textures.count(id))
		{
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Binding texture failed: invalid texture id.", Diagnostics::MessageType::Error);
			return;
		}
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_textures[id]);
	}
	void OpenGLDevice::DestroyTexture(const TextureID& id)
	{
		if (!CheckValid()) return;
		if (!m_textures.count(id)) 
		{
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Destroing texture failed: invalid texture id.", Diagnostics::MessageType::Error);
			return;
		}
		auto t = m_textures[id];
		glDeleteTextures(1, &t);
		m_textures.erase(id);
	}

	ShaderID OpenGLDevice::CreateShader(const std::string& name, const std::vector<ShaderSource>& sources)
	{
		if (!CheckValid()) return Definitions::InvalidID;
		std::string m_vertexSrc;
		std::string m_fragmentSrc;

		for (auto& source : sources) 
		{
			if (source.type == ShaderType::VERTEX) m_vertexSrc = source.sourceCode;
			if (source.type == ShaderType::FRAGMENT) m_fragmentSrc = source.sourceCode;
		}

		GLuint vertex = CompileShader(GL_VERTEX_SHADER, m_vertexSrc);
		GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, m_fragmentSrc);

		auto m_id = glCreateProgram();
		glAttachShader(m_id, vertex);
		glAttachShader(m_id, fragment);
		glLinkProgram(m_id);

		GLint success;
		glGetProgramiv(m_id, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(m_id, 512, nullptr, infoLog);
			std::stringstream ss;
			ss << infoLog;
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Shader's linking failed: " + ss.str() + ".", Diagnostics::MessageType::Error);
			return Definitions::InvalidID;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		//Creating GLSLShader

		auto key_id = GetNextID(IDType::Shader);

		m_shaders[key_id] = m_id;

		return key_id;
	}

	void OpenGLDevice::SetUniform(ShaderID shader, const std::string& name, const UniformValue& value)
	{
		if (!m_shaders.count(shader))
		{
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Shader's uniform set attemp is failed: invalid shader's id.", Diagnostics::MessageType::Error);
			return;
		}
		GLint id = glGetUniformLocation(m_shaders[shader], name.c_str());
		if (id > -1)
		{
			Math::Vector2 vec_value2;
			Math::Vector3 vec_value3;
			Math::Vector4 vec_value4;
			switch (value.type)
			{
			case UniformValue::ValueType::BOOL:
				glUniform1i(id, (GLboolean)value.GetBool());
				break;
			case UniformValue::ValueType::FLOAT:
				glUniform1f(id, (GLfloat)value.GetFloat());
				break;
			case UniformValue::ValueType::INT:
				glUniform1i(id, (GLint)value.GetInt());
				break;
			case UniformValue::ValueType::UINT:
				glUniform1ui(id, (GLuint)value.GetUInt());
				break;
			case UniformValue::ValueType::VECTOR2:
				vec_value2 = value.GetVector2();
				glUniform2f(id, (GLfloat)vec_value2.x, (GLfloat)vec_value2.y);
				break;
			case UniformValue::ValueType::VECTOR3:
				vec_value3 = value.GetVector3();
				glUniform3f(id, (GLfloat)vec_value3.x, (GLfloat)vec_value3.y, (GLfloat)vec_value3.z);
				break;
			case UniformValue::ValueType::VECTOR4:
				vec_value4 = value.GetVector4();
				glUniform4f(id, (GLfloat)vec_value4.x, (GLfloat)vec_value4.y, (GLfloat)vec_value4.z, (GLfloat)vec_value4.w);
				break;
			case UniformValue::ValueType::MATRIX4x4:
				glUniformMatrix4fv(id, 1, GL_TRUE, Math::GetValuePtr(value.GetMatrix4x4()));
				break;
			default:
				break;
			}
		}
		else 
		{
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Shader's uniform set attemp is failed: invalid uniform name \"" + name + "\" of shader with id " + std::to_string(shader) + ".", Diagnostics::MessageType::Error);
		}
	}


	void OpenGLDevice::Draw(const RenderCommand& render_command) {
		if (!CheckValid()) return;

		//TODO improve error system with stack

		// Attach shader
		BindShader(render_command.shader_id);

		// Attach textures
		for (const auto& [slot, texture_id] : render_command.textures) {
			BindTexture(slot, texture_id);
		}

		// Set uniformы
		for (const auto& [name, value] : render_command.uniforms) {
			SetUniform(render_command.shader_id, name, value);
		}

		// Set and draw mesh
		BindMesh(render_command.mesh_id);
		auto& mesh = m_meshes[render_command.mesh_id];

		glDrawElements(GL_TRIANGLES,
			static_cast<GLsizei>(mesh.indices_count),
			GL_UNSIGNED_INT, nullptr);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::stringstream ss;
			ss << "OpenGL error: 0x" << std::hex << error;
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Draw error. OpenGL error:" + ss.str() + ".", Diagnostics::MessageType::Error);
		}

	}
	void OpenGLDevice::DrawBatch(const std::vector<RenderCommand>& render_commands) {
		if (!CheckValid()) return;

		auto sorted_commands = render_commands;
		std::sort(sorted_commands.begin(), sorted_commands.end(), [](const auto& a, const auto& b) {
			// 1. Shader is the first (most expensive)

			if (a.shader_id != b.shader_id) return a.shader_id < b.shader_id;

			// 2. After by textures (cheaper)
			if (a.textures != b.textures) return a.textures < b.textures;

			// 3. By meshes (cheapest)
			return a.mesh_id < b.mesh_id;
			});

		ShaderID current_shader = Definitions::InvalidID;	
		std::vector<std::pair<TextureID, GLuint>> current_textures;		//texture - slot

		for (const auto& cmd : sorted_commands) {
			// Replace shader if he is changed
			if (cmd.shader_id != current_shader) {
				BindShader(cmd.shader_id);
				current_shader = cmd.shader_id;
				current_textures.clear(); // Reset textures
			}

			// Replace textures if they are changed
			if (cmd.textures != current_textures) {
				for (const auto& [tex_id, slot] : cmd.textures) {
					BindTexture(slot, tex_id);
				}
				current_textures = cmd.textures;
			}

			// Uniform's set every time (they are unique for every object)
			for (const auto& [name, value] : cmd.uniforms) {
				SetUniform(cmd.shader_id, name, value);
			}

			// Mesh bind always (it is cheap)
			BindMesh(cmd.mesh_id);
			glDrawElements(GL_TRIANGLES, m_meshes[cmd.mesh_id].indices_count, GL_UNSIGNED_INT, nullptr);
		}
	}

	void OpenGLDevice::UpdateMesh(const MeshID& id, const MeshData& data)
	{
		if (!CheckValid()) return;

		if (!m_meshes.count(id)) {
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Update mesh failed: invalid mesh id.", Diagnostics::MessageType::Error);
			return;
		}

		auto& mesh = m_meshes[id];

		// Обновляем VBO
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.vertices.size() * sizeof(Math::Vertex), data.vertices.data());
		mesh.vertices_count = data.vertices.size();
		// Обновляем EBO (если изменилось количество индексов)
		if (data.indices.size() != mesh.indices_count) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(GLuint),
				data.indices.data(), GetGLBufferMode(mesh.buffer_mode));
			mesh.indices_count = data.indices.size();
		}
		else {
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, data.indices.size() * sizeof(GLuint), data.indices.data());
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	void OpenGLDevice::SetRenderViewport(Viewports::Viewport* viewport)
	{
		if (!viewport || !CheckValid()) return;

		// Сохраняем текущий цвет очистки
		GLfloat prevColor[4];
		glGetFloatv(GL_COLOR_CLEAR_VALUE, prevColor);

		// Устанавливаем вьюпорт
		glViewport(viewport->GetPosition().x, viewport->GetPosition().y,
			viewport->GetResolution().x, viewport->GetResolution().y);

		// Настраиваем и применяем scissor
		glEnable(GL_SCISSOR_TEST);
		glScissor(viewport->GetPosition().x, viewport->GetPosition().y,
			viewport->GetResolution().x, viewport->GetResolution().y);

		// Очищаем с цветом фона вьюпорта
		const auto& bgColor = viewport->GetBackgroundColor();
		glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Восстанавливаем scissor и цвет
		glDisable(GL_SCISSOR_TEST);
		glClearColor(prevColor[0], prevColor[1], prevColor[2], prevColor[3]);

		m_CurrentViewport = viewport;
	}
	void OpenGLDevice::Set(GDSettings setting_type, GDSettingsValues setting_value)
	{

		switch (setting_type)
		{
		case Graphics::GDSettings::DEPTH_TEST:
			break;
		case Graphics::GDSettings::BLEND:
			break;
		case Graphics::GDSettings::BLEND_MODE:
			break;
		case Graphics::GDSettings::POLYGON_MODE:
			break;
		case Graphics::GDSettings::UNPACK_ALIGNMENT:
			break;
		default:
			break;
		}


		m_Settings[setting_type] = setting_value;
	}
	void OpenGLDevice::BindShader(const ShaderID& id)
	{
		if (!m_shaders.count(id)) 
		{
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Shader's binding failed: invalid shader's id.", Diagnostics::MessageType::Error);
			return;
		}

		glUseProgram(m_shaders[id]);

	}
	void OpenGLDevice::DestroyShader(const ShaderID& id)
	{
		if (!m_shaders.count(id))
		{
			Diagnostics::Logger::Get().SendMessage("(OpenGLDevice) Shader's destruction failed: invalid shader's id.", Diagnostics::MessageType::Error);
			return;
		}

		glDeleteProgram(m_shaders[id]);
		m_shaders.erase(id);

	}
}