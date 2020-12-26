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

	//auto iter = find(list.begin(), list.end(), [searchID](Info* info)
	//{
	//	if (strcmp(info->name, searchID) == 0)
	//	{
	//		return true;
	//	}
	//});

	//if (iter != list.end())
	//{
	//	int i = 0;
	//}
	//else
	//{
	//	int i = 0;
	//}

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