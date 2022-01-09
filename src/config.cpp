#include "config.h"

namespace nodesamp {
    std::unique_ptr<Config> GetConfig() {
        std::ifstream configFile("./nodesamp-conf.json");
        if(configFile.is_open()) {
            std::string configJson;
            std::string line;
            while(std::getline(configFile, line))
                configJson += line;
            configFile.close();
            std::unique_ptr<Config> config(new Config);
            json configObject = json::parse(configJson);
            configObject["packagePath"].get_to(config->packagePath);
            std::string nodeOptions;
            configObject["nodeOptions"].get_to(nodeOptions);
            config->nodeOptions = Utils::Split(nodeOptions, ' ');
            return config;
        } else throw Exception("Configuration file not found.");
    }
}