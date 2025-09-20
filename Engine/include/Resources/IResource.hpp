#pragma once
#include <string>


namespace Resources
{
	enum class ResourceState {
		NotLoaded,
		Loaded,     // Данные загружены
		Initialized, // Инициализирован в памяти
		Error
	};

	
	class IResource 
	{
	public:
		virtual ~IResource() = default;
		virtual bool Load(const std::string& path) = 0;	//Load file data

		virtual bool Init() = 0;	//Initializate resource to memory

		virtual void Release() = 0;
		virtual std::string GetType() const = 0;

		std::string GetPath() const { return m_path; }

		virtual bool Reload() {
			Release();
			return Load(m_path) && Init();
		}

		virtual ResourceState GetState() const { return m_state; };
	protected:
		std::string m_resourceName = "IResource";
		std::string m_path;
		ResourceState m_state;
	};
}