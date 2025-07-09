#pragma once

class TaskQueue
{
public:
	using Task = std::function<void()>;

	void push(Task task)
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.push(std::move(task));
		}

		m_cv.notify_one();
	}

	// Call this from main thread to drain tasks
	void drain()
	{
		std::queue<Task> tasks;
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			std::swap(tasks, m_queue);
		}

		while (!tasks.empty())
		{
			tasks.front()();
			tasks.pop();
		}
	}

private:
	std::mutex m_mutex;
	std::queue<Task> m_queue;
	std::condition_variable m_cv;
};