#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>
#include "Logger.hpp"

class Exception :
    public std::exception
{
public:
    Exception(const std::string& error) noexcept;

    const char* what() const noexcept;

    void fail(Logger& log, const std::string& out) noexcept;

    virtual ~Exception() = default;
private:

    std::string _error;
};

#endif // EXCEPTION_HPP
