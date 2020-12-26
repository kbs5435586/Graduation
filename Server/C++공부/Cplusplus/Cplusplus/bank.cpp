#include "pch.h"
#include "insertInfo.h"
#include "insertMoney.h"
#include "drawMoney.h"
#include "showInfo.h"
#include "onlyVoid.h"

int main()
{
	vector<Info*> clients;

	while (1)
	{
		int select;
		showMenu();
		cin >> select;

		switch (select)
		{
		case 1:
			insertInfo(clients);
			break;

		case 2:
			insertMoney(clients);
			break;

		case 3:
			drawMoney(clients);
			break;
		case 4:
			showInfo(clients);
			break;
		case 5:
			exitProgram();
			break;
		}
		system("pause");
		system("cls");
	}
}
