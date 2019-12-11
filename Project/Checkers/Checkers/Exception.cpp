#include "Exception.hpp"

Exception::Exception(const std::string& error) noexcept :
    _error(error)
{}

const char* Exception::what() const noexcept {
    return _error.c_str();
}

void Exception::fail(Logger& log, const std::string& out) noexcept {
    std::ofstream ifs(out, std::ios::out | std::ios::app);
    ifs << what() << std::endl;
    log << CRITICAL << what() << std::endl;
}
