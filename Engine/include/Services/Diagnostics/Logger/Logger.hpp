#pragma once
#include <Services\Diagnostics\\ProcessObserver\ProcessObserver.hpp>
#include <functional>
#include <string>
#include <mutex>

namespace Diagnostics
{
	enum class MessageType
	{
		Info,
		Warning,
		Error,
	};

	struct Message 
	{
		std::string text;
		MessageType type = MessageType::Info;
		uclock::time_point time = null_time_point;
	};

	using log_function = std::function<void(const Message& message)>;	//void(const Message&)

	class Logger : public Core::IService
	{
	private:
		const size_t m_maxMessages = 50;
		std::vector<Message> m_logTrace;		//Array for logging in file, if messages > max_messages (or ~Logger or WriteToFile) then we write to log file with program start time in name

		//m_logTrace.reserve(m_maxMessages)

		std::unordered_map<Definitions::identificator, log_function> m_handlers;

		static Definitions::identificator m_nextHandlerID;

		bool m_consoleOutputEnable = true;

		std::mutex m_logMutex;

		void WriteToConsole(const Message& message);

	public:
		Logger();
		//basic
		void Init() override;
		void Shutdown() override;

		//work with handlers
		Definitions::identificator AddLogHandler(log_function log_handler);
		bool DeleteLogHandler(Definitions::identificator log_handler_id);
		//settings
		void SetConsoleOutput(bool value) { m_consoleOutputEnable = value; }
		//Logging
		void SaveLogTrace();
		void SendMessage(std::string text, MessageType type = MessageType::Info);
		void SendMessage(const Message& message);
		void ClearLogTrace();
		const std::vector<Message>& GetLogTrace() const { return m_logTrace; }
	};
}

