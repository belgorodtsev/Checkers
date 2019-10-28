#include "Cell.hpp"

Cell::Cell() : _state(Cell::State::BLANK)
{}

Cell::Cell(State state)  :
    _state(state)
{}

Cell::State Cell::get_state() const {
    return _state;
}

void Cell::set_state(State state) {
    _state = state;
}

char Cell::get_color() const {
    if (_state == Cell::State::WHITE)
        return 'W';
    else if (_state == Cell::State::BLACK)
        return 'B';
    else
        return '0';
}

