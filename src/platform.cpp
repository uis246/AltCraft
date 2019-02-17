#include <easylogging++.h>

#include "platform.hpp"

nlohmann::json JSONGet(std::string a, nlohmann::json j){
    auto ref=j[a];
    if(ref==nullptr){
        LOG(FATAL)<<"No \""+a+"\" field";
    }
    return ref;
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
