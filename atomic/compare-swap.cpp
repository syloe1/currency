#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> value(0);

void updateIfZero() {
    int expected = 0;
    int newValue = 42;
    // 只有当 value 的值等于 expected 时，才会将 value 设置为 newValue
    if (value.compare_exchange_strong(expected, newValue)) {
        std::cout << "Value was updated to " << newValue << std::endl;
    } else {
        std::cout << "Value was not updated. It was " << value.load() << std::endl;
    }
}

int main() {
    std::thread t1(updateIfZero);
    std::thread t2(updateIfZero);
    t1.join();
    t2.join();

    std::cout << "Final value: " << value.load() << std::endl;
    return 0;
}
