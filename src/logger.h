#ifndef NODESAMP_LOGGER_H
#define NODESAMP_LOGGER_H

#include <string>
#include <iostream>
#include "rang.hpp"

namespace nodesamp {
    void Log(std::string content);

    void Error(std::string error);
}

#endif