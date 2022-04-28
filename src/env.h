#ifndef NODESAMP_ENV_H
#define NODESAMP_ENV_H

#include <string>
#include <memory>
#include "node.h"
#include <malloc.h>
#include "amx/amx.h"

namespace nodesamp {
    class Platform;

    class Environment {
        friend class Platform;
        private:
            Platform* platform;
            std::string source;
            std::unique_ptr<node::CommonEnvironmentSetup> setup;
            bool exited = false;

            Environment(Platform* platform, std::string source);

            void load();
            void tick();
            void callAmxPublic(AMX* amx, std::string name, cell* params, cell* retval);
            void exit();
            void stop();
    };   
}

#endif