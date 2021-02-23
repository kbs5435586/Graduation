#include <iostream>
using namespace std;

// �긴���� ������� ���� �Ϲ����� ���
class CUnit
{
public:
	virtual void Attack() = 0;
};

class CHorseRider : public CUnit
{
public:
	virtual void Attack()
	{
		cout << "���� ź��" << endl;
	}
};

class CHorseBow : public CHorseRider
{
public:
	virtual void Attack()
	{
		CHorseRider::Attack();
		cout << "Ȱ�� ��� �ο��" << endl;
	}
};

class CHorseSpear : public CHorseRider
{
public:
	virtual void Attack()
	{
		CHorseRider::Attack();
		cout << "â�� ��� �ο��" << endl;
	}
};

class CHorseSword : public CHorseRider
{
public:
	virtual void Attack()
	{
		CHorseRider::Attack();
		cout << "���� ��� �ο��" << endl;
	}
};

class CSoldier : public CUnit
{
public:
	virtual void Attack()
	{
		cout << "�ѹ���" << endl;
	}
};

class CLancer : public CSoldier
{
public:
	virtual void Attack()
	{
		CSoldier::Attack();
		cout << "â�� ��� �ο��" << endl;	// ���⼭ ����!
	}
};

void main()
{
	CUnit* pUnit1 = new CHorseBow;
	CUnit* pUnit2 = new CHorseSpear;
	CUnit* pUnit3 = new CHorseSword;

	pUnit1->Attack();

	cout << "------------------------" << endl;

	pUnit2->Attack();

	cout << "------------------------" << endl;

	pUnit3->Attack();

	delete pUnit1;
	delete pUnit2;
	delete pUnit3;
}