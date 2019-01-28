#include <stdint.h>
#include <string>
#include <map>
#include <experimental/filesystem>
enum ACMStatus{
    LOADED=0b1,
    LUA=0b10,
    HAS_ASSETS=0b100,
};
struct ACMod
{
    uint8_t status;
//    uint16_t id;
    std::string name;
};
void initPluginSystem();
class PluginSystem{
public:
    static void loadPlugin(const std::experimental::filesystem::directory_entry &dirEntry);
    static std::vector<struct ACMod> mods;
    static std::map<std::string, size_t> modids;
};
