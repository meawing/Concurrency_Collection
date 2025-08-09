#pragma once

#include <vector>
#include <lines/std/atomic.hpp>
#include <lines/std/thread.hpp>
#include <lines/std/condvar.hpp>
#include <lines/std/mutex.hpp>
#include "task.hpp"
#include "queue.hpp"
#include "wg.hpp"

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();

    void Submit(Task task);
    void Wait();

    static ThreadPool* This();

private:
    void Work();

private:
    MPMCBlockingUnboundedQueue<Task> task_queue_;
    WaitGroup wait_group_;
    lines::Atomic<bool> is_stopping_{false};
    std::vector<lines::Thread> threads_;
    
    static inline thread_local ThreadPool* current_pool = nullptr;
};
