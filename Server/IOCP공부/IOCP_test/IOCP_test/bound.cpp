#include <iostream>
#include <thread>

using namespace std;

constexpr int LOOP = 25'000'000;
volatile bool done = false;
volatile int* bound;
int error = 0;

char cc;
int a;

void bounce()
{
	for (int i = 0; i < LOOP; ++i)
		*bound = -(1 + *bound);
	done = true;
}

void inspector()
{
	while (false == done)
	{
		int v = *bound;
		if ((v != 0) && (v != -1))
		{
			cout << hex << v << " , ";
			error++;
		}
	}
}

int main()
{
	cout << "cc: " << reinterpret_cast<long long>(&cc) << endl;
	cout << "a : " << reinterpret_cast<long long>(&a) << endl;
	int a[32];
	long long temp = reinterpret_cast<long long>(&a[31]);
	temp -= temp % 64;
	temp -= 2; 
	// 여기서 64바이트 캐쉬 라인 맨 앞으로 옮겨놓은게 -2바이트 옮겨지면서 2개의 캐쉬라인을 걸치게 된다
	// 그래서 읽고 쓸때 2개의 캐쉬라인을 왔다갔다하면 읽고 쓴다, 그래서 한 코어가 윗 캐쉬라인을 읽고
	// 아래 캐쉬라인을 읽기 전에 다른 코어가 그 사이 읽어버리면 최종값과 초기값이 아닌 중간값이 읽어져버림
	// 캐쉬 라인 사이즈 바운더리
	// byte
	bound = reinterpret_cast<int*>(temp);
	*bound = 0;
	thread t1{ bounce };
	thread t2{ inspector };

	t1.join();
	t2.join();

	cout << "error : " << error << endl;
	return 0;
}