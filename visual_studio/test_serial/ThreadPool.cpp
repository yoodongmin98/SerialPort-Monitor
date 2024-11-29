#include "ThreadPool.h"



ThreadPool::ThreadPool()
{

}

ThreadPool::ThreadPool(size_t numThreads) : stop(false)
{
	//어차피 복잡하게 쓸거아니니까 람다캡처
	for (size_t i = 0; i < numThreads; ++i)
	{
		Worker.emplace_back([this]
			{
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(QueueMutex);

						condition.wait(lock, [this]
							{
								return stop || !tasks.empty();
							});

						if (stop && tasks.empty())
							return;

						task = std::move(tasks.front());
						tasks.pop();
					}
					task();
				}
			});
	}
}


ThreadPool::~ThreadPool()
{
	std::unique_lock<std::mutex> lock(QueueMutex);
	stop = true;
	
	condition.notify_all();
	for (std::thread& worker : Worker)
		worker.join();
}

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{

}



