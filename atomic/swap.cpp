#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> value(0);

void swapValues() {
    int oldValue = value.exchange(42); // 将 value 更新为 42，并返回旧值
    std::cout << "Old value was " << oldValue << std::endl;
}

int main() {
    std::thread t1(swapValues);
    std::thread t2(swapValues);
    t1.join();
    t2.join();

    std::cout << "Final value: " << value.load() << std::endl;
    return 0;
}
