#include "Mammal.h"
class Dog : public Mammal
{
public:
	Dog();
	~Dog();

private:
	Color m_color;

public:
	Color getColor();
	void setColor(Color color);
};
