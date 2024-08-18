#include <regex>
#include <string>
#include <istream>

// We don't care about printing out the listings for these files.
static const std::regex forbiddenIncludes{
    "structs|eventmacros|config|constants|enums|hardware|bin|common|"
    "macros|textmacros|symbols"
};

static const std::regex lineReInclude{
    R"/(^[0-9A-F]{6}\w \d .{13} \s*\.INCLUDE "[^"]+")/"
};

class ListingMatcher {
    template <typename T>
    static inline bool _shouldOutputIncludedFile(T includedFilename) {
        // We should output the file if it doesn't match one of the forbidden
        // includes.
        return not std::regex_match(includedFilename, forbiddenIncludes);
    }
public:
    void processListing(std::istream & listing);
};

void
ListingMatcher::processListing(std::istream & listing) {
    std::string curLine;
    // Listings start with a bunch of junk that we don't care about. Keep going
    // until we've gotten a file to output into.
    
}
