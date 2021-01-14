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