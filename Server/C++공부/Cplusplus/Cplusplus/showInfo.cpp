#include "pch.h"
#include "showInfo.h"

void showInfo(vector<Info*>& list)
{
	for (auto& iter : list)
	{
		cout << "�� �̸� : " << iter->name << endl;
		cout << "���� ��ȣ : " << iter->account << endl;
		cout << "�� �ܾ� : " << iter->cash << endl << endl;
	}
}