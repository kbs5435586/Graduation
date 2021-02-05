#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

volatile int sum = 0;
atomic_bool flags[2] = { false,false };
atomic_int victim = 0;
// 피터슨 알고리즘 자체가 변수가 아토믹이라는 가정하에 만들어진 알고리즘임
void p_lock(int id)
{
	int other = 1 - id;
	flags[id] = true;
	victim = id;
	while ((true == flags[other] && victim == id)); 
	// 여기서 아웃 오브 오더 발생해서 flags[id] = true 먼저 안읽어서 두 thread가 전부 
}

void p_unlock(int id)
{
	flags[id] = false;
}

void worker(int id)
{
	for (int i = 0; i < 25'000'000; ++i)
	{
		p_lock(id);
		sum += 2;
		p_unlock(id);
	}
}

int main()
{
	thread con{ worker,0 };
	thread pro{ worker,1 };

	con.join();
	pro.join();

	cout << "result : " << sum << endl;
	return 0;
}