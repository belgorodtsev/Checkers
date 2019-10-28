#include <iostream>
#include <Board.hpp>

int main(int argc, char* argv[]) {
   
    Board check(argv[1]);
    check.save_to_file();
    auto res = check.minimax_root(pos(6, 5), 3, true);
   // auto p = check.check_m(pos(4, 5), pos(6, 3), true);
    std::cout << res.first << " " << res.second << std::endl;

    system("pause");
    return 0;
}