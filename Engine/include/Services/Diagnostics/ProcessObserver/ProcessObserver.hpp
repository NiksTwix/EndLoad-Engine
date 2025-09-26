#pragma once
#include <Core\IServices.hpp>
#include <chrono>


namespace Diagnostics 
{

	using stclock = std::chrono::steady_clock;
	using sclock = std::chrono::system_clock;
	constexpr auto null_time_point_st = stclock::time_point();
	constexpr auto null_time_point_s = sclock::time_point();

	enum class TimeFormat
	{
		System,
		Program
	};

	class ProcessObserver : public Core::IService
	{
		stclock::time_point m_programStartTime = stclock::now();

		//FPS
		stclock::time_point m_lastTime;
		float m_fps = 0.0f;
		float m_smoothFPS = 60.0f;
		float m_frameTime = 0.0f;

		size_t currentFrame;

		ProcessObserver();
	public:

		static ProcessObserver& Get();

		void ResetStartTime();	//startProgram will be now time

		stclock::time_point GetStartTime() const;
		stclock::time_point GetCurrentTime() const;
		sclock::time_point GetCurrentSystemTime() const;

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