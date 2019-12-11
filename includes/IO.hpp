#ifndef IO_HPP
#define IO_HPP

#include <iostream>
#include <cstdlib>
#include <utility>
#include <string>
#include <chrono>
#include <thread>

using pos = std::pair<size_t, size_t>;
using posMove = std::pair<pos, pos>;

namespace io {
    // очистить консоль
    void clearScreen();

    // задать позицию хода с консоли в формате A1->B2 или a1->b2
    posMove setMovePos();

    // вызывается, если первый ход невозможно сделать
    posMove restartSetMovePos();

    // информация о сделанном ходе
    void infoMove(const char color, const posMove& move);

    void sleep(size_t sec);

    void pause();

    void message(const std::string& str);

} // namespace io

#endif // IO_HPP