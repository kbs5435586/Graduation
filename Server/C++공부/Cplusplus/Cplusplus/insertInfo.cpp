#include "pch.h"
#include "insertInfo.h"

void insertInfo(vector<Info*>& list)
{
	Info* info = new Info;

	cout << "�� �̸� : ";
	cin >> info->name;
	cout << "���� ��ȣ : ";
	cin >> info->account;
	cout << "�� �ܾ� : ";
	cin >> info->cash;

	list.push_back(info);
}