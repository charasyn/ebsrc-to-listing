// charasyn 2024
// Public domain; see UNLICENSE.txt for more info

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
    MapFileReader const & map_;
    IRenderer & renderer_;
public:
    ListingMatcher(MapFileReader const & map, IRenderer & renderer) : map_(map), renderer_(renderer) {}
    virtual void processListing(std::istream & listing){
        std::vector<std::string> includeStack{};
        std::optional<ListingLine> lastLine{std::nullopt};
        std::optional<ListingLine> nextLine{std::nullopt};
        std::string nextLineStr{};
        Segment const * curSegment = nullptr;
        while (listing.good()) {
            if (not std::getline(listing, nextLineStr)) {
                break;
            }
            if (nextLineStr.length() == 0) {
                continue;
            }
            // Limit line length to 400 characters, to avoid breaking things later.
            if (nextLineStr.length() > 400) {
                nextLineStr.resize(400);
                nextLineStr += "<... long line trimmed>";
            }
            if (includeStack.size() == 0) {
                // Check if this line is the "Current file" line.
                std::smatch currentFileReMatch;
                if (std::regex_match(nextLineStr, currentFileReMatch, Re::currentFileLine)) {
                    includeStack.push_back(currentFileReMatch.str(1));
                    continue;
                }
            }
            // For cases where we have lines without a segment defined beforehand, we will just
            // use an offset of zero.
            nextLine = ListingLine::fromString(getRom(), curSegment ? curSegment->start : 0U, nextLineStr);
            if (not nextLine.has_value()) {
                // Unknown line - not an error (the file starts with non-format lines).
                // Keep going.
                continue;
            }
            if (nextLine->bodyText().length() == 0 and nextLine->codeBytes().size() > 0) {
                // This line is just more bytes for the previous line.
                if (not lastLine) {
                    throw malformed_listing("Unexpectedly, there are bytes on a line without a "
                                            "previous line to associate them with");
                } else if (nextLine->includeDepth() != lastLine->includeDepth()) {
                    throw malformed_listing("Bytes continuation line has different include depth");
                }
                lastLine->extendBytes(*nextLine);
                continue;
            }
            {
                // Check if this line is a segment stmt.
                std::smatch segmentReMatch;
                if (std::regex_match(nextLine->bodyText(), segmentReMatch, Re::segmentBody)) {
                    curSegment = &map_.getSegment(segmentReMatch.str(1));
                    continue;
                }
            }
            // If we get here, we know lastLine is a "complete line" (with all bytes included from
            // future lines). Let's focus on that, and save nextLine for next by moving it to
            // lastLine.
            lastLine.swap(nextLine);
            if (not nextLine) {
                continue;
            }
            ListingLine lineToProcess{std::move(*nextLine)};
            if (includeStack.size() == 0) {
                throw malformed_listing("Listing started without \"Current file\" line");
            } else if (lineToProcess.includeDepth() > includeStack.size()) {
                throw malformed_listing("Line's include depth is deeper than includeStack");
            }
            if (not curSegment and lineToProcess.codeBytes().size() > 0) {
                throw malformed_listing("Bytes are defined outside of a segment");
            }
            // Output the line if it's in a file we care about writing out.
            renderer_.consumeLine(includeStack.at(lineToProcess.includeDepth() - 1), lineToProcess);
            {
                // Check if this line is an include stmt.
                std::smatch includeReMatch;
                if (std::regex_match(lineToProcess.bodyText(), includeReMatch, Re::includeBody)) {
                    // We've detected an include stmt. Erase everything in the include stack above this line,
                    // and then add this file to the stack.
                    includeStack.erase(includeStack.begin() + lineToProcess.includeDepth(), includeStack.end());
                    includeStack.push_back(includeReMatch.str(2));
                    continue;
                }
            }
        }
    }
};

std::unique_ptr<IListingMatcher> createListingMatcher(MapFileReader const & map, IRenderer & renderer) {
    return std::make_unique<ListingMatcher>(map, renderer);
}
