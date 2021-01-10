#include "Dog.h"

Dog::Dog()
{
	cout << "Dog 积己磊" << endl;
}

Dog::~Dog()
{
	cout << "Dog 家戈磊" << endl;
}

Color Dog::getColor()
{
	return m_color;
}

void Dog::setColor(Color color)
{
	m_color = color;
}

void Dog::showInfo()
{
	cout << "This is dog info!" << endl;
}

void Dog::showSound()
{
	cout << "港港!" << endl;
}
