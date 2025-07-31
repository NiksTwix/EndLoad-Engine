#include "Core/Logger.hpp"


void Logger::Log(const std::string& message, Logger::Level level) //TODO add time marking
{
	std::lock_guard<std::mutex> lock(m_logMutex);
	LogEntry entry;
	entry.message = message;
	entry.level = level;

	if (m_timeMark) {
		entry.timestamp = GetCurrentTime() + ":";
	}

	WriteToConsole(entry);
	m_logBuffer.push_back(entry);

	if (m_logBuffer.size() >= MAX_BUFFER_SIZE) {
		WriteToFile();
	}
	
}

void Logger::Shutdown()
{
	if (!m_logBuffer.empty()) {
		WriteToFile();
	}
	if (m_logFile.is_open()) {
		m_logFile.close();
	}
	Log("Logger shutdown completed.", Level::Info);
	m_isValid = false;
}

void Logger::SetLogFile(std::string path)
{
	m_logFile.open(path, std::ios::app);
	if (!m_logFile.is_open()) {
		Log("(Logger): failed to open log file.", Level::Error);
	}
}

void Logger::WriteToFile()
{
	if (m_logFile.is_open())
	{
		for (auto str : m_logBuffer) 
		{
			m_logFile << str.timestamp << str.message << std::endl;
		}
	}
}

void Logger::WriteToConsole(LogEntry& log_entry)
{
	switch (log_entry.level)
	{
	case Logger::Level::Info:
		std::cout << log_entry.timestamp << "\x1b[37m" << log_entry.message << "\x1b[0m" << std::endl;
		break;
	case Logger::Level::Warning:
		std::cout << log_entry.timestamp << "\x1b[33m" << "Warning: " << log_entry.message << "\x1b[0m" << std::endl;
		break;
	case Logger::Level::Error:
		std::cout << log_entry.timestamp << "\x1b[31m" << "Error: " << log_entry.message << "\x1b[0m" << std::endl;
		break;
	case Logger::Level::Success:
		std::cout << log_entry.timestamp << "\x1b[32m" << log_entry.message << "\x1b[0m" << std::endl; //"Success: " << 
		break;
	default:
		break;
	}
}

std::string Logger::GetCurrentTime()
{
	auto now = std::chrono::steady_clock::now();
	auto delta = now - m_startTime;
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
	return std::to_string(ms);
}



