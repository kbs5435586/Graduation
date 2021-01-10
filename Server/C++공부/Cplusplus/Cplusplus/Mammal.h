#pragma once
#include "pch.h"

class Mammal
{
public:
	Mammal();
	Mammal(int id, int age, int weight);
	~Mammal();

private:
	//const int m_id;
	int m_id;
	int m_age;
	int m_weight;

public:
	int getAge() const;
	int getWeight() const;

	void setAge(unsigned int a);
	void setWeight(unsigned int a);

	void showInfo();
};

