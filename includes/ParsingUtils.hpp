#ifndef PARSINGUTILS_HPP
#define PARSINGUTILS_HPP

#include <string>
#include "Logger.hpp"

using pos = std::pair<size_t, size_t>;
using posMove = std::pair<pos, pos>;

namespace pu {
    // Проверка строки входного файла на валидность
    bool checkLineBoard(const std::string& line, size_t col, Logger& log, const std::string& out);

    // Проверка первой строки A B C D E F G H
    bool checkFirstLine(const std::string& line, Logger& log, const std::string& out);

    // Проверка заданной возиции хода
    bool chekMovePos(const std::string& str);

    // Перевод позици на доске в строку типа 0,0 -> А1
    std::string posToString(const pos& pos);

    // Аналогично предидущему только наоборот
    pos stringToPos(const std::string& str);

    // Парс входных аргументов
    std::pair<char, size_t> parsArgv(const char* color, const char* depth);

} // namespace pu

#endif // PARSINGUTILS_HPP
