/**
 * @file
 * @author Yotam Shvartsun <yotam.shvartsun@gmail.com>
 * @version 1.0
 * @section DESCRIPTION
 * This file contains the definition of the Threadpool class, which (as its name
 * suggests) is the CHttp version of the well-known design pattern 'thread-pool'
 */
#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include <chttp/Router.h>
#include <chttp/util/Socket.h>

/**
 * This class is a thread-pool implementation for the CHttp framework
 * @note This is a singleton!
 */
class Threadpool {
private:
  /**
   * The instance of the pool
   */
  static Threadpool *instance;
  /**
   * Mutex guarding the clients queue
   */
  std::mutex clientMutex;
  /**
   * Hold whether the pool is running or not.
   * This is an atomic variable, which means its thread-safe by default and dose
   * not require a lock to protect itself
   */
  std::atomic_bool isPoolRunning{true};
  /**
   * Number of waiting clients in the queue
   */
  std::atomic<int> numTasks{};
  /**
   * A condition variable which syncs the workers
   */
  std::condition_variable canExecuteWorker;
  /**
   * Create a threadpool instance
   * @param socketFunction Function to run for every client
   * @param router Router pointer used by `socketFunction`
   */
  Threadpool(const std::function<void(Router *, std::shared_ptr<Socket>)>
                 &socketFunction,
             Router *router);
  /**
   * Queue of clients waiting to be handled
   */
  std::queue<std::shared_ptr<Socket>> clientQueue;
  /**
   * Vector containing the threads created by the pool
   */
  std::vector<std::thread> threads;
  /**
   * The function responsible for handling clients
   */
  std::function<void(std::shared_ptr<Socket>)> socketHandler;

public:
  /**
   * Get the instance of the threadpool. If the instance dose not exist, create
   * a new one.
   * @param socketFunction Function to run for every client
   * @param router Router pointer used by `socketFunction`
   * @return Instance of the thread-pool
   */
  static Threadpool *GetInstance(
      std::function<void(Router *, std::shared_ptr<Socket>)> *socketFunction,
      Router *router);
  /**
   * Set the instance to nullptr
   */
  static void Reset();
  /**
   * Add a new client to the queue
   * @param client Client to add
   */
  void AddWork(const std::shared_ptr<Socket> &client);
  /**
   * Destroy the thread-pool
   */
  ~Threadpool();
};