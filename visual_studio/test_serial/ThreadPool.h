#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <functional>
#include <queue>
#include <condition_variable>
#include <future>
#include <memory>


class ThreadPool
{
public:
	ThreadPool();
	ThreadPool(size_t numThreads);

	~ThreadPool(); 

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F(Args...)>::type> {
        using returnType = typename std::invoke_result<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<returnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<returnType> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(QueueMutex);

            if (stop)
                throw std::runtime_error("Enqueue�Լ��� ThreadPool���� ������ϴ�");

            tasks.emplace([task]() { (*task)(); }); 
        }
        condition.notify_one();
        return res;
    }


   
protected:
private:
	//���ư� �������
	std::vector<std::thread> Worker;
	//Queue�� ������ Task(Funtion���� ����)
	std::queue<std::function<void()>> tasks;
	std::mutex QueueMutex;
	std::condition_variable condition;
	//���ߴ°� ������ Flag
	std::atomic<bool> stop;
};