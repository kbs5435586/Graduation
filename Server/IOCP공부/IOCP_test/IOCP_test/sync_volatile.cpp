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
	// ready�� �޸𸮿��� ���а� �������Ϳ� �����ص� ready ���̶� �������� ���ؼ� ���ѷ����� ��Ǯ��
	// �׷��ٰ� �׳� �޸𸮿��� �б⿡�� �Ź� �޸� �������� ���� cpu, �޸� ������ ����Ҹ� �Ͼ
	// �ٸ� ���Ľ����� ���ư��� ���α׷��� �ǵ�� ���� ���ϰ� �Ͼ, �׷��� �������� ���̶� ���ؼ� 
	// ���ѷ��� �Ͼ�°� ������, ������ ��忡�� �����Ϸ��� ����ȭ 
	// ������ ����� ��忡���� �Ź� ready�� ���� �о ������ �ȳ� 
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