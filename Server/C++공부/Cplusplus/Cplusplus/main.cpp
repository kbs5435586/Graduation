#include "pch.h"
#include "Dog.h"

int main()
{
	Dog Happy;
	Dog Puffy(4, 15);

	Happy.showInfo();
	Puffy.showInfo();
	return 0;
}