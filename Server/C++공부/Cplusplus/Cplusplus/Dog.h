#pragma once
#include "pch.h"

class Dog
{
public:
	Dog();
	Dog(int age, int weight);

private:
	int m_age;
	int m_weight;

public:
	int getAge();
	int getWeight();

	void setAge(unsigned int a);
	void setWeight(unsigned int a);

	void showInfo();
};

