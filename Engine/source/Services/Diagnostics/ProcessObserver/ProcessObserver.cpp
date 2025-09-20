#include <Services\Diagnostics\ProcessObserver\ProcessObserver.hpp>


Diagnostics::ProcessObserver::ProcessObserver()
{
	Init();
	m_programStartTime = Diagnostics::stclock::now();
}

Diagnostics::ProcessObserver& Diagnostics::ProcessObserver::Get()
{
	static Diagnostics::ProcessObserver po;
	return po;
}

void Diagnostics::ProcessObserver::ResetStartTime()
{
	m_programStartTime = stclock::now();
}

Diagnostics::stclock::time_point Diagnostics::ProcessObserver::GetStartTime() const
{
	return m_programStartTime;
}

Diagnostics::stclock::time_point Diagnostics::ProcessObserver::GetCurrentTime() const
{
	return stclock::now();
}

Diagnostics::sclock::time_point Diagnostics::ProcessObserver::GetCurrentSystemTime() const
{
	return sclock::now();
}

float Diagnostics::ProcessObserver::GetProgramTimeMS() const
{
	auto now = stclock::now();
	auto delta = now - m_programStartTime;
	return std::chrono::duration<float>(delta).count() * 1000;
}

void Diagnostics::ProcessObserver::CalculateDelta()
{
	auto now = stclock::now();
	auto delta = now - m_lastTime;
	m_lastTime = now;

	m_frameTime = std::chrono::duration<float>(delta).count();
	m_fps = 1.0f / m_frameTime;
	
	m_smoothFPS = m_smoothFPS * 0.9f + m_fps * 0.1f;
}
