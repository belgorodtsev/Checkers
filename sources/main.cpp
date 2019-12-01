#include <iostream>
#include <Board.hpp>

int main(int argc, char* argv[]) {
    // сделать лог в классе game который будут записываться ходы
    Board check(argv[1]);
    check.minimaxRoot('w', 3);
    check.make_best_move();
    check.saveToFile("C:\\Users\\Sergei\\Desktop\\save_data.txt");
    return 0;
}