#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

int sync_data = 0;
//volatile bool ready = false;
bool ready = false;
mutex m;

void produce()
{
	m.lock();
	sync_data = 999;
	ready = true;
	m.unlock();
}

void customer()
{
	while (true)
	{
		lock_guard <mutex> gl(m);
		if (true == ready)
			break;
	}
	while (false == ready);
	// ready를 메모리에서 안읽고 레지스터에 저장해둔 ready 값이랑 무한으로 비교해서 무한루프가 안풀림
	// 그렇다고 그냥 메모리에서 읽기에는 매번 메모리 접근으로 인해 cpu, 메모리 과열과 전기소모가 일어남
	// 다른 병렬식으로 돌아가는 프로그램도 건들고 성능 저하가 일어남, 그래서 레지스터 값이랑 비교해서 
	// 무한루프 일어나는게 정상임, 릴리스 모드에선 컴파일러가 최적화 
	// 하지만 디버깅 모드에서는 매번 ready를 직업 읽어서 에러가 안남 
	cout << "recived " << sync_data << endl;
}

int main()
{
	thread con{ customer };
	thread pro{ produce };

	con.join();
	pro.join();

	return 0;
}