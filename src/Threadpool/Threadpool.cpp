#include <Threapool/Threadpool.h>

ThreadPool* ThreadPool::instance = 0;

ThreadPool::ThreadPool(std::function<void(Router, Socket)> socketFunction, Router router) {
    using std::placeholders::_1;
    this->numTasks = 0;
    this->socketHandler = std::bind(socketFunction, router, _1);
    size_t workerNumber = std::thread::hardware_concurrency();
    auto worker = [&] {
        std::cout << "Worker up" << std::endl;
        Socket client;
        this->functionMutex.lock();
        this->functionMutex.unlock();
        for (;;) {
            if (!this->isPoolRunning)
                return;
            this->functionMutex.lock();
            if (!this->clientQueue.empty()) {
                client = this->clientQueue.front();
                this->clientQueue.pop();
                this->functionMutex.unlock();
                this->socketHandler(client);
                this->numTasks--;
            } else {
                this->functionMutex.unlock();
                std::this_thread::yield();
            }
        }
    };
    for (int i = 0; i < workerNumber; i++) {
        this->threads.emplace_back(std::thread(worker));
    }
}

ThreadPool::~ThreadPool() {
    this->functionMutex.lock();
    auto tmp = std::queue<Socket>();
    this->clientQueue.swap(tmp);
    this->functionMutex.unlock();
    this->isPoolRunning = false;
    for(auto & t : this->threads)
        t.join();
}

ThreadPool *ThreadPool::GetInstance(std::function<void(Router, Socket)> *socketFunction, Router * router) {
    if(router == nullptr || socketFunction == nullptr)
    {
        if(ThreadPool::instance != nullptr)
            return ThreadPool::instance;
        throw std::invalid_argument("Invalid arguments - Router/socketFunctions cannot be nullptr!");
    }
    ThreadPool::instance = new ThreadPool(*socketFunction, *router);
    return ThreadPool::instance;
}

void ThreadPool::AddWork(Socket & socket) {
    if(!this->isPoolRunning)
        throw std::runtime_error("Pool not started!");
    this->functionMutex.lock();
    this->clientQueue.push(socket);
    this->functionMutex.unlock();
    this->numTasks++;
}

void ThreadPool::WaitAll() {
    if(!this->isPoolRunning)
        return;
    while(this->numTasks.load() > 0)
        std::this_thread::yield();
}


