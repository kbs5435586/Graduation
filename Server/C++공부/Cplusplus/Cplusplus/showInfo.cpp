#include "pch.h"
#include "showInfo.h"

void showInfo(vector<Info*>& list)
{
	for (auto& iter : list)
	{
		cout << "°í°´ ÀÌ¸§ : " << iter->name << endl;
		cout << "°èÁÂ ¹øÈ£ : " << iter->account << endl;
		cout << "°í°´ ÀÜ¾× : " << iter->cash << endl << endl;
	}
}