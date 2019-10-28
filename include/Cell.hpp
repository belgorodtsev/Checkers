#pragma once
#include <utility>

using pos = std::pair<size_t, size_t>;

class Cell {
public:
    enum class State {WHITE, BLACK, BLANK};
    Cell();
    Cell(State state);
    ~Cell() = default;
    State get_state() const;
    char get_color() const;
    void set_state(State state);
 
private:
   // std::string name_;
    State _state;
};

