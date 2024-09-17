/*
线程池实现：实现一个简单的线程池，可以提交任务并处理它们。使用std::queue和std::condition_variable来管理任务队列。
*/
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <future>
#include <mutex>
#include <condition_variable>

using namespace std;

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    
    // 提交任务并返回future
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> future<typename result_of<F(Args...)>::type>;

private:
    // 工作线程
    vector<thread> workers;
    
    // 任务队列
    queue<function<void()>> tasks;
    
    // 同步机制
    mutex queueMutex;
    condition_variable condition;
    bool stop;
};

// 构造函数
ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                function<void()> task;
                
                {
                    unique_lock<mutex> lock(this->queueMutex);
                    
                    // 等待任务或者停止
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });
                    
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }
                    
                    task = move(this->tasks.front());
                    this->tasks.pop();
                }
                
                task();
            }
        });
    }
}

// 析构函数
ThreadPool::~ThreadPool() {
    {
        unique_lock<mutex> lock(queueMutex);
        stop = true;
    }
    
    condition.notify_all();
    
    for (thread &worker : workers) {
        worker.join();
    }
}

// 提交任务
template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> future<typename result_of<F(Args...)>::type> {
    using returnType = typename result_of<F(Args...)>::type;
    
    auto task = make_shared<packaged_task<returnType()>>(bind(forward<F>(f), forward<Args>(args)...));
    
    future<returnType> res = task->get_future();
    
    {
        unique_lock<mutex> lock(queueMutex);
        
        // 不允许在停止后添加任务
        if (stop) {
            throw runtime_error("enqueue on stopped ThreadPool");
        }
        
        tasks.emplace([task]() { (*task)(); });
    }
    
    condition.notify_one();
    return res;
}

int main() {
    ThreadPool pool(4);
    
    auto f1 = pool.enqueue([](int x, int y) { return x + y; }, 2, 3);
    auto f2 = pool.enqueue([](const string& str) { return str + " world!"; }, "Hello");
    
    cout << "Result of f1: " << f1.get() << endl;
    cout << "Result of f2: " << f2.get() << endl;
    
    return 0;
}
