#include "logger.h"

namespace nodesamp {
    void Log(std::string content) {
        std::cout << rang::style::bold << rang::fg::yellow << "[NodeSamp]" << rang::style::reset;
        std::cout << " " << content << std::endl;
    }

    void Error(std::string content) {
        std::cout << rang::style::bold << rang::fg::yellow << "[NodeSamp]" << rang::style::reset;
        std::cout << rang::style::bold << rang::fg::red << "[Error]" << rang::style::reset;
        std::cout << " " << content << std::endl;
    }
}