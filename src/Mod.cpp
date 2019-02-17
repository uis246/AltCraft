#include "Mod.hpp"

#include <luajit-2.0/lualib.h>
#include <easylogging++.h>

namespace fs = std::experimental::filesystem;

__inline void walkFiles(fs::directory_entry dir, void(*callback)(const fs::directory_entry dirEntry, lua_State* ptr), lua_State* ptr){
    for (auto &file : fs::directory_iterator(dir)) {
        if(fs::is_directory(file)){
            walkFiles(file, callback, ptr);
        }else{
            callback(file, ptr);
        }
    }
}
void Mod::loadLuaFromFile(fs::directory_entry file, lua_State* lua){
    //Is ends on .lua
    std::string path=file.path();
    size_t len=path.length();
    const char* str=path.c_str();
    if(str[len-4]=='.'&&str[len-3]=='l'&&str[len-2]=='u'&&str[len-1]=='a'){
        if(luaL_dofile(lua, str)){
            LOG(FATAL) << "Failed to load file "+path;
        }
    }
}
void Mod::loadFiles(fs::directory_entry dir){
    if(status.lua){
        walkFiles(dir, loadLuaFromFile, vm->lua);
    }
}
Mod::Mod(std::string id, ScriptingVM* lua){
    id=id;
    if(lua){
        vm=lua;
        status.lua=true;
    }
}
