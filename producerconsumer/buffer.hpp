#pragma once

#include <condition_variable>
#include <limits>
#include <mutex>
#include <queue>
#include <utility>

class BufferOperationInterrupted: public std::exception{};

// Thread-safe task queue
template<typename Task>
class Buffer
{
public:
    explicit Buffer() = default;
    ~Buffer() = default;

    // Wakes up every thread
    // Must be used after work flag change to interrupt operations
    void notifyAll();
    // Thread-safely adds task to the queue
    void emplace(Task&& task, const bool& workFlag);
    // Thread-safely gets front element and pops the queue
    Task getAndPop(const bool& workFlag);

private:
    // Task queue
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

    Task val = move(_taskQueue.front());
    _taskQueue.pop();
    lock.unlock();
    // Notify thread holded into emplace()
    _queueFullCondVar.notify_one();

    return val;
}
