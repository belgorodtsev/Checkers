#pragma once
#include <Cell.hpp>
#include <array>
#include <algorithm>
#include <string>
#include <map>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

class Board
{
public:
    enum class MoveResult {SUCCESSFUL_MOVE, SUCCESSFUL_COMBAT, PROHIBITED};
    Board();
    Board(const std::string& filename);
    ~Board() = default;
    void save_to_file();
    pos minimax_root(const pos& startPos, size_t depth, bool isMaximisingPlayer);
    size_t minimax(const pos& startPos, size_t depth, size_t alpha, size_t beta, bool isMaximisingPlayer);
    void make_move(const pos& startPos, const pos& endPos);

private:
    std::map<pos, Cell> _cells;
    const size_t _boardSize = 8;
    void parsing(const std::string& filename);
    MoveResult check_move(const pos& startPos, const pos& endPos, bool direction);
    std::vector<pos> generate_all_moves(const pos& startPos, bool direction);
    void roll_back_move(const pos& startPos, const pos& endPos);
    int evaluateBoard();
    int getPieceValue(const pos& pos);
};

