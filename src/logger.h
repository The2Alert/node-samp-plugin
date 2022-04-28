#ifndef NODESAMP_LOGGER_H
#define NODESAMP_LOGGER_H

#include <string>

namespace nodesamp {
    namespace Logger {
        void Init();

        void Error(std::string content);

        void Debug(std::string content);

        namespace Config {
            void Error(std::string content);
        }
    }

    void Log(std::string content);
}

#endif