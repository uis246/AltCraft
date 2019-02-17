#include "ScriptingVM.hpp"
#include <string>
#include <experimental/filesystem>

class Mod{
public:
    Mod(std::string modid, ScriptingVM* lua);

    void loadFiles(std::experimental::filesystem::directory_entry dir);

    static void loadLuaFromFile(std::experimental::filesystem::directory_entry file, lua_State* lua);

    #pragma pack(push,1)
    #ifdef __GNUC__
    #define PACKED __attribute__ ((__packed__))
    #else
    #define PACKED
    #endif
    struct PACKED status{
        bool loaded=false;
        bool lua=false;
        bool native=false;
    } status;
    #pragma pack(pop)

    std::string modid;
    std::string name;

    ScriptingVM* vm;
};
