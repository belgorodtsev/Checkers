﻿#ifndef CELL_HPP
#define CELL_HPP

#include <utility>

using pos = std::pair<size_t, size_t>;
using posMove = std::pair<pos, pos>;

const char BLANK = '0';
const char BLACK = 'b';
const char WHITE = 'w';
const char BLACK_QUEEN = 'B';
const char WHITE_QUEEN = 'W';

class Cell {
public:
    enum class State {WHITE, BLACK, BLANK};

    Cell();

    Cell(State state, bool queen);

    Cell& operator = (const Cell& cell);

    ~Cell() = default;

    State getState() const;

    // Находится ли на этой клетке дамка
    bool isQueen() const;

    // Возвращает символ фигуры
    char getSymbolPieces() const;

    void setState(State state);

    void setQueen(bool q);
 
private:
    State _state;

    bool _queen;
};

#endif // CELL_HPP