#include <iostream>
#include <fstream>

#include "exceptions.hpp"
#include "matcher.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "USAGE: ebsrc-to-listing <.lst>" << std::endl;
        return 1;
    }
    std::ifstream fileIn{argv[1]};
    try {
        auto listingMatcher = createListingMatcher();
        listingMatcher->processListing(0, fileIn);
    } catch (malformed_listing & e) {
        std::cerr << "Malformed listing: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
