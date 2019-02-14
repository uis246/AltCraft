#include <stdint.h>
#include <string>
#include <map>
#include <experimental/filesystem>
extern "C" {
#include <luajit-2.0/luajit.h>
}
struct ACMod
{
    #pragma pack(push,1)
#ifdef __GNUC__
#define PACKED __attribute__ ((__packed__))
#else
#define PACKED
#endif
    struct PACKED ACMStatus
    {
        bool loaded=false;
        bool lua=false;
        bool assets=false;
    } status;
    #pragma pack(pop)
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
