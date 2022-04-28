#include "config.h"
#include <fstream>
#include "exception.h"
#include "json.hpp"
#include "utils.h"

namespace nodesamp {
    ConfigException::ConfigException(std::string message): std::string(message) {}

    std::unique_ptr<Config> GetConfig() {
        using namespace nlohmann;
        std::ifstream configFile("./nodesamp-conf.json");
        if(configFile.is_open()) {
            std::string configJson;
            std::string line;
            while(std::getline(configFile, line))
                configJson += line;
            configFile.close();
            try {
                std::unique_ptr<Config> config(new Config);
                json configObject = json::parse(configJson);
                if(configObject["path"].is_string())
                    configObject["path"].get_to(config->path);
                std::string nodeOptions;
                if(configObject["nodeOptions"].is_string())
                    configObject["nodeOptions"].get_to(nodeOptions);
                config->nodeOptions = Utils::Split(nodeOptions, ' ');
                std::string nodeArgs;
                if(configObject["nodeArgs"].is_string())
                    configObject["nodeArgs"].get_to(nodeArgs);
                config->nodeArgs = Utils::Split(nodeArgs, ' ');
                return config;
            } catch(const json::exception& exception) {
                throw ConfigException(exception.what());
            }
        } else throw Exception("Configuration file not found.");
    }
}