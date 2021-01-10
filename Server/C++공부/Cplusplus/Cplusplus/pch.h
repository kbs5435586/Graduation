#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct Info
{
	char name[20] = {};
	int account = 0;
	int cash = 0;
};

enum Color { COLOR_BLACK, COLOR_YELLOW, COLOR_BROWN, COLOR_RED, COLOR_END };