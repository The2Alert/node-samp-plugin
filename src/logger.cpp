#include "logger.h"
#include <iostream>
#define SAMPGDK_AMALGAMATION
#include "sampgdk.h"
#include "rang.hpp"

namespace nodesamp {
    namespace Logger {
        void Init() {
            #ifdef _WIN32
            rang::rang_implementation::setWinTermAnsiColors(std::cout.rdbuf());
            #endif
        }
    
        void Error(std::string content) {
            sampgdk::logprintf("\u001b[33m\u001b[1m[NodeSamp]\u001b[22m\u001b[39m\u001b[31m\u001b[1m[Error]\u001b[22m\u001b[39m %s", content.c_str());
        }

        void Debug(std::string content) {
            sampgdk::logprintf("\u001b[33m\u001b[1m[NodeSamp]\u001b[22m\u001b[39m\u001b[34m\u001b[1m[Debug]\u001b[22m\u001b[39m %s", content.c_str());
        }

        namespace Config {
            void Error(std::string content) {
                sampgdk::logprintf("\u001b[33m\u001b[1m[Config]\u001b[22m\u001b[39m\u001b[31m\u001b[1m[Error]\u001b[22m\u001b[39m %s", content.c_str());
            }
        }
    }

    void Log(std::string content) {
        sampgdk::logprintf("\u001b[33m\u001b[1m[NodeSamp]\u001b[22m\u001b[39m %s", content.c_str());
    }
}