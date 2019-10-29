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
    pos minimax_root(size_t depth);
    size_t minimax(const pos& startPos, size_t depth, size_t alpha, size_t beta, bool isMaximisingPlayer);
    void  make_best_move();

private:
    std::map<pos, Cell> _cells;
    pos _pos_from_search;
    std::pair<pos, Board::MoveResult> _best_move;
    const size_t _boardSize = 8;

    void parsing(const std::string& filename);
    std::string pos_to_string(const pos& pos);
    MoveResult check_move(const pos& startPos, const pos& endPos, bool direction);
    void make_move(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos);
    std::vector<std::pair<pos, Board::MoveResult>> generate_all_moves(const pos& startPos, bool direction);
    void roll_back_move(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos);
    int evaluateBoard();
    int getPieceValue(const pos& pos);
};

