#ifndef NODESAMP_PLATFORM_H
#define NODESAMP_PLATFORM_H

#include <string>
#include <vector>
#include <memory>
#include "v8.h"
#include "node.h"
#include "env.h"
#include <malloc.h>
#include "amx/amx.h"

namespace nodesamp {
    class Platform {
        friend std::unique_ptr<Platform> InitPlatform(const std::vector<std::string>& options, std::string path, const std::vector<std::string>& args);
        friend class Environment;
        private:
            std::string path;
            std::vector<std::string> args;
            std::unique_ptr<node::MultiIsolatePlatform> platform;
            std::unique_ptr<Environment> defaultEnv;

            Platform(const std::vector<std::string>& options, std::string path, const std::vector<std::string>& args);
    
            void init();
        public:
            Environment* loadDefaultEnv();
            Environment* getDefaultEnv();
            void stopDefaultEnv();
            void tick();
            void callAmxPublic(AMX* amx, std::string name, cell* params, cell* retval);
            void uninit();
    };

    std::unique_ptr<Platform> InitPlatform(const std::vector<std::string>& options, std::string path, const std::vector<std::string>& args);
}

#endif