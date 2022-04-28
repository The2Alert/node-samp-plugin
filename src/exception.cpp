#include "exception.h"

namespace nodesamp {
    Exception::Exception(std::string message): std::string(message) {}
}