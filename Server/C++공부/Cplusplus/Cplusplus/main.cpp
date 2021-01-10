#include "pch.h"
#include "Dog.h"

int main()
{
	//Mammal Happy(1004);
	Mammal Puffy(1005, 4, 15);
	Dog Happy;
	Mammal* Luffy = new Mammal(1004, 5, 20);

	Happy.showInfo();
	//Puffy.showInfo();
	//Luffy->showInfo();

	delete Luffy;
	return 0;
}