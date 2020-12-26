#include "pch.h"
#include "drawMoney.h"

void drawMoney(vector<Info*>& list)
{
	char searchID[20];
	int drawCash;
	bool isFinished = false;

	cout << "�� �̸� : ";
	cin >> searchID;

	cout << "��ݾ� : ";
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
				cout << "��� �Ϸ�! " << endl;
				isFinished = true;
				break;
			}
			else
				cout << "�ܾ� ����! " << endl;
		}


	}

	if (!isFinished)
		cout << "�ش� �� �̸� ����" << endl;
}