#pragma once
#include <Core\IServices.hpp>
#include <chrono>


namespace Diagnostics 
{

	using uclock = std::chrono::steady_clock;
	constexpr auto null_time_point = uclock::time_point();
	class ProcessObserver
	{
		uclock::time_point m_programStartTime = uclock::now();

		//FPS
		uclock::time_point m_lastTime;
		float m_fps = 0.0f;
		float m_smoothFPS = 60.0f;
		float m_frameTime = 0.0f;

		ProcessObserver();
	public:

		static ProcessObserver& Get();

		void ResetStartTime();	//startProgram will be now time

		uclock::time_point GetStartTime() const;
		uclock::time_point GetCurrentTime() const;

		float GetProgramTimeMS() const;

		//FPS
		void CalculateDelta();
		
		float GetFPS() const { return  m_smoothFPS; }
		float GetFrameTimeMS() const { return  m_frameTime * 1000; }
		float GetFrameTime() const { return  m_frameTime; }

		static float GetFPSST() { return Get().GetFPS(); }
		static float GetFrameTimeMSST() { return Get().GetFrameTimeMS(); }
	};
}