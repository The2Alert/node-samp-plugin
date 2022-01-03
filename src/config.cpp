#include "config.h"

namespace nodesamp {
    std::unique_ptr<Config> GetConfig() {
        std::ifstream cFile("./nodesamp-conf.json");
        if(cFile.is_open()) {
            std::string cJson;
            std::string cLine;
            while(std::getline(cFile, cLine))
                cJson += cLine;
            cFile.close();
            std::unique_ptr<Config> config(new Config);
            json cObject = json::parse(cJson);
            cObject["packagePath"].get_to(config->packagePath);
            std::string nodeOptions;
            cObject["nodeOptions"].get_to(nodeOptions);
            config->nodeOptions = Utils::Split(nodeOptions, ' ');
            return config;
        } else throw std::exception("Configuration file not found.");
    }
}