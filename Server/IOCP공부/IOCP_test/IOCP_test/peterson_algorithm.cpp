#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

volatile int sum = 0;
volatile bool flags[2] = { false,false };
volatile int victim = 0;

void p_lock(int id)
{
	int other = 1 - id;
	flags[id] = true;
	atomic_thread_fence(memory_order_seq_cst);
	victim = id;
	while ((true == flags[other] && victim == id)); 
	// ���⼭ �ƿ� ���� ���� �߻��ؼ� flags[id] = true ���� ���о �� thread�� ���� 
}

void p_unlock(int id)
{
	atomic_thread_fence(memory_order_seq_cst);
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