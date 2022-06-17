#pragma once

#include <condition_variable>
#include <limits>
#include <mutex>
#include <queue>
#include <utility>

// Exception class used in Buffer class to indicate interruption of operation
class BufferOperationInterrupted: public std::exception{};

// Thread-safe queue wrapper
template<typename Task>
class Buffer
{
public:
    explicit Buffer() = default;
    ~Buffer() = default;

    // Wake up every thread
    // Must be used after if work flag state get changed
    void notifyAll();
    // Thread-safely add task to the queue
    void emplace(Task&& task, const bool& workFlag);
    // Thread-safely get front element and pop the queue
    Task getAndPop(const bool& workFlag);

private:
    // Main container
    std::queue<Task> _taskQueue;
    // Mutex for every queue modification
    std::mutex _queueMtx;
    // Thread control for full queue case
    std::condition_variable _queueEmptyCondVar;
    // Thread control for empty queue case
    std::condition_variable _queueFullCondVar;
    // Queue max size limiter
    static constexpr int _queueMaxSize = std::numeric_limits<int>::max();
};

template<typename Task>
void Buffer<Task>::notifyAll()
{
    _queueEmptyCondVar.notify_all();
    _queueFullCondVar.notify_all();
}

template<typename Task>
void Buffer<Task>::emplace(Task&& task, const bool& workFlag)
{
    std::unique_lock<std::mutex> lock(_queueMtx);

    // Hold thread if tasks limit has been reached
    while(_taskQueue.size() >= _queueMaxSize)
    {
        // Interrupt operation if work flag is false
        if(!workFlag)
        {
            // Interrupt called. New tasks cannot be added
            throw BufferOperationInterrupted{};
        }

        // Wait notification from getAndPop() or notifyAll()
        _queueFullCondVar.wait(lock);
    }

    _taskQueue.emplace(std::forward<Task>(task));
    lock.unlock();
    // Notify thread holded into getAndPop()
    _queueEmptyCondVar.notify_one();
}

template<typename Task>
Task Buffer<Task>::getAndPop(const bool& workFlag)
{
    std::unique_lock<std::mutex> lock(_queueMtx);

    // Hold thread if there are no tasks available
    while(_taskQueue.empty())
    {
        if(!workFlag)
        {
            // Interrupt called
            throw BufferOperationInterrupted{};
        }

        // Wait notification from getAndPop() or notifyAll()
        _queueEmptyCondVar.wait(lock);
    }

    Task val = std::move(_taskQueue.front());
    _taskQueue.pop();
    lock.unlock();
    // Notify thread holded into emplace()
    _queueFullCondVar.notify_one();

    return val;
}
