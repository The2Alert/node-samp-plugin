#include "utils.h"

namespace nodesamp {
    namespace Utils {
        std::string ConvertCP1251ToUTF8(std::string source) {
            return cp2utf(source);
        }

        std::string ConvertUTF8ToCP1251(std::string source) {
            return utf2cp(source);
        }

        std::vector<std::string> Split(std::string source, char delimiter) {
            std::vector<std::string> result;
            std::string substring;
            for(char symbol: source) {
                if(symbol != delimiter)
                    substring += symbol;
                else {
                    result.push_back(substring);
                    substring = "";
                }
            }
            if(substring.length() != 0)
                result.push_back(substring);
            return result;
        }
    }
}