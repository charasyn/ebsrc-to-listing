#include <exception>
#include <stdexcept>

class malformed_listing : public std::runtime_error {
    using parent = std::runtime_error;
public:
    explicit malformed_listing(const std::string & msg) : parent(msg) {}
    explicit malformed_listing(const char* msg) : parent(msg) {}
};
