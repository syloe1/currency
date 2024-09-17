#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> counter(0);

void increment() {
    for (int i = 0; i < 1000; ++i) {
        counter.fetch_add(1); // 原子加法
    }
}

void decrement() {
    for (int i = 0; i < 1000; ++i) {
        counter.fetch_sub(1); // 原子减法
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(decrement);
    t1.join();
    t2.join();

    std::cout << "Counter: " << counter.load() << std::endl;
    return 0;
}
