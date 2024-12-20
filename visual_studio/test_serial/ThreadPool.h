#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>


class ThreadPool
{
public:
	static ThreadPool* TP;
	ThreadPool();
	ThreadPool(size_t numThreads);

	~ThreadPool(); 

	
	void AddWork(std::function<void()> _function);
	void ClearWork();
	std::queue<std::function<void()>> GetTasks()
	{
		return tasks;
	}
	std::vector<std::thread>& GetWorkers()
	{
		return Worker;
	}
	void Resize(size_t numThreads);
protected:
	void WorkerThread();
private:
	//돌아갈 쓰레드들
	std::vector<std::thread> Worker;
	std::mutex QueueMutex;
	std::queue<std::function<void()>> tasks;
	std::condition_variable condition;
	std::atomic<bool> stop;
};