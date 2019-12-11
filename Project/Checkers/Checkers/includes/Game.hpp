#ifndef GAME_HPP
#define GAME_HPP

#include "Board.hpp"
#include "Logger.hpp"
#include "IO.hpp"

class Game
{
public:
    // Конструктор, который устанавливает шашки в начальное состояние
    Game();

    // Конструктор, который заполняет позиции на доске из файла
    Game(const std::string& in, const std::string& out);

    ~Game();

    void saveGame();

    // Расчёт лучшего хода
    void calculateBestMove(const char color, size_t depth);

    // Сделать лучший ход
    void makeBestMove();

    // Возвращает лучший ход (только после вызова calculateBestMove())
    std::pair<pos, Board::MoveResult> getBestMove() const;

    // Печать доски
    void printBoard() const;
    
    // Полноценная игра. Первый аргумент - выбор цвета за который будем
    // играть ('w' или  'b'), второй - глубина просчёта ходов соперника
    void play(const char color, size_t depth);

private:
    Board *_board;
};

#endif // GAME_HPP
