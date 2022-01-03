#ifndef NODESAMP_UTILS_H
#define NODESAMP_UTILS_H

#include <string>
#include <vector>
#include "iconvlite.h"

namespace nodesamp {
    namespace Utils {
        std::string ConvertCP1251ToUTF8(std::string source);
        std::string ConvertUTF8ToCP1251(std::string source);
        std::vector<std::string> Split(std::string source, char delimiter);
    }
}

#endif