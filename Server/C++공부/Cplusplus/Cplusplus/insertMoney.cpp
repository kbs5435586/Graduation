#include "pch.h"
#include "insertMoney.h"

void insertMoney(vector<Info*>& list)
{
	char searchID[20];
	int insertCash;
	bool isFinished = false;

	cout << "고객 이름 : ";
	cin >> searchID;

	cout << "입금액 : ";
	cin >> insertCash;

	for (auto& iter : list)
	{
		if (strcmp(iter->name, searchID) == 0)
		{
			iter->cash += insertCash;
			cout << "입금 완료! " << endl;
			isFinished = true;
			break;
		}
	}

	if (!isFinished)
		cout << "해당 고객 이름 없음" << endl;
}