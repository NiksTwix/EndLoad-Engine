#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <chrono>
#include <Core/IServices.hpp>


class Logger : public IHelperService
{
public:
	enum class Level 
	{
		Info,
		Warning,
		Error,
		Success
	};

	Logger() { Init(); }
	virtual ~Logger() = default;

	virtual void Log(const std::string& message, Logger::Level level = Level::Info);

	virtual void Init() 
	{
		m_startTime = std::chrono::steady_clock::now();
		m_isValid = true;
	}  // Опционально
	virtual void Shutdown() override;

	virtual void SetLogFile(std::string path);

	void SetTimeMark(bool value) { m_timeMark = value; }
private:
	struct LogEntry {
		std::string message;
		Level level;
		std::string timestamp;
	};
	std::mutex m_logMutex;
	bool m_timeMark = true;
	std::ofstream m_logFile;
	std::vector<LogEntry> m_logBuffer;
	static const size_t MAX_BUFFER_SIZE = 1000;

	void WriteToFile(); //Запись лога в файл при переполнении
	void WriteToConsole(LogEntry& log_entry); //Запись лога в файл при переполнении
	std::string GetCurrentTime();

	std::chrono::time_point<std::chrono::steady_clock> m_startTime;

};