#include <map>
#include <string>

#include "Mod.hpp"

class ModManager{
public:
    static void initModSystem();
    static void loadMod(std::experimental::filesystem::directory_entry dir);

private:
    static std::map<std::string, Mod> mods;
};
