#pragma once
//Пул потоков
#include <atomic>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable> //Условная переменная


namespace Multithreading 
{
	class ThreadPool
	{
	public:
		ThreadPool(size_t count_of_threads)
		{
			for (int i = 0; i < count_of_threads; i++)
			{
				m_workers.emplace_back([this]() //L-func to thread
					{
						while (true) //Work all time
						{
							std::function<void()> task;
							{
								//Code block for automaticaly mutex clearing
								std::unique_lock<std::mutex> lock(m_queue_mutex);

								m_condition.wait(lock, [this]() //Wait while queue is clear or pool will be stoped
									{
										return m_stop || !m_tasks.empty(); //Condition valid -> pool starting
									});

								// If pool stoped or there aren't tasks - stop
								if (m_stop && m_tasks.empty()) return;

								// Take task from queue
								task = std::move(m_tasks.front());
								m_tasks.pop();
							}
							// Make task
							task();
						}

					});
			}
		}
		~ThreadPool() {
			{
				std::unique_lock<std::mutex> lock(m_queue_mutex);
				m_stop = true; //stop pool
			}
			m_condition.notify_all();
			for (auto& worker : m_workers) {
				if (worker.joinable()) worker.join(); //Pool's do last tasks
			}
		}
		//Вставка новой задачи
		template<class F>
		void Enqueue(F&& task) {
			{
				std::unique_lock<std::mutex> lock(m_queue_mutex);
				m_tasks.emplace(std::forward<F>(task));
			}
			m_condition.notify_one();
		}
	private:
		std::vector<std::thread> m_workers;
		std::queue<std::function<void()>> m_tasks;
		std::mutex m_queue_mutex;
		std::condition_variable m_condition;
		std::atomic<bool> m_stop;
	};
}
