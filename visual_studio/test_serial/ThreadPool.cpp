#include "ThreadPool.h"



ThreadPool::ThreadPool()
{

}

ThreadPool::ThreadPool(size_t numThreads)
{
    for (size_t i = 0; i < numThreads; ++i)
    {
        Worker.push_back(std::thread(&ThreadPool::WorkerThread, this));
    }
}


ThreadPool::~ThreadPool()
{
	
}



void ThreadPool::WorkerThread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(QueueMutex);
        {

        }
    }
}


void ThreadPool::AddWork(std::function<void()> _function)
{
    if (stop) 
    {
        throw std::runtime_error("ThreadPool is stopped.");
    }
    tasks.push(_function);
}
