#pragma once
#include <Core/IServices.hpp>
#include <unordered_map>
#include <string>

enum class RenderTypes
{
	OpenGL,
	Vulkan
};
enum class WindowAPIs
{
	SDL,
	GLFW
};

enum class Switches 
{
	FrustumCulling
};


class SettingsManager : public IHelperService
{
public:
	SettingsManager() { Init(); };
	~SettingsManager() = default;

	// Разрешаем перемещение
	SettingsManager(SettingsManager&&) = default;
	SettingsManager& operator=(SettingsManager&&) = default;

	RenderTypes GetRenderType() { return RenderType; }
	WindowAPIs GetWindowAPI() { return WindowAPI; }

	virtual void Init() { if (m_isValid) return; m_isValid = true; }  // Опционально
	virtual void Shutdown() { m_isValid = false; }

	void LoadSettingsFromFile(std::string path_to_file) {};

	void ResetBuffers() {};

	bool GetSwitch(Switches switch_) { return bool_values[switch_]; }
	void SetSwitch(Switches switch_, bool value) { bool_values[switch_] = value; }

	float GetEngineVersion() { return engineVersion; }
	std::string GetEngineName() { return engineName; }

private:
	RenderTypes RenderType = RenderTypes::OpenGL;
	WindowAPIs WindowAPI = WindowAPIs::GLFW;

	float engineVersion = 0.1f;	//ENGINE_VERSION
	std::string engineName = "EndLoad Engine"; //ENGINE_NAME

	std::unordered_map<Switches, bool> bool_values;
};