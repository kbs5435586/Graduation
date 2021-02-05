#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

using namespace std;
using namespace chrono;

constexpr int LOOP = 50'000'000;
constexpr int MAX_THREADS = 32;

atomic_int sum = 0;
mutex m1;

void worker(int num_thread)
{
	for (int i = 0; i < LOOP / num_thread; ++i)
	{
		sum += 2;
		// sum = sum + 2�ϸ� ���� sum�� ������� ������� +2�ϴ� ������ ������ �ȵȴ�
	}
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