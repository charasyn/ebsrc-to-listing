#pragma once
#include <exception>
#include <stdexcept>

class malformed_listing : public std::runtime_error {
    using parent = std::runtime_error;
public:
    explicit malformed_listing(const std::string & msg) : parent(msg) {}
    explicit malformed_listing(const char* msg) : parent(msg) {}
};

class internal_error : public std::runtime_error {
    using parent = std::runtime_error;
public:
    explicit internal_error(const std::string & msg) : parent(msg) {}
    explicit internal_error(const char* msg) : parent(msg) {}
};

class rom_hash_mismatch : public std::runtime_error {
    using parent = std::runtime_error;
public:
    explicit rom_hash_mismatch(const std::string & msg) : parent(msg) {}
    explicit rom_hash_mismatch(const char* msg) : parent(msg) {}
};
