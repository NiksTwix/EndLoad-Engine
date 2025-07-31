#pragma once
#include <Core/IServices.hpp>
#include <unordered_map>
#include <string>

enum class RenderTypes
{
	OpenGL,
	Vulkan
};


enum class Switches 
{
	FrustumCulling
};

enum class NumberValues 
{
	MaxFPS
};

enum class Paths 
{
	MeshShader,
	SpriteShader,
	PlaneShader,		//Шейдер простой 2D панели
	TextShader
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

	virtual void Init() 
	{  
		if (m_isValid) return; m_isValid = true; 

		//Предустановка
		bool_values[Switches::FrustumCulling] = false;
		path_values[Paths::MeshShader] = "EngineAssets\\Graphical\\OpenGL\\Shaders\\MeshShader.elsh";
		path_values[Paths::SpriteShader] = "EngineAssets\\Graphical\\OpenGL\\Shaders\\SpriteShader.elsh";
		path_values[Paths::PlaneShader] = "EngineAssets\\Graphical\\OpenGL\\Shaders\\PlaneShader.elsh";
		path_values[Paths::TextShader] = "EngineAssets\\Graphical\\OpenGL\\Shaders\\TextShader.elsh";
	} 
	virtual void Shutdown() { m_isValid = false; }

	void LoadSettingsFromFile(std::string path_to_file) {};

	void ResetBuffers() {};

	bool GetSwitch(Switches switch_) { return bool_values[switch_]; }
	void SetSwitch(Switches switch_, bool value) { bool_values[switch_] = value; }

	float GetEngineVersion() { return engineVersion; }
	std::string GetEngineName() { return engineName; }

	std::string GetPaths(Paths path_) { return path_values[path_]; }
	void SetPaths(Paths path_, bool value) { path_values[path_] = value; }


private:
	RenderTypes RenderType = RenderTypes::OpenGL;

	float engineVersion = 0.1f;	//ENGINE_VERSION
	std::string engineName = "EndLoad Engine"; //ENGINE_NAME

	unsigned int maxFPS = 80;

	std::unordered_map<Switches, bool> bool_values;
	std::unordered_map<NumberValues, int> number_values;

	std::unordered_map<Paths, std::string> path_values;
};