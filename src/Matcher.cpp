#include <format>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "BodySanitizer.hpp"
#include "Exceptions.hpp"
#include "ListingLine.hpp"
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
    IRenderer * renderer_;
public:
    ListingMatcher(IRenderer * renderer) : renderer_(renderer) {}
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
                    renderer_->changeFile(curFile);
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
                    renderer_->consumeLine(*lastLine);
                }
                lastLine = curLine;
            }
        }
    }
};

std::unique_ptr<IListingMatcher> createListingMatcher(IRenderer * renderer) {
    return std::make_unique<ListingMatcher>(renderer);
}
