#include "Dog.h"

Dog::Dog()
{
	cout << "Dog ������" << endl;
}

Dog::~Dog()
{
	cout << "Dog �Ҹ���" << endl;
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
	cout << "�۸�!" << endl;
}
