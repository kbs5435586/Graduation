#include "pch.h"
#include "onlyVoid.h"

void exitProgram()
{
	exit(0);
}

void showMenu()
{
	cout << "========== Menu ==========" << endl;
	cout << "1. 계좌개설 " << endl;
	cout << "2. 입금 " << endl;
	cout << "3. 출금 " << endl;
	cout << "4. 계좌정보 전체 출력 " << endl;
	cout << "5. 프로그램 종료 " << endl << endl;
	cout << "선택: ";
}
