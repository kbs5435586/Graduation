#include "Cat.h"

Cat::Cat()
{
	cout << "Cat ������" << endl;
}

Cat::~Cat()
{
	cout << "Cat �Ҹ���" << endl;
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
	cout << "�߿�~" << endl;
}
