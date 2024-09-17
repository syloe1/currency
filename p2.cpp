/*
生产者-消费者问题：实现一个生产者-消费者模型，使用std::mutex和std::condition_variable来协调生产者和消费者之间的数据共享。
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
using namespace std;

queue<int> buffer;
const unsigned int maxBufferSize = 0;
mutex mtx;
condition_variable notFull;
condition_variable notEmpty;

//producter
void producer(int id) {
    for (int i{0}; i < 10; ++i) {
        this_thread::sleep_for(chrono::milliseconds(100));
        unique_lock<mutex> lock(mtx);

        notFull.wait(lock, []{return buffer.size() < maxBufferSize;});
        buffer.push(i);
        cout<<"Producer "<<id<<" produced "<<i<<'\n';
        notEmpty.notify_one();
    }
}

//consumer thread
void consumer(int id) {
    while (true) {
        unique_lock<mutex> lock(mtx);
        notEmpty.wait(lock, []{return !buffer.empty(); });
        int data = buffer.front();
        buffer.pop();
        cout<<"Consumer "<<id<<" consumed "<<data<<'\n';
        notFull.notify_one();
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

int main() {
    thread prod1(producer, 1);
    thread prod2(producer, 2);
    thread cons1(consumer, 1);
    thread cons2(consumer, 2);
    prod1.join(); prod2.join();
    cons1.join(); cons2.join();

    return 0;
}
