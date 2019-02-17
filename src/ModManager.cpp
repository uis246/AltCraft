#include <easylogging++.h>
#include <experimental/filesystem>
#include <nlohmann/json.hpp>

#include "platform.hpp"
#include "ModManager.hpp"

namespace fs = std::experimental::filesystem;

static void walkDirs(fs::directory_entry dir, void(*callback)(const fs::directory_entry dirEntry)){
    for (auto &file : fs::directory_iterator(dir)) {
        if(fs::is_directory(file)){
            callback(file);
        }
    }
}

std::map<std::string, Mod> ModManager::mods;

void ModManager::loadMod(fs::directory_entry dir){
    std::string info=dir.path().string().append(SEPARATOR"acmod.info");
    size_t sz=fs::file_size(info);

    void* inf=loadFile(info.c_str(), sz);

    nlohmann::json j=nlohmann::json::parse((char*)inf);
    UnloadFile(inf, sz);

    std::string modid=JSONGet("modid", j).get<std::string>();

    ScriptingVM* vm=new ScriptingVM();
    vm->loadACLA();
    Mod mod(modid, vm);
    LOG(INFO) << "Loading plugin: " << (mod.name=JSONGet("name", j).get<std::string>()).c_str();

    mod.loadFiles(dir);

    mods.insert(std::pair<std::string, Mod>(modid, mod));
}

void ModManager::initModSystem(){
    LOG(INFO) << "Initializing UPluginSystem";
    walkDirs(fs::directory_entry("./mods/"), ModManager::loadMod);
    LOG(INFO) << mods.size() << " plugin(s) loaded";
}
