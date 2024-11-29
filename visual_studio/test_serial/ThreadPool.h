#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <functional>
#include <queue>
#include <condition_variable>
#include <memory>


class ThreadPool
{
public:
	ThreadPool();
	ThreadPool(size_t numThreads);

	~ThreadPool(); 

	
	void AddWork(std::function<void()> _function);
   
protected:
	void WorkerThread();
private:
	//돌아갈 쓰레드들
	std::vector<std::thread> Worker;
	std::mutex QueueMutex;
	std::queue<std::function<void()>> tasks;
	std::atomic<bool> stop;
};