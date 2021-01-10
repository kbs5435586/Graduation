#include "pch.h"
#include "Dog.h"
#include "Cat.h"

void showData(Mammal &ref);

int main()
{
	//Mammal Happy(1004);
	//Mammal Puffy(1005, 4, 15);
	Dog Happy;
	Cat Leo;

	Mammal* Luffy = new Dog;

	//Leo.showInfo();
	//Happy.showInfo();
	//Puffy.showInfo();
	//Luffy->showInfo();
	showData(*Luffy);

	delete Luffy;
	return 0;
}

void showData(Mammal &ref)
{
	ref.showInfo();
}