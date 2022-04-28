#include "utils.h"
#include <sstream>

namespace nodesamp {
    namespace Utils {
        std::vector<std::string> Split(std::string source, char delimiter) {
            std::vector<std::string> result;
            std::stringstream stream(source);
            std::string chunk;
            while(std::getline(stream, chunk, delimiter))
                result.push_back(chunk);
            return result;
        }
    }
}