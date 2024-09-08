#pragma once

#include <cstdint>
#include <istream>
#include <memory>

#include "Common.hpp"
#include "IRenderer.hpp"
#include "MapFileReader.hpp"

class IListingMatcher {
public:
    virtual ~IListingMatcher() {}
    virtual void processListing(std::istream & listing) = 0;
};

std::unique_ptr<IListingMatcher> createListingMatcher(MapFileReader const & map, IRenderer & renderer);
