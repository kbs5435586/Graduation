#include <iostream> 

extern "C" {
#include "lua.h" 
#include "lauxlib.h" 
#include "lualib.h" 
}

#pragma comment (lib, "lua54.lib")

using namespace std;

int main() {
	const char* buff = "print \"Hello from Lua.\"\n";

	lua_State* L = luaL_newstate();
	luaL_openlibs(L); 
	luaL_loadbuffer(L, buff, strlen(buff), "line"); 
	int error = lua_pcall(L, 0, 0, 0);
	
	if (error)
	{
		cout << "Error:" << lua_tostring(L, -1); 
		lua_pop(L, 1);
	} 
	lua_close(L);
	return 0;
}