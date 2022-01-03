#include "logger.h"

namespace nodesamp {
    void log(std::string content) {
        std::cout << rang::style::bold << rang::fg::yellow << "[NodeSamp]" << rang::style::reset;
        std::cout << " " << content << std::endl;
    }

    void error(std::string content) {
        std::cout << rang::style::bold << rang::fg::yellow << "[NodeSamp]" << rang::style::reset;
        std::cout << rang::style::bold << rang::fg::red << "[Error]" << rang::style::reset;
        std::cout << " " << content << std::endl;
    }
}