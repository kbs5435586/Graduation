#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

volatile int sum = 0;
/*
volatile

변수를 다룰때 실행속도를 더 빠르게 하기위해서 컴파일러가 변수를 
레지스터에 저장해서 모든 연산이 거기서 이뤄짐
근데 volatile를 선언해주면 컴파일러가 최적화를 하지 않고 
무조건 메모리에다가 직접 쓰고 참조할때도 메모리에서 참조하지
레지스터에 저장해놓고 그 값을 참조하는 짓은 안함

원래는 레지스터 선언이 있었는데 씨언어 표준이 바뀌면서 사라짐
근데 volatile은 있는 이유가 컴파일러가 최적화 처리를 해선 안되는
코드들이 있음 / 벤치마킹 코드, IO에 관련있는 코드 (파일 전송, 레지스터 입출력, 메모리 맵드 IO

메모리 맵드 IO는 메모리 주소를 가지고 IO를 하는데 컴파일러가 최적화 한다고
메모리 변형을 해서 레지스터에만 들어가고 메모리에 안들어가 오작동을 일으키는걸 방지
*/

int main()
{
	auto st = high_resolution_clock::now();
	for (int i = 0; i < 50'000'000; ++i) // 돌려보면 살짝 멈칫함, 오래 걸려서
		sum += 2;
	auto et = high_resolution_clock::now();
	auto du = et - st; 

	cout << "Sum : " << sum << endl;
	cout << "Time : " << duration_cast<milliseconds>(du).count() << " msecs\n";
	// 0msec으로 너무 빠른 결과가 나오는데 그건 컴파일러가 너무 똑똑해서 우리가 시킨대로
	// 안하고 본인이 최선의 방법을 선택해버려서 그럼

	// 릴리즈 모드로 돌리면 우리가 선택한 방법과 컴터가 추천하는 방법이 합쳐져버리서 성능 자동 향상
	// 그래서 정확한 성능측정이 불가능함
}