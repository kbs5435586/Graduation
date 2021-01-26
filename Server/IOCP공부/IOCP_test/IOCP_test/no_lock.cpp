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
// �̷��� �����ϸ� sums�̶�� �迭�� ���� ��ġ�� 64����Ʈ ������ ��ġ�϶�� ����
// �ȿ� �ִ� ���ҵ��� ��ġ�� �ƹ� ��� ����, alignas�� ������ �� �տ� �;���

struct alignedint 
{
	//alignas(64) volatile int sum;
	alignas(256) volatile int sum; // ���� ��Ī���� ���� ���
};
alignedint sums[MAX_THREADS]; // sums�ȿ� �ִ� �迭�� 64����Ʈ ������ ������� �Ǽ� ĳ�� �������� �����

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
2�� �ۿ� �Ȼ�����

���� : �迭�� �浹�� ����Ŵ
�޸𸮸� ������ int�� �о��ٰ��ؼ� cpu�� �޸𸮿��� 4����Ʈ�� �о�°� �ƴ�
cpu�� ĳ���� �ְ� cpu�� �޸� ������ ĳ�� ���� ������ ����
intel�� ĳ�� ������ 64����Ʈ��

�׷��� 4����Ʈ �����͵��� �ھ ������ 4����Ʈ�� ������ �Ǵµ� 64����Ʈ ĳ�������� �� ����

�Ȱ��� �޸��� �ּҰ� ĳ�� �α��� �̻� �����ϸ� �ȵ� �Ѹ��� ������Ʈ�ϸ� �� ���� �ٸ� �Ѹ��� �о�ߵǴµ�
���� ĳ�������� �־ �ڱⲯ�� ������Ʈ�ϰ� ��밡 ������Ʈ�Ѱ� �Ⱥ���
�׷��� intel���� ���� ĳ�������� ���� �ھ ���� ���ϰ� ����� ����
�׷��� �� �ھ ĳ�������� ������Ʈ �Ϸ��� �Ҷ� ���� �ٸ� ĳ�������� ���� ���ϰ� ����
�׷��� �Ѹ� ������Ʈ�ߴٰ� �������� �װ� �ٸ� �ھ �޾ƿͼ� ������Ʈ �ϰ� �Դٰ�����

�װ� ĳ�� �������̶�� ��, �׷��� �ӵ��� ��Ƽ�ھ�� ������ ����

alignas(64)�� ���� �����Ϸ����� �޸𸮿� �Ҵ��� �Ҷ� ĳ������ ������ ����Ʈ��
*/