// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

#include <regex>

#include "BodySanitizer.hpp"

std::string sanitizeBody(std::string bodyText) {
    return std::regex_replace(bodyText, std::regex{"\t"}, "    ");
}
