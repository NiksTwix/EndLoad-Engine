#include <Services/Diagnostics/Logger/Logger.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <format>

Definitions::identificator Diagnostics::Logger::m_nextHandlerID = 1;

void Diagnostics::Logger::WriteToConsole(const Message& message)
{
	std::string strtime = std::format("{:%Y:%m:%d:%H:%M}", message.time);

	switch (message.type)
	{
	case MessageType::Info:
		std::cout << strtime << "\x1b[37m " << message.text << "\x1b[0m" << std::endl;
		break;
	case MessageType::Warning:
		std::cout << strtime << "\x1b[33m" << message.text << "\x1b[0m" << std::endl;
		break;
	case MessageType::Error:
		std::cout << strtime << "\x1b[31m " << message.text << "\x1b[0m" << std::endl;
		break;
	default:
		std::cout << strtime << "\x1b[37m " << message.text << "\x1b[0m" << std::endl;
		break;
	}
}

Diagnostics::Logger::Logger()
{
	//Manual initialization is required here, because logger depends process observer
}

void Diagnostics::Logger::Init()
{
	std::lock_guard<std::mutex> lock(m_logMutex);
	if (m_isValid) return;

	m_logTrace.reserve(m_maxMessages);	//Excludes array allocation

	m_isValid = true;
}

void Diagnostics::Logger::Shutdown()
{
	SaveLogTrace();
	m_isValid = false;
}

Definitions::identificator Diagnostics::Logger::AddLogHandler(LogCallback log_handler)
{
	std::lock_guard<std::mutex> lock(m_logMutex);
	auto id = m_nextHandlerID++;

	m_handlers[id] = log_handler;

	return id;
}

bool Diagnostics::Logger::DeleteLogHandler(Definitions::identificator log_handler_id)
{
	std::lock_guard<std::mutex> lock(m_logMutex);
	if (!m_handlers.count(log_handler_id)) return false;

	return m_handlers.erase(log_handler_id);
}

void Diagnostics::Logger::SaveLogTrace()
{
	std::lock_guard<std::mutex> lock(m_logMutex);
	//Creates directory with name "Diagnostics"
	//Creates directory with name "Logs"
	//Creates or rewrite file with name "log_[programStartTime]"

	if (m_logTrace.empty()) return;

	// Создаём папки если их нет
	std::filesystem::create_directories("Diagnostics/Logs");

	// Формируем имя файла
	auto now = ProcessObserver::Get().GetCurrentSystemTime();;
	//std::time_t t = std::chrono::system_clock::to_time_t(now);
	//std::tm tm = *std::localtime(&t);
	std::string filename = std::format("Diagnostics/Logs/log_{:%Y-%m-%d_%H-%M-%S}.txt",
		now);
	// Пишем в файл
	std::ofstream file(filename);
	for (const auto& msg : m_logTrace) {
		file << std::format("[{0:%H:%M:%S}] {1}\n", msg.time, msg.text);
	}
	
	ClearLogTrace();
} 

void Diagnostics::Logger::SendMessage(std::string text, MessageType type)
{
	Message message;
	message.text = text;
	message.type = type;
	auto& process_observer = ProcessObserver::Get();
	message.time = process_observer.GetCurrentSystemTime();	//In another case Message:time = null_time_point

	SendMessage(message);
}

void Diagnostics::Logger::SendMessage(const Message& message)
{
	if (m_consoleOutputEnable) WriteToConsole(message);

	//handlers call

	for (auto& [_, handler] : m_handlers) 
	{
		handler(message);
	}

	if (m_logTrace.size() >= m_maxMessages) SaveLogTrace();
}

void Diagnostics::Logger::ClearLogTrace()
{

	m_logTrace.clear();
}
