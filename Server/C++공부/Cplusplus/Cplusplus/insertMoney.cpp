#include "pch.h"
#include "insertMoney.h"

void insertMoney(vector<Info*>& list)
{
	char searchID[20];
	int insertCash;
	bool isFinished = false;

	cout << "�� �̸� : ";
	cin >> searchID;

	cout << "�Աݾ� : ";
	cin >> insertCash;

	for (auto& iter : list)
	{
		if (strcmp(iter->name, searchID) == 0)
		{
			iter->cash += insertCash;
			cout << "�Ա� �Ϸ�! " << endl;
			isFinished = true;
			break;
		}
	}

	if (!isFinished)
		cout << "�ش� �� �̸� ����" << endl;
}