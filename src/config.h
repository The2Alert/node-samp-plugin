#ifndef NODESAMP_CONFIG_H
#define NODESAMP_CONFIG_H

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <exception>
#include "json.hpp"
#include "utils.h"

using json = nlohmann::json;

namespace nodesamp {
    struct Config {
        std::string packagePath;
        std::vector<std::string> nodeOptions;
    };

    std::unique_ptr<Config> GetConfig();
}

#endif