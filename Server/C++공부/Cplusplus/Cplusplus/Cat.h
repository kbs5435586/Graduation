#pragma once
#include "Mammal.h"
class Cat : public Mammal
{
public:
	Cat();
	~Cat();

private:
	Color m_color;

public:
	Color getColor();
	void setColor(Color color);
	void showInfo() override;
	void showSound() override;
};

