#include "Cell.hpp"

Cell::Cell() : _state(Cell::State::BLANK)
{}

Cell::Cell(State state, bool queen)  :
    _state(state),
    _queen(queen)
{}

Cell& Cell::operator = (const Cell& cell) {
    _state = cell._state;
    _queen = cell._queen;
    return *this;
}

Cell::State Cell::getState() const {
    return _state;
}

void Cell::setState(State state) {
    _state = state;
}

bool Cell::isQueen() const {
    return _queen;
}

void Cell::setQueen(bool q) {
    _queen = q;
}


char Cell::getSymbolPieces() const {
    if (_state == Cell::State::WHITE)
        return _queen ? WHITE_QUEEN : WHITE;
    else if (_state == Cell::State::BLACK)
        return _queen ? BLACK_QUEEN : BLACK;
    else {
        return BLANK;
    }
}

