#ifndef LOGGER_HPP
#define LOGGER_HPP

#pragma warning(disable : 4996)
#include <fstream>
#include <cassert>
#include <ctime>
#include <sstream>


// Log levels
typedef enum {
    VERBOSE = 0,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
} logger_level;


class Logger : public std::ostringstream {
public:
    Logger(const char *f);
    Logger(const std::string& f);
    Logger(const Logger &) = delete;
    Logger &operator= (const Logger &) = delete;
    ~Logger();


    void set_level(const logger_level& level);
    void set_default_line_level(const logger_level& level);
    void flush();

    template <typename T>
    Logger& operator<<(const T& t){
        *static_cast<std::ostringstream *>(this) << t;
        return (*this);
    }

    Logger& operator<<(const logger_level& level);
    typedef Logger& (*LoggerManip)(Logger&);
    Logger& operator<<(LoggerManip m);

private:
    std::string get_time() const;
    inline const char* level_str(const logger_level& level);

    std::ofstream  _file;
    std::ostream&  _log;
    logger_level   _level;
    logger_level   _line_level;
    logger_level   _default_line_level;
};


namespace std {
    inline Logger& endl(Logger& out)
    {
        out.put('\n');
        out.flush();
        return (out);
    }
}

#endif // LOGGER_HPP
