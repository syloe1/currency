  #include <iostream>
	#include <thread>
	#include <vector>
	using namespace std;
	void printMessage(const string& message, int id) {
	    cout<<"Thread " <<id<<": "<<message<<'\n';
	}
	int main() {
	    const int numThreads = 10;
	    vector<thread> threads;
	    for (int i{0}; i < numThreads; ++i) {
	        threads.emplace_back(printMessage, "Hello from thread", i);
	    }
	    //等待所有线程完成
	    for (auto& t : threads) {
	        if (t.joinable()) {
	            t.join(); //阻塞线程， 直到线程t完成
	        }
	    }
	    cout<<"All threads have finished executions"<<'\n';
	    return 0;
	}
	//execution
	// g++ -o m_hello m_hello.cpp -pthread
