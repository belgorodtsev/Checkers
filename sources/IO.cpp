#include "IO.hpp"
#include <ParsingUtils.hpp>

namespace io {
    void clearScreen() {
        system("cls");
    }

    void pause() {
        system("pause");
    }

    void message(const std::string& str) {
        std::cout << str << std::endl;
    }

    void sleep(size_t sec) {
        std::this_thread::sleep_for(std::chrono::seconds(sec));
    }

    posMove setMovePos() {
        std::cout << "Enter the pos of the move:";
        std::string move;
        bool flag = false;
        while (!flag) {
            std::cin >> move;
            flag = pu::chekMovePos(move);
            if (flag) {
                pos startPos = pu::stringToPos(move.substr(0, 2));
                pos ensPos = pu::stringToPos(move.substr(4, 2));
                return { startPos , ensPos };
            }
            else {
                std::cout << "Wrong format, try it again:";
                move.clear();
            }
        }
    }

    posMove restartSetMovePos() {
        std::cout << "Wrong move, try it again!\n";
        return setMovePos();
    }

    void infoMove(const char color, const posMove& move) {
        auto colorInfo = color == 'w' ? "White`s move:" : "Black`s move:";
        std::cout << colorInfo << pu::posToString(move.first) <<
            "->" << pu::posToString(move.second) << std::endl;
    }


} // namespace io
