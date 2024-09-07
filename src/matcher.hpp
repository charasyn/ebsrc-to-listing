#include <cstdint>
#include <istream>
#include <memory>

class IListingMatcher {
public:
    virtual ~IListingMatcher() {}
    virtual void processListing(uint8_t bank, std::istream & listing) = 0;
};

std::unique_ptr<IListingMatcher> createListingMatcher();
