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

class ListingMatcher : public IListingMatcher {
    template <typename T>
    static inline bool _shouldOutputIncludedFile(T includedFilename) {
        // We should output the file if it doesn't match one of the forbidden
        // includes.
        return not std::regex_match(includedFilename, forbiddenIncludes);
    }
    MapFileReader const & map_;
    IRenderer & renderer_;
public:
    ListingMatcher(MapFileReader const & map, IRenderer & renderer) : map_(map), renderer_(renderer) {}
    virtual void processListing(std::istream & listing){
        std::optional<ListingLine> lastLine{};
        std::optional<ListingLine> curLine{};
        std::string curLineStr{};
        std::string curFile{};
        Segment const * curSegment = nullptr;
        while (listing.good()) {
            if (not std::getline(listing, curLineStr)) {
                break;
            }
            if (curLineStr.length() == 0) {
                continue;
            }
            // For cases where we have lines without a segment defined beforehand, we will just
            // use an offset of zero.
            curLine = ListingLine::fromString(curSegment ? curSegment->start : 0U, curLineStr);
            if (not curLine.has_value()) {
                // Unknown line - not an error (the file starts with non-format lines).
                // Keep going.
                continue;
            }
            {
                // Check if this line is a segment stmt.
                std::smatch segmentReMatch;
                if (std::regex_match(curLine->bodyText(), segmentReMatch, segmentBodyRe)) {
                    curSegment = &map_.getSegment(segmentReMatch.str(1));
                    continue;
                }
            }
            {
                // Check if this line is an include stmt.
                std::smatch includeReMatch;
                if (std::regex_match(curLine->bodyText(), includeReMatch, includeBodyRe)) {
                    if (_shouldOutputIncludedFile(includeReMatch.str(1))) {
                        curFile = includeReMatch.str(1);
                        renderer_.changeFile(curFile);
                        continue;
                    }
                }
            }
            // If we don't have a segment or filename by this point, we don't care about this line.
            if (not curSegment or curFile.empty()) {
                if (curLine->codeBytes().size() > 0) {
                    throw malformed_listing("Bytes are defined outside of a file");
                }
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
                    renderer_.consumeLine(*lastLine);
                }
                lastLine = curLine;
            }
        }
    }
};

std::unique_ptr<IListingMatcher> createListingMatcher(MapFileReader const & map, IRenderer & renderer) {
    return std::make_unique<ListingMatcher>(map, renderer);
}
