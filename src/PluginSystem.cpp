#include <stdbool.h>
#include <stdio.h>
#include <nlohmann/json.hpp>
#include <easylogging++.h>

extern "C"{
#include <luajit-2.0/lauxlib.h>
#include <luajit-2.0/lualib.h>
}

#include "PluginSystem.hpp"

#ifdef _POSIX_C_SOURCE
#include <fcntl.h>
#include <sys/mman.h>
#ifndef USE_MMAP
#define USE_MMAP 1
#endif
#else
#include <memory.h>
#define USE_MMAP 0
#endif

#ifdef WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

#ifdef __GNUC__
#warning NOT THREADSAFE
#else
#pragma message ("Warning: NOT THREADSAFE")
#endif

#if VERBOSE_PLUGIN_LOADING
#define VERBOSE_JSON_PARSING 1
#define VERBOSE_LUA_LOADING 1
#endif

namespace fs = std::experimental::filesystem;

//LUA API
//AltCraft Lua API
static int ACLA_log(lua_State* L){
//    int lvl=luaL_checkinteger(L, 1);
    const char* str=luaL_checkstring(L, 1);
    LOG(INFO) << str;
    return 0;
}
//Lua decls
static const struct luaL_reg ACLA [] = {
{"log", ACLA_log},
{NULL, NULL}
};
//End of decls
//End of LUA API

// /pugin
// /pugin/info.json
// /pugin/src/*.lua
static void loadLua(const fs::directory_entry &file, lua_State* lua){
    std::string path=file.path();
    size_t len=path.length();
    const char* str=path.c_str();
    if(str[len-4]=='.'&&str[len-3]=='l'&&str[len-2]=='u'&&str[len-1]=='a'){
#if VERBOSE_LUA_LOADING
        LOG(INFO) << "Loading lua file: " << str;
#endif
//        luaL_loadfile(lua, str);
        if(luaL_dofile(lua, str)){
            LOG(FATAL) << "Failed to load file "+file.path().string();
        }
    }
}
static void walkDir(const fs::directory_entry &dirEntry, void(*callback)(const fs::directory_entry &dirEntry, lua_State* lua), lua_State* lua){
if(lua){
    for (auto &file : fs::directory_iterator(dirEntry)) {
        if(fs::is_directory(file)){
            walkDir(dirEntry, callback, lua);
        }else{
            callback(file, lua);
        }
    }
}else{
    for (auto &file : fs::directory_iterator(dirEntry)) {
        if(fs::is_directory(file)){
            callback(file, nullptr);
        }
    }
}
}

void* loadFile(const char* fname, size_t size){
    FILE* f=fopen(fname, "rb");
    if(f==NULL){abort();}
    void* p;
#ifdef USE_MMAP
    p=mmap(NULL, size, PROT_READ, MAP_SHARED, fileno(f), 0);
#else
    p=malloc(size);
    fread(p, 1, size, f);
#endif
    fclose(f);
    return p;
}

#if USE_MMAP
#define UnloadFile(ptr, size) unloadFile(ptr, size)
inline void unloadFile(void* ptr, size_t size){
    munmap(ptr, size);
#else
#define UnloadFile(ptr, size) unloadFile(ptr)
inline void unloadFile(void* ptr){
    free(ptr);
#endif
}

std::vector<struct ACMod> PluginSystem::mods;
std::map<std::string, size_t> PluginSystem::modids;
inline auto JSONGet(std::string a, nlohmann::json j){
#if VERBOSE_JSON_PARSING
    auto ref=j[a];
    if(ref==nullptr){
        LOG(FATAL)<<"No \""+a+"\" field";
    }
    return ref;
#else
    return j[a];
#endif
}

void PluginSystem::loadPlugin(const fs::directory_entry &dirEntry, lua_State* lua) {
    std::string info=dirEntry.path().string().append(SEPARATOR"acmod.info");
    size_t sz=fs::file_size(info);
    struct ACMod acm;

    void* inf=loadFile(info.c_str(), sz);

    nlohmann::json j=nlohmann::json::parse((char*)inf);
    UnloadFile(inf, sz);

    std::string modid=JSONGet("modid", j).get<std::string>();
    LOG(INFO) << "Loading plugin: " << (acm.name=JSONGet("name", j).get<std::string>()).c_str();
    if(JSONGet("type", j).get<std::string>()=="lua"){
        if(lua==nullptr){
            acm.lua=lua_open();
            luaL_openlibs(acm.lua);
        }else{
            acm.lua=lua;
        }
        luaL_openlib(acm.lua, "ACLA", ACLA, 0);
        walkDir(dirEntry, loadLua, acm.lua);
        lua_getglobal(acm.lua, "UACPluginInit");
        if(lua_pcall(acm.lua, 0, 0, 0)){
            LOG(FATAL) << "Error while initializing plugin " << acm.name;
        }
        acm.status.lua=true;
    }else{
        acm.lua=nullptr;
    }
    acm.status.loaded=true;
    modids.insert(std::pair<std::string, size_t>(modid, mods.size()));
    mods.push_back(acm);
}
void initPluginSystem(){
    LOG(INFO) << "Initializing UPluginSystem";
    walkDir(fs::directory_entry("./mods/"), PluginSystem::loadPlugin, nullptr);
    LOG(INFO) << PluginSystem::mods.size() << " plugin(s) loaded";
}
