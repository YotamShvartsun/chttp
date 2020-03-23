/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the implementation of the Threadpool class, which (as its
 * name suggests) is the CHttp version of the well-known design pattern
 * 'thread-pool'
 */
#include <Threapool/Threadpool.h>

Threadpool *Threadpool::instance = nullptr;

Threadpool::Threadpool(
    const std::function<void(Router *, std::shared_ptr<Socket>)>
        &socketFunction,
    Router *router) {
  using std::placeholders::_1;
  this->numTasks = 0;
  this->socketHandler = std::bind(socketFunction, router, _1);
  size_t workerNumber = std::thread::hardware_concurrency();
  /**
   * This function (stored in worker) is the main function run by the workers
   * It waits for a new client, and then tries to serve it, using the
   * ``Threadpool::socketHandler``
   */
  auto worker = [&] {
    std::shared_ptr<Socket> client;
    for (;;) {
      std::unique_lock<std::mutex> lock(this->clientMutex);
      // Check if the lock is free and if the worker can serve clients (there
      // are clients in the line and the pool is running)
      this->canExecuteWorker.wait(lock, [&] {
        return !this->isPoolRunning || !this->clientQueue.empty();
      });
      if (this->isPoolRunning) {
        client = this->clientQueue.front();
        this->clientQueue.pop();
        // After finishing everything that should be done with the clientQueue,
        // the lock can be freed
        lock.unlock();
        if (!client->IsClosed()) { // If the socket is already closed (for some
                                   // reason) skip this client
          this->socketHandler(client); // Handle the client
          client->Close();             // Close the resources
          this->numTasks--;            // Task completed!
        }
      } else
        return;
    }
  };
  std::thread t;
  for (int i = 0; i < workerNumber; i++) {
    t = std::thread(worker);
    this->threads.push_back(std::move(t));
  }
  std::cout << "[INFO]: Threadpool runing" << std::endl;
}

Threadpool::~Threadpool() {
  this->clientMutex.lock();
  // Empty the client queue
  auto tmp = std::queue<std::shared_ptr<Socket>>();
  this->clientQueue.swap(tmp);
  this->numTasks = 0;
  // The pool is no longer running
  this->isPoolRunning = false;
  // Allow all workers to die
  this->clientMutex.unlock();
  this->canExecuteWorker.notify_all();
  // Clear the worker threads
  this->threads.clear();
}

Threadpool *Threadpool::GetInstance(
    std::function<void(Router *, std::shared_ptr<Socket>)> *socketFunction,
    Router *router) {
  if (router == nullptr || socketFunction == nullptr) {
    if (Threadpool::instance != nullptr) {
      return Threadpool::instance;
    }
    // If no instance exists, the arguments must supply a way to create it!
    throw std::invalid_argument(
        "Invalid arguments - Router/socketFunctions cannot be nullptr!");
  }
  Threadpool::instance = new Threadpool(*socketFunction, router);
  return Threadpool::instance;
}

void Threadpool::AddWork(const std::shared_ptr<Socket> &client) {
  if (!this->isPoolRunning)
    throw std::runtime_error("Pool not started!");
  // Add client to queue
  this->clientMutex.lock();
  this->clientQueue.push(client);
  this->clientMutex.unlock();
  // Allow one worker to handle the client
  this->canExecuteWorker.notify_one();
  this->numTasks++;
}

void Threadpool::Reset() { Threadpool::instance = nullptr; }
