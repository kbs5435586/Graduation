#include <stdio.h> 

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h" 
}

int main(void)
{
	int rows, cols;

	lua_State* L = luaL_newstate(); //��Ƹ� ��
	luaL_openlibs(L); //��� ǥ�� ���̺귯���� ��

	luaL_loadfile(L, "ex1.lua");
	lua_pcall(L, 0, 0, 0);

	lua_getglobal(L, "rows"); 
	lua_getglobal(L, "cols");

	rows = (int)lua_tonumber(L, -2); 
	cols = (int)lua_tonumber(L, -1);

	printf("Rows %d, Cols %d\n", rows, cols);
	lua_pop(L, 2);

	lua_close(L); 
	return 0;
}