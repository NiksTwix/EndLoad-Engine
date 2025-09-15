#include <Services\Diagnostics\ProcessObserver\ProcessObserver.hpp>


Diagnostics::ProcessObserver::ProcessObserver()
{
	m_programStartTime = Diagnostics::uclock::now();
}

Diagnostics::ProcessObserver& Diagnostics::ProcessObserver::Get()
{
	static Diagnostics::ProcessObserver po;
	return po;
}

void Diagnostics::ProcessObserver::ResetStartTime()
{
	m_programStartTime = uclock::now();
}

Diagnostics::uclock::time_point Diagnostics::ProcessObserver::GetStartTime() const
{
	return m_programStartTime;
}

Diagnostics::uclock::time_point Diagnostics::ProcessObserver::GetCurrentTime() const
{
	return uclock::now();
}

float Diagnostics::ProcessObserver::GetProgramTimeMS() const
{
	auto now = uclock::now();
	auto delta = now - m_programStartTime;
	return std::chrono::duration<float>(delta).count() * 1000;
}

void Diagnostics::ProcessObserver::CalculateDelta()
{
	auto now = uclock::now();
	auto delta = now - m_lastTime;
	m_lastTime = now;

	m_frameTime = std::chrono::duration<float>(delta).count();
	m_fps = 1.0f / m_frameTime;
	
	m_smoothFPS = m_smoothFPS * 0.9f + m_fps * 0.1f;
}
