#include "Cat.h"

Cat::Cat()
{
	cout << "Cat 积己磊" << endl;
}

Cat::~Cat()
{
	cout << "Cat 家戈磊" << endl;
}

Color Cat::getColor()
{
	return m_color;
}

void Cat::setColor(Color color)
{
	m_color = color;
}

void Cat::showSound()
{
	cout << "具克~" << endl;
}
