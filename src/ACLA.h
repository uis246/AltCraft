#include "easylogging++.h"
extern "C"{
#include <luajit-2.0/luajit.h>
#include <luajit-2.0/lualib.h>
}

static int ACLA_log(lua_State* lua){
    const char* str=luaL_checkstring(lua, 1);
    LOG(INFO) << str;
    return 0;
}

static const struct luaL_reg ACLA [] = {
{"log", ACLA_log},
{NULL, NULL}
};
