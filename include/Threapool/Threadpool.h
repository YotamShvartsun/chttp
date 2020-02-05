#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <vector>
#include <functional>
#include <stdexcept>

#include <chttp/util/Socket.h>
#include <chttp/Router.h>

class ThreadPool {
private:
    static ThreadPool *instance;
    std::mutex functionMutex;
    std::atomic_bool isPoolRunning = true;
    std::atomic<int> numTasks{};

    ThreadPool(const std::function<void(Router, Socket *)> &socketFunction, Router router);

    ~ThreadPool();

    std::queue<Socket *> clientQueue;
    std::vector<std::thread> threads;
    std::function<void(Socket *)> socketHandler;
public:
    static ThreadPool *GetInstance(std::function<void(Router, Socket *)> *socketFunction, Router *);

    void AddWork(Socket *);

    void WaitAll();
};