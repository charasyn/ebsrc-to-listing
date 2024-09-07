#include <iostream>
#include <fstream>
#include <array>
#include <cstdint>

#include "common.hpp"
#include "exceptions.hpp"
#include "matcher.hpp"

#include "sha256.hpp"

constexpr static sha256_type cleanRomHash = {
    0xa8,0xfe,0x22,0x26,
    0x72,0x80,0x02,0x78,
    0x6d,0x68,0xc2,0x7d,
    0xdd,0xdf,0x0b,0x90,
    0xa8,0x94,0xdb,0x52,
    0xe4,0xdf,0xe2,0x68,
    0xfd,0xf7,0x2a,0x68,
    0xca,0xe5,0xf0,0x2e,
};

static EbRom theRom_;
const EbRom & getRom() { return theRom_; }

static void initializeRom(EbRom & rom, std::istream & romIstream) {
    romIstream.read(reinterpret_cast<char *>(rom.data.data()), rom.data.size());
    hash_sha256 romHasher{};
    romHasher.sha256_init();
    romHasher.sha256_update(rom.data.data(), rom.data.size());
    const auto actualHash = romHasher.sha256_final();
    for (std::size_t i = 0; i < actualHash.size(); i += 1) {
        if (cleanRomHash[i] != actualHash[i]) {
            throw rom_hash_mismatch("");
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "USAGE: ebsrc-to-listing <ROM file (.sfc)> <Listing file (.lst)>" << std::endl;
        return 1;
    }
    using openmode = std::ios_base::openmode;
    std::ifstream romIstream{argv[1], openmode::_S_bin | openmode::_S_in};
    std::ifstream listingFileIstream{argv[2]};
    (void)romIstream;
    (void)listingFileIstream;
    try {
        initializeRom(theRom_, romIstream);
        auto listingMatcher = createListingMatcher();
        listingMatcher->processListing(listingFileIstream);
    } catch (const malformed_listing & e) {
        std::cerr << "ERROR: Malformed listing: " << e.what() << std::endl;
        return 1;
    } catch (const rom_hash_mismatch & e) {
        std::cerr << "ERROR: Invalid ROM hash." << std::endl;
        return 1;
    }
    return 0;
}
