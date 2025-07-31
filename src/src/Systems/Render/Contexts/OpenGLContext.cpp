#pragma once
#include <Systems/Render/Contexts/OpenGLContext.hpp>
#include <Core\Logger.hpp>
#include <Services\Scene\SceneManager.hpp>
#include <Services\Settings\SettingsManager.hpp>
#include <Services/Resources/ResourceManager.hpp>
#include <Systems\Render\Drawings.hpp>
#include <Core\ServiceLocator.hpp>

OpenGLContext::OpenGLContext()
{
	Init();
}

void OpenGLContext::Init()
{
	is_valid = true;
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}

void OpenGLContext::Update(Viewport* viewport, SceneID scene) {
}

void OpenGLContext::ClearBuffers()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D , 0);
}

void OpenGLContext::SetViewport(Viewport* viewport) {
	if (!viewport) return;

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

	m_currentViewport = viewport;
}

unsigned int OpenGLContext::SetupMesh(std::vector<Math::Vertex> vertices, std::vector<unsigned int> indices, RCBufferModes mode)
{
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	auto mode_ = GL_STATIC_DRAW;

	switch (mode)
	{
	case RCBufferModes::STATIC:
		mode_ = GL_STATIC_DRAW;
		break;
	case RCBufferModes::DYNAMIC:
		mode_ = GL_DYNAMIC_DRAW;
		break;
	case RCBufferModes::STREAM:
		mode_ = GL_STREAM_DRAW;
		break;
	}

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Math::Vertex), vertices.data(), mode_);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), mode_);

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
		ServiceLocator::Get<Logger>()->Log("(OpenGL & OpenGLContext): mesh upload to gpu error:" + ss.str(), Logger::Level::Error);
	}

	ActiveGBuffers[VAO].push_back(VBO);
	ActiveGBuffers[VAO].push_back(EBO);

	return VAO;
}

void OpenGLContext::UpdateVBOData(unsigned int vao, std::vector<Math::Vertex> vertices)
{
	if (ActiveGBuffers.count(vao) == 0)
	{
		ServiceLocator::Get<Logger>()->Log("(OpenGL & OpenGLContext): update vbo error: incorrect vao.", Logger::Level::Error);
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, ActiveGBuffers[vao][0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Math::Vertex) * vertices.size(), vertices.data());

	glBindVertexArray(0);
}

void OpenGLContext::DestroyMesh(unsigned int vao)
{
	if (ActiveGBuffers.find(vao) != ActiveGBuffers.end()) 
	{
		for (auto& b : ActiveGBuffers[vao]) 
		{
			glDeleteBuffers(1,&b);
		}
		glDeleteVertexArrays(1, &vao);
		ActiveGBuffers.erase(vao);
	}
}

void OpenGLContext::DrawElements(unsigned int vao, int index_count)
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	auto error = glGetError();

	if (error != 0)
	{
		std::stringstream ss;
		ss << glewGetErrorString(error);
		ServiceLocator::Get<Logger>()->Log("(OpenGL & OpenGLContext): draw elements error:" + ss.str(), Logger::Level::Error);
	}
}

void OpenGLContext::BindTexture(unsigned int tblock,unsigned int value)
{
	// Проверка максимального количества текстур
	if (tblock >= GL_MAX_TEXTURE_IMAGE_UNITS) {
		ServiceLocator::Get<Logger>()->Log(
			"(OpenGL & OpenGLContext): exceeded maximum texture units.",
			Logger::Level::Error
		);
		return;
	}
	glActiveTexture(GL_TEXTURE0 + tblock);
	glBindTexture(GL_TEXTURE_2D, value);
}

unsigned int OpenGLContext::CreateTexture(const void* pixels, Math::Vector2 size, unsigned int bytes_per_pixel)
{
	if (size.x <= 0 || size.y <= 0) {
		ServiceLocator::Get<Logger>()->Log(
			"(OpenGL & OpenGLContext): invalid texture size",
			Logger::Level::Error
		);
		return 0;
	}

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	GLenum format;
	switch (bytes_per_pixel) {
	case 1: format = GL_RED; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	default:
		ServiceLocator::Get<Logger>()->Log(
			"(OpenGL & OpenGLContext): invalid bytes_per_pixel",
			Logger::Level::Error
		);
		glDeleteTextures(1, &id);
		return 0;
	}

	// Для 1-байтных текстур устанавливаем специальные параметры
	if (bytes_per_pixel == 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	}

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format, // Внутренний формат
		static_cast<GLsizei>(size.x),
		static_cast<GLsizei>(size.y),
		0,
		format, // Формат данных
		GL_UNSIGNED_BYTE,
		pixels
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::stringstream ss;
		ss << "OpenGL error: 0x" << std::hex << error;
		ServiceLocator::Get<Logger>()->Log(
			"(OpenGL & OpenGLContext): " + ss.str(),
			Logger::Level::Error
		);
		glDeleteTextures(1, &id);
		return 0;
	}

	return id;
}

void OpenGLContext::DestroyTexture(unsigned int id)
{
	glDeleteTextures(1, &id);
}

void OpenGLContext::Set(RCSettings set, RCSettingsValues value) {
	switch (set) {
	case RCSettings::DEPTH_TEST:
		value == RCSettingsValues::TRUE ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
		break;

	case RCSettings::BLEND:
		value == RCSettingsValues::TRUE ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
		break;

	case RCSettings::BLEND_MODE:
		if (value == RCSettingsValues::SRC_ALPHA) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Фикс: правильная смесь для прозрачности
		}
		else if (value == RCSettingsValues::ONE_MINUS_SRC_ALPHA) {
			glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE);  // Альтернативный режим
		}
		break;

	case RCSettings::POLYGON_MODE:  // Исправлено: POLIGON → POLYGON
		switch (value) {
		case RCSettingsValues::LINES:   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case RCSettingsValues::FILL:    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		case RCSettingsValues::POINTS:  glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
		default: break;
		}
		break;
	case RCSettings::UNPACK_ALIGNMENT:
		switch (value)
		{
		case RCSettingsValues::ONE_BYTE: glPixelStorei(GL_UNPACK_ALIGNMENT, 1); break;
		case RCSettingsValues::TREE_BYTES: glPixelStorei(GL_UNPACK_ALIGNMENT, 3); break;
		case RCSettingsValues::FOUR_BYTES: glPixelStorei(GL_UNPACK_ALIGNMENT, 4); break;
		}
		break;
	default:
		break;
	}
}


void OpenGLContext::Shutdown()
{
	is_valid = false;

	std::vector<GLuint> keys;		//Для избежания ошибок сохраняем ключи в отдельный массив

	for (auto vao : ActiveGBuffers) 
	{
		keys.push_back(vao.first);
	}
	for (auto vao : keys) DestroyMesh(vao);
}

Viewport* OpenGLContext::GetViewport()
{
	return m_currentViewport;
}

void OpenGLContext::ResetBuffers()
{
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Сброс текстуры
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
