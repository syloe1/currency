#include <iostream>
#include <atomic>
#include <thread>
using namespace std;
atomic<int> cnt(0);
void increment() {
    for (int i{0}; i < 1000; ++i) ++cnt;
}
int main() {
    thread t1(increment);
    thread t2(increment);
    t1.join(); t2.join();
    cout<<"Cnt: "<<cnt.load()<<"\n";
    return 0;
}