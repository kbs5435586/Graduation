#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;
using namespace chrono;

constexpr int LOOP = 50'000'000;
constexpr int MAX_THREADS = 32;

volatile int sum = 0;
mutex m1;

void worker(int num_thread)
{
	volatile int local_sum = 0;
	for (int i = 0; i < LOOP / num_thread; ++i)
	{
		local_sum += 2;
	}
	m1.lock();
	sum += local_sum;  // 임계영역의 크기를 줄여버림
	m1.unlock();
}

int main()
{
	for (int n = 1; n <= MAX_THREADS; n *= 2)
	{
		vector<thread> workers;
		sum = 0;

		auto st = high_resolution_clock::now();
		for (int i = 0; i < n; ++i)
			workers.emplace_back(worker, n);

		for (auto& th : workers)
			th.join();

		auto et = high_resolution_clock::now();
		auto du = et - st;

		cout << n << "Threads, Sum : " << sum << endl;
		cout << "	Time : " << duration_cast<milliseconds>(du).count() << " msecs\n";
	}
}