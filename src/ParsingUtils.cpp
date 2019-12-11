#include <ParsingUtils.hpp>
#include <Exception.hpp>
#include <Cell.hpp>

namespace pu {

    bool checkLineBoard(const std::string& line, size_t col, Logger& log, const std::string& out) {
        try {
            if (line.size() > 17 || line[0] - '0' != col + 1 || line[1] != ' ')
                throw Exception("Invalid input!");
        }
        catch (Exception& except) {
            except.fail(log, out);
            return false;
        }
        const char space = ' ';
        const char whiteDiagonal = '_'; // Игра ведётся только на чёрных
        if (col % 2 == 0) { // start from '_'
            for (size_t i = 2; i < line.size(); i += 4) {
                if (i == 14) {
                    try {
                        if (line[i] != whiteDiagonal || line[i + 1] != space ||
                            (line[i + 2] != BLACK && line[i + 2] != WHITE && line[i + 2] != BLANK &&
                                line[i + 2] != BLACK_QUEEN && line[i + 2] != WHITE_QUEEN))
                            throw Exception("Invalid input!");
                    }
                    catch (Exception& except) {
                        except.fail(log, out);
                        return false;
                    }
                    break;
                }
                try {
                    if (line[i] != whiteDiagonal || line[i + 1] != space || (line[i + 2] != BLACK
                        && line[i + 2] != WHITE && line[i + 2] != BLANK && line[i + 2] != BLACK_QUEEN &&
                        line[i + 2] != WHITE_QUEEN) || line[i + 3] != space)
                        throw Exception("Invalid input!");
                }
                catch (Exception& except) {
                    except.fail(log, out);
                    return false;
                }
            }
        }
        else { // start from 'b' or 'w'
            for (size_t i = 2; i < line.size(); i += 4) {
                if (i == 14) {
                    try {
                        if ((line[i] != BLACK && line[i] != WHITE && line[i] != BLANK &&
                            line[i] != WHITE_QUEEN && line[i] != BLACK_QUEEN) ||
                            line[i + 1] != space || line[i + 2] != whiteDiagonal)
                            throw Exception("Invalid input!");
                    }
                    catch (Exception& except) {
                        except.fail(log, out);
                        return false;
                    }
                    break;
                }
                try {
                    if ((line[i] != BLACK && line[i] != WHITE && line[i] != BLANK &&
                        line[i] != WHITE_QUEEN && line[i] != BLACK_QUEEN)
                        || line[i + 1] != space || line[i + 2] != whiteDiagonal || line[i + 3] != space)
                        throw Exception("Invalid input!");
                }
                catch (Exception& except) {
                    except.fail(log, out);
                    return false;
                }
            }
        }
        return true;
    }

    bool checkFirstLine(const std::string& line, Logger& log, const std::string& out) {
        const char space = ' ';
        try {
            if (line[0] != space && line[1] != space)
                throw Exception("Invalid input!");
        }
        catch (Exception& except) {
            except.fail(log, out);
            return false;
        }
        int codeSymbol = 65; // код символа - 'A'
        for (size_t i = 2; i < line.size(); i += 2) {
            try {
                if (i < 16)
                    if (line[i] != codeSymbol || line[i + 1] != space)
                        throw Exception("Invalid input!");
                if (i == 16 && line[i] != codeSymbol) // else out of range
                    throw Exception("Invalid input!");
            }
            catch (Exception& except) {
                except.fail(log, out);
                return false;
            }
            ++codeSymbol;
        }
        return true;
    }

    bool chekMovePos(const std::string& str) {
        if (str.empty() || str.size() != 6)
            return false;
        // коды символов
        auto A = 65;
        auto H = 72;
        auto a = 97;
        auto h = 104;
        auto one = 49;
        auto eight = 56;
        for (auto i = 0; i < str.size(); i += 2) {
            if (i != 2) {
                if (((str[i] < A || str[i] > H) || (str[i] < a || str[i] > h)) &&
                    (str[i + 1] < one || str[i + 1] > eight))
                    return false;
                continue;
            }
            if (str[i] != '-' || str[i + 1] != '>')
                return false;
        }
        return true;
    }

    std::string posToString(const pos& pos) {
        std::string strPos;
        switch (pos.first) {
        case 0: {
            strPos += 'A';
            break;
        }
        case 1: {
            strPos += 'B';
            break;
        }
        case 2: {
            strPos += 'C';
            break;
        }
        case 3: {
            strPos += 'D';
            break;
        }
        case 4: {
            strPos += 'E';
            break;
        }
        case 5: {
            strPos += 'F';
            break;
        }
        case 6: {
            strPos += 'G';
            break;
        }
        case 7: {
            strPos += 'H';
            break;
        }
        }
        strPos += std::to_string(pos.second + 1);
        return strPos;
    }

    pos stringToPos(const std::string& str) {
        pos startPos;
        for (size_t i = 0; i < str.size(); i++) {
            switch (str[i]) {
            case 'a':
            case 'A': {
                startPos.first = 0;
                break;
            }
            case 'b':
            case 'B': {
                startPos.first = 1;
                break;
            }
            case 'c':
            case 'C': {
                startPos.first = 2;
                break;
            }
            case 'd':
            case 'D': {
                startPos.first = 3;
                break;
            }
            case 'e':
            case 'E': {
                startPos.first = 4;
                break;
            }
            case 'f':
            case 'F': {
                startPos.first = 5;
                break;
            }
            case 'g':
            case 'G': {
                startPos.first = 6;
                break;
            }
            case 'h':
            case 'H': {
                startPos.first = 7;
                break;
            }
            case '1': {
                startPos.second = 0;
                break;
            }
            case '2': {
                startPos.second = 1;
                break;
            }
            case '3': {
                startPos.second = 2;
                break;
            }
            case '4': {
                startPos.second = 3;
                break;
            }
            case '5': {
                startPos.second = 4;
                break;
            }
            case '6': {
                startPos.second = 5;
                break;
            }
            case '7': {
                startPos.second = 6;
                break;
            }
            case '8': {
                startPos.second = 7;
                break;
            }
            }
        }
        return startPos;
    }

    std::pair<char, size_t> parsArgv(const char* color, const char* depth) {
        char player;
        if (color == "WHITE")
            player = WHITE;
        else if (color == "BLACK")
            player = BLACK;
        else if (color[0] == 'w')
            player = WHITE;
        else if (color[0] == 'b')
            player = BLACK;
        else
            player = 'w';

        auto d = std::atoi(depth);
        if (d > 8)
            d = 8;
        return { player, d };
    }

} // namespace cu