#include "ScriptingVM.hpp"
#include "ACLA.h"

ScriptingVM::ScriptingVM(){
    lua_State* l=luaL_newstate();
    luaL_openlibs(l);
    lua=l;
}
ScriptingVM::ScriptingVM(lua_State* l){
    lua=l;
}
void ScriptingVM::loadACLA(){
    luaL_openlib(lua, "ACLA", ACLA, 0);
}
