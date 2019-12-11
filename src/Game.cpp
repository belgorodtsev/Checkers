#include "Game.hpp"

Game::Game() {
    _board = new Board();
}

Game::Game(const std::string& in, const std::string& out) {
    _board = new Board(in, out);
}

Game::~Game() {
    delete _board;
}

void Game::saveGame() {
    _board->save();
}

void Game::calculateBestMove(const char player, size_t depth) {
    _board->calculateBestMove(player, depth);
}

// Расчёт лучшего хода
void Game::calculateBestMove(const char* color, const char* depth) {
    auto res = pu::parsArgv(color, depth);
    _board->calculateBestMove(res.first, res.second);
}

void Game::makeBestMove() {
    _board->makeBestMove();
}

std::pair<pos, Board::MoveResult> Game::getBestMove() const {
    return _board->getBestMove();
}

void Game::printBoard() const {
    _board->print();
}

void Game::play(const char color, size_t depth) {
    char botColor = color == WHITE ? BLACK : WHITE;
    posMove move;
    _board->print();
    // format move a2->b3 or A2->B3
    while (!_board->isGameOver()) {
        move = io::setMovePos();
        bool result = false;
        while (!result) {
            result = _board->makeMove(move.first, move.second, color);
            if (result) {
                io::clearScreen();
                _board->print();
                _board->calculateBestMove(botColor, depth);
                _board->makeBestMove();
                io::clearScreen();
                _board->print();
                io::infoMove(botColor, _board->getMadeMove());
            } else {
                move = io::restartSetMovePos();
            }
        }
    }
    io::message("Game over!");
    io::pause();
}
