#include <cstdint>
#include <iostream>
#include <filesystem>
#include <format>
#include <fstream>
namespace fs = std::filesystem;

#include "Common.hpp"
#include "Exceptions.hpp"
#include "Matcher.hpp"
#include "TextRenderer.hpp"
#include "MapFileReader.hpp"

#include "Sha256.hpp"

struct EbVersionInfo {
    std::string longName;
    std::string country;
    sha256_type cleanRomHash;
};

EbVersionInfo versions[] = {
    {
        .longName{"earthbound"},
        .country{"US"},
        .cleanRomHash{
            0xa8,0xfe,0x22,0x26,
            0x72,0x80,0x02,0x78,
            0x6d,0x68,0xc2,0x7d,
            0xdd,0xdf,0x0b,0x90,
            0xa8,0x94,0xdb,0x52,
            0xe4,0xdf,0xe2,0x68,
            0xfd,0xf7,0x2a,0x68,
            0xca,0xe5,0xf0,0x2e,
        }
    },
    {
        .longName{"mother2"},
        .country{"JP"},
        .cleanRomHash{
            0x1f,0x8c,0xfd,0x13,
            0x17,0x7d,0x86,0xb0,
            0xeb,0x2c,0x8a,0xdc,
            0xf9,0xe1,0xa4,0xf0,
            0xec,0x89,0x66,0xfa,
            0x15,0x83,0x07,0x2b,
            0x65,0xa1,0xb1,0xc0,
            0xe7,0x96,0x1a,0x5d,
        }
    },
};

const EbVersionInfo & getVersionFromCountryCode(const std::string & country) {
    for (const auto & ver : versions) {
        if (ver.country == country) {
            return ver;
        }
    }
    throw bad_country("");
}

static EbRom theRom_;
const EbRom & getRom() { return theRom_; }

static void initializeRom(const EbVersionInfo & version, EbRom & rom, std::istream & romIstream) {
    romIstream.read(reinterpret_cast<char *>(rom.data.data()), rom.data.size());
    hash_sha256 romHasher{};
    romHasher.sha256_init();
    romHasher.sha256_update(rom.data.data(), rom.data.size());
    const auto actualHash = romHasher.sha256_final();
    for (std::size_t i = 0; i < actualHash.size(); i += 1) {
        if (version.cleanRomHash[i] != actualHash[i]) {
            throw rom_hash_mismatch("");
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "USAGE: ebsrc-to-listing <US or JP> <path to ebsrc/build directory> <path to output directory>" << std::endl;
        return 1;
    }
    std::string countryStr{argv[1]};
    fs::path fpSource{argv[2]};
    fs::path fpDest{argv[3]};

    std::string currentListing{"<none>"};
    try {
        const EbVersionInfo & version = getVersionFromCountryCode(countryStr);
        using openmode = std::ios_base::openmode;
        std::ifstream romIstream{fpSource / (version.longName + ".sfc"), openmode::_S_bin | openmode::_S_in};
        std::ifstream mapFileIstream{fpSource / (version.longName + ".map")};
        initializeRom(version, theRom_, romIstream);
        const auto map = MapFileReader::fromIstream(mapFileIstream);
        for (int bank = 0; bank < 0x30; bank += 1) {
            currentListing = std::format("bank{:02X}", bank);
            std::cout << "Processing " << currentListing << "..." << std::endl;
            std::ifstream listingFileIstream{fpSource / version.country / (currentListing + ".lst")};
            std::ofstream outputOstream{fpDest / (currentListing + ".txt")};
            TextRenderer renderer{outputOstream};
            auto listingMatcher = createListingMatcher(map, renderer);
            listingMatcher->processListing(listingFileIstream);
        }
    } catch(const bad_country & e) {
        std::cerr << "ERROR: Invalid country code '" << countryStr << "', must be JP or US." << std::endl;
        return 1;
    } catch (const malformed_listing & e) {
        std::cerr << "ERROR: Malformed listing '" << currentListing << "': " << e.what() << std::endl;
        return 1;
    } catch (const rom_hash_mismatch & e) {
        std::cerr << "ERROR: Invalid ROM hash." << std::endl;
        return 1;
    }
    return 0;
}
