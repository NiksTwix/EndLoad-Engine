#pragma once
#include <string>
#include <SpecialHeaders/Definitions.hpp>
#include <any>
#include <typeindex>

namespace Resources
{
	enum class ResourceState {
		NotLoaded = 0,
		Loaded,     // Data loaded
		Initialized, // Inited in memory
		NeedReinit,
		Error
	};
	class IResource 
	{
	private:
		static ResourceID GetNext() 
		{
			static ResourceID next = 1;
			return next++;
		}
		ResourceID m_id = Definitions::InvalidID;
	public:
		IResource() 
		{
			m_id = GetNext();
		}

		ResourceID GetID() const { return m_id; }

		virtual ~IResource() = default;
		virtual bool Load(const std::string& path) = 0;	//Load FILE data

		virtual bool SetData(std::any data) = 0;	//Set data

		virtual std::type_index GetDataType() const = 0;

		virtual bool Init() = 0;	//Initializate resource to memory

		virtual bool Uninit() = 0;//When inheriting, don't forget to do a type check by sizeof

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
		ResourceState m_state =  ResourceState::NotLoaded;
		Windows::WindowID m_ownerWindow = Definitions::InvalidID; // Place of init
		template<typename T>
		bool ValidateData(const std::any& data) const {
			return data.type() == typeid(T);
		}
	};
}