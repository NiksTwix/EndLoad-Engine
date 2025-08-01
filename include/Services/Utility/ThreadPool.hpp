#pragma once
//��� �������
#include <atomic>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable> //�������� ����������

class ThreadPool 
{
public:
	ThreadPool(size_t count_of_threads)
	{
		for (int i = 0; i < count_of_threads; i++) 
		{
			m_workers.emplace_back([this]() //����������� ������-��������� � �����
				{
					while (true) //�������� ������
					{
						std::function<void()> task;
						{
							//���� ����, ����� ������� ������������� �����������
							std::unique_lock<std::mutex> lock(m_queue_mutex);

							m_condition.wait(lock, [this]() //��� ���� � ������� ��������� ������ ��� ��� �� ���������
								{
									return m_stop || !m_tasks.empty(); //������� ����������� -> ����� ������������
								});

							// ���� ��� ���������� � ����� ��� � ��������� �����
							if (m_stop && m_tasks.empty()) return;

							// ���� ������ �� �������
							task = std::move(m_tasks.front());
							m_tasks.pop();
						}
						// ��������� ������ 
						task();
					}

				});
		}
	}
	~ThreadPool() {
		{
			std::unique_lock<std::mutex> lock(m_queue_mutex);
			m_stop = true; //������������� ���
		}
		m_condition.notify_all();
		for (auto& worker : m_workers) {
			if (worker.joinable()) worker.join(); //������ ����������� ���� ������
		}
	}
	//������� ����� ������
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