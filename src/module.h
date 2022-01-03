#ifndef NODESAMP_MODULE_H
#define NODESAMP_MODULE_H

#include <stdlib.h>
#include <string>
#include <memory>
#include <vector>
#include "v8.h"
#include "node.h"
#include "uv.h"
#include "amx.h"

namespace nodesamp {
    class Platform;

    class Module {
        private:
            Platform* platform;
            std::unique_ptr<node::CommonEnvironmentSetup> setup;
            std::unique_ptr<Amx> amx;
        public:
            Module(Platform* platform);

            void init();
            node::CommonEnvironmentSetup* getSetup();
            Amx* getAmx();
            void tick();
            void uninit();
    };
}

#endif