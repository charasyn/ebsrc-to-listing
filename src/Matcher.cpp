#include <format>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "BodySanitizer.hpp"
#include "Exceptions.hpp"
#include "Matcher.hpp"
#include "Regexes.hpp"

template <class T>
inline std::optional<std::smatch> matchRegex(std::regex regex, T str) {
    std::smatch ret;
    if (std::regex_match(str, ret, regex)) {
        return ret;
    }
    return {};
}

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
        std::string bodyText = sanitizeBody(m.str(3));
        return ListingLine(codeAddress, std::move(codeBytes), std::move(bodyText));
    }
    std::string toString() const {
        return std::format("{:06X}: {} {} {} {} {} {}",
            codeAddress_,
            codeBytes_.size() > 0 ? (codeBytes_[0].has_value() ? std::format("{:02X}", codeBytes_[0].value()) : "??" ) : "  ",
            codeBytes_.size() > 1 ? (codeBytes_[1].has_value() ? std::format("{:02X}", codeBytes_[1].value()) : "??" ) : "  ",
            codeBytes_.size() > 2 ? (codeBytes_[2].has_value() ? std::format("{:02X}", codeBytes_[2].value()) : "??" ) : "  ",
            codeBytes_.size() > 3 ? (codeBytes_[3].has_value() ? std::format("{:02X}", codeBytes_[3].value()) : "??" ) : "  ",
            codeBytes_.size() > 4 ? "..." : "   ",
            bodyText_
        );
    }
    void updateRelocations(const EbRom & rom) {
        for (std::size_t offset = 0; offset < codeBytes_.size(); offset += 1) {
            if (codeBytes_[offset].has_value()) {
                continue;
            }
            uint32_t effectiveAddress = codeAddress_ + offset;
            uint8_t data = rom.readSnesAddr(effectiveAddress);
            codeBytes_[offset] = data;
        }
    }
    void extendBytes(const ListingLine & otherLine) {
        codeBytes_.insert(codeBytes_.end(), otherLine.codeBytes_.begin(), otherLine.codeBytes_.end());
    }
    bool isEmpty() const {
        return codeBytes_.size() == 0 and std::regex_match(bodyText_, whitespace);
    }
};

class ListingMatcher : public IListingMatcher {
    static constexpr int NO_BANK = -1;
    template <typename T>
    static inline bool _shouldOutputIncludedFile(T includedFilename) {
        // We should output the file if it doesn't match one of the forbidden
        // includes.
        return not std::regex_match(includedFilename, forbiddenIncludes);
    }
    template <typename T>
    static inline int _getBankFromLine(T curLineStr) {
        std::smatch m;
        if (not std::regex_match(curLineStr, m, bankLineRe)) {
            return NO_BANK;
        }
        return std::stoi(m.str(1), nullptr, 16) + 0xC0;
    }
public:
    virtual void processListing(std::istream & listing){
        std::optional<ListingLine> lastLine{};
        std::optional<ListingLine> curLine{};
        std::string curLineStr{};
        std::string curFile{};
        int bank = NO_BANK;
        while (listing.good()) {
            if (not std::getline(listing, curLineStr)) {
                break;
            }
            if (curLineStr.length() == 0) {
                continue;
            }
            if (bank == NO_BANK) {
                std::smatch bankMatch;
                bank = _getBankFromLine(curLineStr);
                continue;
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
            curLine->updateRelocations(getRom());
            if (curLine->bodyText().length() == 0 and curLine->codeBytes().size() > 0) {
                // This line is just more bytes for the previous line.
                if (not lastLine) {
                    throw malformed_listing("Unexpectedly, there are bytes on a line without a "
                                            "previous line to associate them with");
                }
                lastLine->extendBytes(*curLine);
            } else {
                if (lastLine and not lastLine->isEmpty()) {
                    std::cout << std::format("{:60} | ", curFile) << lastLine->toString() << std::endl;
                }
                lastLine = curLine;
            }
        }
    }
};

std::unique_ptr<IListingMatcher> createListingMatcher() {
    return std::make_unique<ListingMatcher>();
}
