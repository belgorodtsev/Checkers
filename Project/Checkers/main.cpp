#include <iostream>
#include <Game.hpp>

int main(int argc, char* argv[]) {
    if (argc >= 4) {
        Game game(argv[1], argv[2]);
        game.calculateBestMove(argv[3], argv[4]);
        game.makeBestMove();
        game.saveGame();
    }
    return 0;
}