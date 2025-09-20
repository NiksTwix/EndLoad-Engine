#pragma once
#include <string>
#include <mutex>
#include <Multithreading\ThreadPool.hpp>

namespace Core 
{
	class EngineCore
	{
	public:
		EngineCore(const EngineCore&) = delete;
		EngineCore& operator=(const EngineCore&) = delete;
		static EngineCore& Get();
		bool IsRunning() { return m_isRunning; }
		void Start();
		void Stop();
		void Shutdown();
	private:
		Multithreading::ThreadPool m_ThreadPool = Multithreading::ThreadPool(2);
		std::mutex m_coreMutex;
		void Mainloop();
		EngineCore();
		void Init();
		bool m_stoped = false;
		bool m_isRunning = false;

	};
}
