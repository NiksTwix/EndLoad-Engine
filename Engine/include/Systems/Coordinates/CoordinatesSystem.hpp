#pragma once
#include <Core\IServices.hpp>
#include <Services/Scenes/SceneManager.hpp>
#include <condition_variable>
/*
	Coordinates system update coordinate-depended components and data
*/


namespace Coordinates 
{
	class CoordinatesSystem : public Core::ISystem 
	{
	private:
		std::atomic<bool> m_updated{ false };
		std::mutex m_coordinatesMutex;
		std::condition_variable m_update_cv;
	public:
		CoordinatesSystem();
		void Init() override;
		void Shutdown() override;
		Core::SystemPriority GetPriority() override { return Core::SystemPriority::COORDINATES; }
		void Update() override;
		void UpdateScene(Scenes::SceneContext* context);

		bool IsUpdated() const {return m_updated;}
		void NotifyAll();
		std::mutex& GetMutex() { return m_coordinatesMutex; }
		void WaitForCompletion();
		bool IsNeedAsync() override { return true; }
	};
}