#include <nlohmann/json.hpp>

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

#if VERBOSE_PLUGIN_LOADING
#define VERBOSE_JSON_PARSING 1
#define VERBOSE_LUA_LOADING 1
#endif

void* loadFile(const char* fname, size_t size);

#if USE_MMAP
#define UnloadFile(ptr, size) unloadFile(ptr, size)
__inline void unloadFile(void* ptr, size_t size){
    munmap(ptr, size);
#else
#define UnloadFile(ptr, size) unloadFile(ptr)
__inline void unloadFile(void* ptr){
    free(ptr);
#endif
}
#define VERBOSE_JSON_PARSING 1
#if VERBOSE_JSON_PARSING
nlohmann::json JSONGet(std::string a, nlohmann::json j);
#else
__inline auto JSONGet(std::string a, nlohmann::json j){
    return j[a];
}
#endif
