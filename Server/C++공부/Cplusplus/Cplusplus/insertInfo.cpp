#include "pch.h"
#include "insertInfo.h"

void insertInfo(vector<Info*>& list)
{
	Info* info = new Info;

	cout << "°í°´ ÀÌ¸§ : ";
	cin >> info->name;
	cout << "°èÁÂ ¹øÈ£ : ";
	cin >> info->account;
	cout << "°í°´ ÀÜ¾× : ";
	cin >> info->cash;

	list.push_back(info);
}