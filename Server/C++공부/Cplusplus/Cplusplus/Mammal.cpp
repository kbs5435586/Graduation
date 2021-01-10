#include "Mammal.h"

//Dog::Dog(int id) :m_id(id)
//{
//	m_age = 3;
//	m_weight = 10;
//}
//
//Dog::Dog(int id, int age, int weight) :m_id(id)
//{
//	m_age = age;
//	m_weight = weight;
//}

Mammal::Mammal()
{
	cout << "Mammal 持失切" << endl;
	m_id = 1004;
	m_age = 3;
	m_weight = 10;
}

Mammal::Mammal(int id, int age, int weight)
{
	cout << "Mammal 持失切" << endl;
	m_id = id;
	m_age = age;
	m_weight = weight;
}

Mammal::~Mammal()
{
	cout << "Mammal 社瑚切" << endl;
}

int Mammal::getAge() const
{
	return m_age;
}

int Mammal::getWeight() const
{
	return m_weight;
}

void Mammal::setAge(unsigned int a)
{
	m_age = a;
}

void Mammal::setWeight(unsigned int a)
{
	m_weight = a;
}

void Mammal::showInfo()
{
	cout << "id : " << m_id << endl;
	cout << "age : " << m_age << endl;
	cout << "weight : " << m_weight << endl;
}
