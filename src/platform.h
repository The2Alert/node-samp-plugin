#ifndef NODESAMP_PLATFORM_H
#define NODESAMP_PLATFORM_H

#include <vector>
#include <string>
#include <memory>
#include "v8.h"
#include "node.h"
#include "module.h"

namespace nodesamp {
    class Platform {
        public:
            typedef std::vector<std::string> Args;
        private:
            Args args;
            Args execArgs;
            std::unique_ptr<node::MultiIsolatePlatform> platform;
            std::unique_ptr<Module> module;
        public:
            Platform(std::string packagePath, const std::vector<std::string>& options);

            void init();
            Args& getArgs();
            Args& getExecArgs();
            node::MultiIsolatePlatform* getPlatform();
            void initModule();
            Module* getModule();
            void uninitModule();
            void tick();
            void uninit();
    };

    std::unique_ptr<Platform> CreatePlatform(std::string packagePath, const std::vector<std::string>& options);
}

#endif