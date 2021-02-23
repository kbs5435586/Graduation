#include <iostream>
using namespace std;

// �긴�� ������ �������� ��

// ������(Implementor)
class CAttackMotion
{
public:
	virtual void Motion() = 0;
};

class CAttackBow : public CAttackMotion
{
public:
	virtual void Motion()
	{
		cout << "Ȱ�� ��� �ο�" << endl;
	}
};

class CAttackSpear : public CAttackMotion
{
public:
	virtual void Motion()
	{
		cout << "â�� ��� �ο�" << endl;
	}
};

class CAttackSword : public CAttackMotion
{
public:
	virtual void Motion()
	{
		cout << "���� ��� �ο�" << endl;
	}
};

// �߻���(Abstraction)
class CUnit
{
public:
	virtual void Attack() = 0;
	
public:
	void Create(CAttackMotion* pMotion)
	{
		m_pMotion = pMotion;
	}

protected:
	CAttackMotion*	m_pMotion;
};

class CHorseRider : public CUnit
{
public:
	virtual void Attack()
	{
		cout << "���� ź��" << endl;
		m_pMotion->Motion();
	}
};

class CSoldier : public CUnit
{
public:
	virtual void Attack()
	{
		cout << "�ѹ���" << endl;
		m_pMotion->Motion();
	}
};

void main()
{
	// �긴�� ������ �߻����� �������� �и� ��Ű�� �̵��� ������ �������� �����Ѵ�.
	// ����, Ȯ�弳�谡 ����������.


	// ������
	CAttackMotion* pAttackBow = new CAttackBow;
	CAttackMotion* pAttackSpear = new CAttackSpear;
	CAttackMotion* pAttackSword = new CAttackSword;


	// �߻���
	CUnit* pHorseRider = new CHorseRider;
	pHorseRider->Create(pAttackBow);
	pHorseRider->Attack();

	cout << "------------------------------------" << endl;

	pHorseRider->Create(pAttackSpear);
	pHorseRider->Attack();

	cout << "------------------------------------" << endl;

	pHorseRider->Create(pAttackSword);
	pHorseRider->Attack();

	CUnit* pSoldier = new CSoldier;
	pSoldier->Create(pAttackBow);
	pSoldier->Attack();

	cout << "------------------------------------" << endl;

	pSoldier->Create(pAttackSpear);
	pSoldier->Attack();

	cout << "------------------------------------" << endl;

	pSoldier->Create(pAttackSword);
	pSoldier->Attack();

	delete pSoldier;
	delete pHorseRider;
	delete pAttackSpear;
	delete pAttackBow;
	delete pAttackSword;
}