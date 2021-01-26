#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;
using namespace chrono;

constexpr int LOOP = 50'000'000;
constexpr int MAX_THREADS = 32;

volatile int sum = 0;
// alignas(64) volatile int sums[MAX_THREADS]; 
// 이렇게 선언하면 sums이라는 배열의 시작 위치를 64바이트 단위로 배치하라는 뜻임
// 안에 있는 원소들의 배치랑 아무 상관 없음, alignas은 무조건 맨 앞에 와야함

struct alignedint 
{
	//alignas(64) volatile int sum;
	alignas(256) volatile int sum; // 프리 패칭으로 성능 향상
};
alignedint sums[MAX_THREADS]; // sums안에 있는 배열이 64바이트 단위로 얼라인이 되서 캐쉬 쓰레싱이 사라짐

void worker(int num_thread,int thread_id)
{
	for (int i = 0; i < LOOP / num_thread; ++i)
	{
		sums[thread_id].sum += 2;
	}
}

int main()
{
	for (int n = 1; n <= MAX_THREADS; n *= 2)
	{
		vector<thread> workers;
		sum = 0;
		for (auto& s : sums) 
			s.sum = 0;

		auto st = high_resolution_clock::now();
		for (int i = 0; i < n; ++i)
			workers.emplace_back(worker, n, i);

		for (auto& th : workers)
			th.join();

		for (auto& s : sums) sum += s.sum;

		auto et = high_resolution_clock::now();
		auto du = et - st;

		cout << n << "Threads, Sum : " << sum << endl;
		cout << "	Time : " << duration_cast<milliseconds>(du).count() << " msecs\n";
	}
} 

/*
2배 밖에 안빨라짐

이유 : 배열이 충돌을 일으킴
메모리를 읽을떄 int를 읽었다고해서 cpu가 메모리에서 4바이트를 읽어가는게 아님
cpu엔 캐쉬가 있고 cpu가 메모리 읽을땐 캐쉬 라인 단위로 읽음
intel의 캐쉬 라인은 64바이트임

그래서 4바이트 데이터들을 코어가 읽을때 4바이트만 읽으면 되는데 64바이트 캐쉬라인을 다 읽음

똑같은 메모리의 주소가 캐쉬 두군대 이상 존재하면 안됨 한명이 업데이트하면 그 값을 다른 한명이 읽어야되는데
각자 캐쉬라인이 있어서 자기껏만 업데이트하고 상대가 업데이트한게 안보임
그래서 intel에선 같인 캐쉬라인을 여러 코어가 공유 못하게 만들어 놨음
그래서 한 코어가 캐쉬라인을 업데이트 하려고 할때 옆에 다른 캐쉬라인을 접근 못하게 막음
그래서 한명 업데이트했다가 내보내고 그걸 다른 코어가 받아와서 업데이트 하고 왔다갔다함

그걸 캐쉬 쓰래슁이라고 함, 그래서 속도가 멀티코어보다 느리게 나옴

alignas(64)를 쓰면 컴파일러에서 메모리에 할당을 할때 캐쉬라인 단위로 떨어트림
*/