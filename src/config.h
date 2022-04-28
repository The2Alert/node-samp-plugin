#ifndef NODESAMP_CONFIG_H
#define NODESAMP_CONFIG_H

#include <vector>
#include <string>
#include <memory>

namespace nodesamp {
    struct Config {
        std::vector<std::string> nodeOptions;
        std::string path;
        std::vector<std::string> nodeArgs;
    };

    class ConfigException: public std::string {
        public:
            ConfigException(std::string message);
    };

    std::unique_ptr<Config> GetConfig();
}

#endif