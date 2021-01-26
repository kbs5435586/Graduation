#include <iostream>
#include <thread>
#include <vector>

using namespace std;

int tid; // 전역 변수는 공유를 하므로 쓰레드가 서로 가져다가 씀

void thread_func(int id)
{
	tid = id;
	for (int i = 0; i < 10000; i++);
	cout << "My thead ID : " << tid << endl; // id는 로컬 변수가 되므로 공유를 안한다
}

int main()
{
	vector<thread> threads;

	for(int i = 0; i < 10; ++i)
	{
		threads.emplace_back(thread_func, i);
	}
	for (auto& t : threads)
	{
		t.join();
	}
}