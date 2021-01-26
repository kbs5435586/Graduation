#include <iostream>
#include <thread>
#include <vector>

using namespace std;

int tid; // ���� ������ ������ �ϹǷ� �����尡 ���� �����ٰ� ��

void thread_func(int id)
{
	tid = id;
	for (int i = 0; i < 10000; i++);
	cout << "My thead ID : " << tid << endl; // id�� ���� ������ �ǹǷ� ������ ���Ѵ�
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