#include "tp.hpp"
#include <lines/util/thread_local.hpp>

ThreadPool::ThreadPool(size_t num_threads) {
    // Initialize the thread pool and start worker threads.
    for (size_t i = 0; i < num_threads; ++i) {
        threads_.emplace_back([this]() {
            // Set the thread-local pointer for the current thread pool.
            current_pool = this;
            Work();
            current_pool = nullptr;  // Reset after thread exits.
        });
    }
}

ThreadPool::~ThreadPool() {
    // Signal threads to stop and join them.
    is_stopping_ = true;
    task_queue_.Close();

    for (auto& thread : threads_) {
        thread.join();
    }
}

void ThreadPool::Submit(Task task) {
    // Increment the active task count and add the task to the queue.
    wait_group_.Add(1);
    task_queue_.Push(std::move(task));
}

void ThreadPool::Wait() {
    // Wait for all tasks to finish and close the task queue.
    wait_group_.Wait();
    task_queue_.Close();
}

ThreadPool* ThreadPool::This() {
    // Return the thread-local pointer to the thread pool, or nullptr if not set.
    return current_pool;
}

void ThreadPool::Work() {
    // Worker thread loop: process tasks until the pool is stopped.
    while (!is_stopping_) {
        // Attempt to pop a task from the queue.
        auto task = task_queue_.Pop();
        if (!task.has_value()) {
            // The queue is closed, exit the loop.
            break;
        }

        // Execute the task.
        try {
            (*task)();
        } catch (...) {
            // Handle exceptions if necessary.
        }

        wait_group_.Done();
    }
}
