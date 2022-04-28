#ifndef NODESAMP_EXCEPTION_H
#define NODESAMP_EXCEPTION_H

#include <string>

namespace nodesamp {
    class Exception: public std::string {
        public:
            Exception(std::string message);
    };
}

#endif