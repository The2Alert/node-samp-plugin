#ifndef NODESAMP_LOGGER_H
#define NODESAMP_LOGGER_H

#include <string>
#include <iostream>
#include "rang.hpp"

namespace nodesamp {
    void log(std::string content);

    void error(std::string error);
}

#endif