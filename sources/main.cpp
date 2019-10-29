#include <iostream>
#include <Board.hpp>

int main(int argc, char* argv[]) {
    Board check(argv[1]);
    auto res = check.minimax_root(3);
    check.make_best_move();
    check.save_to_file();
    return 0;
}