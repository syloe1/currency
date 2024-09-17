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
#include <condition_varibale>
using namespace std;
class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    template<type F, typename... Args>
    auto enqueue(F&& f, ARgs&&... args) -> future<typename result_of<F(Args...)>::type>;
private:
    vector<thread> workers;
    queue<functional<void()>> tasks;
    mutex queueMutex;
    condition_varibale condition;
    bool stop;
};
ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i{0}; i < numThreads; ++i) {
        workers.emplace_back([this] {
            function<void()> task;
            {
                unique_lock<mutex> lock(this->queueMutex);
                this->condition.wait(lock, [this]{
                    return this->stop || this->tasks.empty();
                });
                if (this->stop && this->tasks.empty()) {return ;}
                task = move(this->tasks.front());
                this->tasks.pop();
            }
            task();
        });
    }
}
ThreadPool::~ThreadPool() {
    {
        unique_lock<mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (thread& worker : workers) {
        worker.join();
    }
}

template<typename F, typename... Args> 
auto ThreadPool::enqueue(F&& f, Args&&... args) -> future<typename result_of<F(Args...)>::type> {
    using returnType = typename result_of<F(Args...)>::type;
    auto task = make_shared<packages_task<returnType()>>(bind(forward<F>(f), forward<Args>(args)...));
    future<returnType> res = task->get_future();
    {
        unique_lock<mutex> lock(queueMutex);
        if (stop) {
            throw runtime_error("enqueue on stopped threadpool");
        }
        tasks.emplace([task](){(*task)(); });
    }
    condition.notify_one();
    return res;
}
int main() {
    ThreadPool pool(4);
    auto f1 = pool.enqueue([](int x, int y) {return x + y ;}, 2, 3);
    auto f2 = pool.enqueue([](const string& str){return str + " world!";}; "Hello");
    cout<<"Result of f1: "<<f1.get()<<'\n';
    cout<<"Result of f2: "<<f2.get()<<'\n';
    return 0;
}
