// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

#include "Common.hpp"
#include "MapFileReader.hpp"
#include "Regexes.hpp"

MapFileReader MapFileReader::fromIstream(std::istream & mapFile) {
    MapFileReader ret{};
    std::string curLineStr{};
    bool inSegments = false;
    while (mapFile.good()) {
        if (not std::getline(mapFile, curLineStr)) {
            break;
        }
        if (curLineStr.length() == 0) {
            inSegments = false;
            continue;
        }
        std::smatch m;
        if (std::regex_match(curLineStr, m, Re::mapFileSegmentHeader)) {
            inSegments = true;
        } else if (inSegments and std::regex_match(curLineStr, m, Re::mapFileSegmentLine)) {
            uint32_t start = std::stoi(m.str(2), nullptr, 16);
            uint32_t end = std::stoi(m.str(3), nullptr, 16);
            ret.segments_.emplace(std::piecewise_construct,
                std::forward_as_tuple(m.str(1)),
                std::forward_as_tuple(m.str(1), start, end));
        }
    }
    return ret;
}
