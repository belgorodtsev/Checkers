#include <iostream>
#include <Game.hpp>

int main(int argc, char* argv[]) {
    Game game(argv[1], argv[2]);
    game.calculateBestMove(WHITE, 3);
    game.makeBestMove();
    game.saveGame();
    return 0;
}