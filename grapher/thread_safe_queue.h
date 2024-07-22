
#include <mutex>
#include <queue>
#include <optional>

template<typename T>
class ThreadSafeQueue{
    std::mutex lock;
    std::queue<T> queue;
public:
    void Push(const T& item){
        lock.lock();
        queue.push(item);
        lock.unlock();
    };
    std::optional<T> Pop(){
        const std::lock_guard<std::mutex> plock(lock);

        if(queue.empty())
            return std::nullopt;
        T& item = queue.front();    
        queue.pop();
        return std::optional<T>(item);
    };
};