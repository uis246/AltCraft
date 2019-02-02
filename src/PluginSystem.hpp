#include <stdint.h>
#include <string>
#include <map>
#include <experimental/filesystem>
extern "C" {
#include <luajit-2.0/luajit.h>
}
enum ACMStatus{
    LOADED=0b1,
    LUA=0b10,
    HAS_ASSETS=0b100,
    NATIVE=0b1000
};
struct ACMod
{
    uint8_t status;
//    uint16_t id;
    std::string name;
    lua_State* lua;
};
void initPluginSystem();
class PluginSystem{
public:
    static void loadPlugin(const std::experimental::filesystem::directory_entry &dirEntry, lua_State* l);
    static std::vector<struct ACMod> mods;
    static std::map<std::string, size_t> modids;
};
