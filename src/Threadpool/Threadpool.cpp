#include <Threapool/Threadpool.h>

ThreadPool *ThreadPool::instance = nullptr;

ThreadPool::ThreadPool(const std::function<void(Router*, std::shared_ptr<Socket>)> &socketFunction, Router* router) {
    using std::placeholders::_1;
    this->numTasks = 0;
    this->socketHandler = std::bind(socketFunction, router, _1);
    size_t workerNumber = std::thread::hardware_concurrency();
    auto worker = [&] {
        std::cout << "Worker up" << std::endl;
        std::shared_ptr<Socket> client;
        for (;;) {
            std::unique_lock<std::mutex> gurd(this->functionMutex);
            this->toRun.wait(gurd, [&] { return !this->isPoolRunning || !this->clientQueue.empty(); });
            if (this->isPoolRunning) {
                client = this->clientQueue.front();
                this->clientQueue.pop();
				gurd.unlock();
                this->socketHandler(client);
                client->Close();
                this->numTasks--;;
            } else
                return;
        }
    };
    std::thread t;
    for (int i = 0; i < workerNumber; i++) {
        t = std::thread(worker);
        this->threads.push_back(std::move(t));
    }
}

ThreadPool::~ThreadPool() {
    this->functionMutex.lock();
    auto tmp = std::queue<std::shared_ptr<Socket>>();
    this->clientQueue.swap(tmp);
    this->isPoolRunning = false;
    this->functionMutex.unlock();
    this->toRun.notify_all();
//    for(auto &t : this->threads)
//        t.join();
    this->threads.clear();
    this->numTasks = 0;
}

ThreadPool *
ThreadPool::GetInstance(std::function<void(Router*, std::shared_ptr<Socket>)> *socketFunction, Router *router) {
    if (router == nullptr || socketFunction == nullptr) {
        if (ThreadPool::instance != nullptr)
            return ThreadPool::instance;
        throw std::invalid_argument("Invalid arguments - Router/socketFunctions cannot be nullptr!");
    }
    ThreadPool::instance = new ThreadPool(*socketFunction, router);
    return ThreadPool::instance;
}

void ThreadPool::AddWork(const std::shared_ptr<Socket> &socket) {
    if (!this->isPoolRunning)
        throw std::runtime_error("Pool not started!");
    this->functionMutex.lock();
    this->clientQueue.push(socket);
    this->functionMutex.unlock();
    this->toRun.notify_one();
    this->numTasks++;
}

void ThreadPool::WaitAll() {
    if (!this->isPoolRunning)
        return;
    while (this->numTasks.load() > 0)
        std::this_thread::yield();
}
void ThreadPool::Reset() {
  ThreadPool::instance = nullptr;
}
