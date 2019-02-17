extern "C"{
#include <luajit-2.0/luajit.h>
#include <luajit-2.0/lauxlib.h>
}

class ScriptingVM
{
public:
    ScriptingVM();
    ScriptingVM(lua_State* lua);

    void loadACLA();

//    void loadLua(std::experimental::filesystem::directory_entry file);

    lua_State* lua;
};
