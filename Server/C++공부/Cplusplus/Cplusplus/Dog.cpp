#include "Dog.h"

Dog::Dog()
{
	m_age = 3;
	m_weight = 10;
}

Dog::Dog(int age, int weight)
{
	m_age = age;
	m_weight = weight;
}

int Dog::getAge()
{
	return m_age;
}

int Dog::getWeight()
{
	return m_weight;
}

void Dog::setAge(unsigned int a)
{
	m_age = a;
}

void Dog::setWeight(unsigned int a)
{
	m_weight = a;
}

void Dog::showInfo()
{
	cout << "age : " << m_age << endl;
	cout << "weight : " << m_weight << endl;
}
