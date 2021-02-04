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
	// ���⼭ 64����Ʈ ĳ�� ���� �� ������ �Űܳ����� -2����Ʈ �Ű����鼭 2���� ĳ�������� ��ġ�� �ȴ�
	// �׷��� �а� ���� 2���� ĳ�������� �Դٰ����ϸ� �а� ����, �׷��� �� �ھ �� ĳ�������� �а�
	// �Ʒ� ĳ�������� �б� ���� �ٸ� �ھ �� ���� �о������ �������� �ʱⰪ�� �ƴ� �߰����� �о�������
	// ĳ�� ���� ������ �ٿ����
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