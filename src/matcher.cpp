#include <format>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "exceptions.hpp"
#include "matcher.hpp"

#if !NOT_VSCODE
#define not !
#define and &&
#define or ||
#endif

// We don't care about printing out the listings for these files.
static const std::regex forbiddenIncludes{
    "(?:"
    "structs|eventmacros|config|constants|enums|hardware|bin|common|"
    "macros|textmacros|symbols"
    ").*"
};

static const std::regex lineRe{
    R"/(^([0-9A-F]{6}). \d+ +((?:[0-9A-F][0-9A-F] |rr |   ){4}) (.*)$)/"
};

static const std::regex byteOrRelocationRe{
    R"/((rr)|([0-9A-F][0-9A-F]) )/"
};

static const std::regex includeBodyRe{
    R"/(\.INCLUDE "([^"]+)")/"
};



class ListingLine {
    using CodeBytes = std::vector<std::optional<uint8_t>>;
    CodeBytes codeBytes_;
    std::string bodyText_;
    uint32_t codeAddress_;
    explicit ListingLine(uint32_t codeAddress, CodeBytes && codeBytes, std::string && bodyText)
        : codeBytes_(std::move(codeBytes)), bodyText_(std::move(bodyText)), codeAddress_(codeAddress) {}
 
public:
    const CodeBytes & codeBytes() const { return codeBytes_; }
    const std::string & bodyText() const { return bodyText_; }
    uint32_t codeAddress() const { return codeAddress_; }
    static std::optional<ListingLine> fromString(uint8_t bank, std::string line) {
        std::smatch m;
        if (not std::regex_match(line, m, lineRe)) {
            return {};
        }
        // Get codeAddress
        int offset = std::stoi(m[1], nullptr, 16);
        if (offset < 0 or offset >= 65536) {
            throw malformed_listing("Line offset is out of range");
        }
        uint32_t codeAddress = offset + (bank << 16);
        // Get codeBytes
        CodeBytes codeBytes{};
        auto byteIt = std::sregex_iterator(m[2].first, m[2].second, byteOrRelocationRe);
        for (; byteIt != std::sregex_iterator(); ++byteIt) {
            if (byteIt->str(1).length()) {
                codeBytes.emplace_back();
            } else {
                uint8_t byteValue = std::stoi(byteIt->str(2), nullptr, 16);
                codeBytes.emplace_back(byteValue);
            }
        }
        // Get bodyText
        std::string bodyText = m.str(3);
        return ListingLine(codeAddress, std::move(codeBytes), std::move(bodyText));
    }
    std::string toString() const {
        return std::format("{:06x}: {} {} {} {} {}",
            codeAddress_,
            codeBytes_.size() > 0 ? (codeBytes_[0].has_value() ? std::format("{:02x}", codeBytes_[0].value()) : "??" ) : "  ",
            codeBytes_.size() > 1 ? (codeBytes_[1].has_value() ? std::format("{:02x}", codeBytes_[1].value()) : "??" ) : "  ",
            codeBytes_.size() > 2 ? (codeBytes_[2].has_value() ? std::format("{:02x}", codeBytes_[2].value()) : "??" ) : "  ",
            codeBytes_.size() > 3 ? (codeBytes_[3].has_value() ? std::format("{:02x}", codeBytes_[3].value()) : "??" ) : "  ",
            bodyText_
        );
    }
};

class ListingMatcher : public IListingMatcher {
    template <typename T>
    static inline bool _shouldOutputIncludedFile(T includedFilename) {
        // We should output the file if it doesn't match one of the forbidden
        // includes.
        return not std::regex_match(includedFilename, forbiddenIncludes);
    }
public:
    virtual void processListing(uint8_t bank, std::istream & listing){
        std::optional<ListingLine> lastLine{};
        std::optional<ListingLine> curLine{};
        std::string curLineStr{};
        std::string curFile{};
        while (listing.good()) {
            if (not std::getline(listing, curLineStr)) {
                throw malformed_listing("Can't getline");
            }
            curLine = ListingLine::fromString(bank, curLineStr);
            if (not curLine.has_value()) {
                // Unknown line - not an error (the file starts with non-format lines).
                // Keep going.
                continue;
            }
            // Check if this line is an include stmt.
            std::smatch includeReMatch;
            if (std::regex_match(curLine->bodyText(), includeReMatch, includeBodyRe)) {
                if (_shouldOutputIncludedFile(includeReMatch.str(1))) {
                    curFile = includeReMatch.str(1);
                    continue;
                }
            }
            // If we don't have a filename by this point, we don't care about this line.
            if (curFile.empty()) {
                continue;
            }
            // TODO: do stuff
            std::cout << std::format("{:60} | ", curFile) << curLine->toString() << std::endl;
        }
    }
};

std::unique_ptr<IListingMatcher> createListingMatcher() {
    return std::make_unique<ListingMatcher>();
}
