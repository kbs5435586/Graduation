#include "pch.h"
#include "drawMoney.h"

void drawMoney(vector<Info*>& list)
{
	char searchID[20];
	int drawCash;
	bool isFinished = false;

	cout << "고객 이름 : ";
	cin >> searchID;

	cout << "출금액 : ";
	cin >> drawCash;

	for (auto& iter : list)
	{
		if (strcmp(iter->name, searchID) == 0)
		{
			if (iter->cash > drawCash)
			{
				iter->cash -= drawCash;
				cout << "출금 완료! " << endl;
				isFinished = true;
				break;
			}
			else
				cout << "잔액 부족! " << endl;
		}


	}

	if (!isFinished)
		cout << "해당 고객 이름 없음" << endl;
}