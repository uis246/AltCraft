#include <stdbool.h>
#include <stdio.h>
#include <nlohmann/json.hpp>
#include <easylogging++.h>

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
#warning "NOT THREADSAFE"
#else
#pragma message ("Warning: NOT THREADSAFE")
#endif

namespace fs = std::experimental::filesystem;

// /pugin
// /pugin/info.json
// /pugin/src/*.lua
inline bool isLua(std::string a){
    size_t len=a.length();
    const char* str=a.c_str();
    return str[len-4]=='.'&&str[len-3]=='l'&&str[len-2]=='u'&&str[len-1]=='a';
}
static void walkDir(const fs::directory_entry &dirEntry, void(*callback)(const fs::directory_entry &dirEntry), bool onFile){
for (auto &file : fs::directory_iterator(dirEntry)) {
    if(onFile){
        if(fs::is_directory(file)){
            walkDir(dirEntry, callback, true);
        }else{
            if(isLua(file.path()))
                callback(file);
        }
    }else{
        if(fs::is_directory(file)){
            callback(file);
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
static inline auto JSONGet(std::string a, nlohmann::json j){
#if VERBOSE_PLUGIN_LOADING
    auto ref=j[a];
    if(ref==nullptr){
        LOG(FATAL)<<"No \""+a+"\" field in "+fs::absolute(info).string();
    }
    return ref;
#else
    return j[a];
#endif
}
void PluginSystem::loadPlugin(const fs::directory_entry &dirEntry) {
    std::string info=dirEntry.path().string().append(SEPARATOR"acmod.info");
    size_t sz=fs::file_size(info);
    struct ACMod acm;

    void* inf=loadFile(info.c_str(), sz);

    nlohmann::json j=nlohmann::json::parse((char*)inf);
    UnloadFile(inf, sz);

    acm.name=JSONGet("name", j).get<std::string>();
    acm.status=0b1;
    modids.insert(std::pair<std::string, size_t>(JSONGet("modid", j).get<std::string>(), mods.size()));
    mods.push_back(acm);
}
void initPluginSystem(){
    walkDir(fs::directory_entry("./mods/"), PluginSystem::loadPlugin, false);
    LOG(INFO) << PluginSystem::mods.size() << " plugins loaded";
}
