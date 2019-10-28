#include "Board.hpp"

Board::Board()
{}

Board::Board(const std::string& filename) {
    parsing(filename);
}

Board::MoveResult Board::check_move(const pos& startPos, const pos& endPos, bool direction) {
    Board::MoveResult result = Board::MoveResult::PROHIBITED;
    auto startCellState = _cells.at(startPos).get_state();
    if (startCellState == Cell::State::BLANK)
        return result;
    const int dX = startPos.first - endPos.first;
    const int dY = startPos.second - endPos.second;
    if (endPos.first >= 0 && endPos.first < _boardSize &&
        endPos.second >= 0 && endPos.second < _boardSize) {
        auto targetCellState = _cells.at(endPos).get_state();
        if (targetCellState == Cell::State::BLANK) {      
            if (abs(dX) == 2 && abs(dY) == 2) {
                pos victimPos((startPos.first + endPos.first) / 2,
                    (startPos.second + endPos.second) / 2);
                auto victimCellState = _cells.at(victimPos).get_state();
                if (targetCellState != victimCellState &&
                    targetCellState != startCellState)
                    result = Board::MoveResult::SUCCESSFUL_COMBAT;
            } else if ((abs(dX) == 1 && dY == 1 && direction) ||
                (abs(dX) == 1 && dY == -1 && !direction))
                result = Board::MoveResult::SUCCESSFUL_MOVE;
        }
    }
    return result;
}

std::vector<pos> Board::generate_all_moves(const pos& startPos, bool direction) {
    std::vector<pos> moves;
    auto startCellState = _cells.at(startPos).get_state();
    if (startCellState == Cell::State::BLANK)
        return moves;
    std::array<pos, 8> possible_pos = {
        std::make_pair(startPos.first + 1, startPos.second + 1), // direction - true
        std::make_pair(startPos.first - 1, startPos.second + 1), // direction - true
        std::make_pair(startPos.first + 1, startPos.second - 1), // direction - false i = 2
        std::make_pair(startPos.first - 1, startPos.second - 1), // direction - false i = 3 
        std::make_pair(startPos.first + 2, startPos.second + 2), // direction - never mind
        std::make_pair(startPos.first - 2, startPos.second + 2),
        std::make_pair(startPos.first + 2, startPos.second - 2),
        std::make_pair(startPos.first - 2, startPos.second - 2) };

    Board::MoveResult result = Board::MoveResult::PROHIBITED;
    for (size_t i = 0; i < possible_pos.size(); ++i) {
        if (i == 2 || i == 3)
            result = check_move(startPos, possible_pos[i], false);
        else 
            result = check_move(startPos, possible_pos[i], true);
        if (result != Board::MoveResult::PROHIBITED)
            moves.push_back(possible_pos[i]);
    }
    return moves;
}

void Board::make_move(const pos& startPos, const pos& endPos) {
    auto startCellState = _cells.at(startPos).get_state();
    if (startCellState == Cell::State::BLANK)
        return;
    auto it = _cells.find(startPos);
    if (it != _cells.end()) {
        _cells.erase(it);
        _cells.insert({ std::move(endPos), std::move(startCellState) });
    }
    else
        return;
}

void Board::roll_back_move(const pos& startPos, const pos& endPos) {
    auto startCellState = _cells.at(endPos).get_state();
    auto it = _cells.find(endPos);
    _cells.erase(it);
    _cells.insert({ std::move(startPos), std::move(startCellState) });
}


pos Board::minimax_root(const pos& startPos, size_t depth, bool isMaximisingPlayer) {
    auto newGameMoves = generate_all_moves(startPos, true);
    size_t bestMove = -9999;
    pos bestMoveFound;
    for (size_t i = 0; i < newGameMoves.size(); i++) {
        auto newGameMove = newGameMoves[i];
        make_move(startPos, newGameMove);
        auto value = minimax(newGameMove, depth - 1, -10000, 10000, !isMaximisingPlayer);
        roll_back_move(startPos, newGameMove);
        if (value >= bestMove) {
            bestMove = value;
            bestMoveFound = newGameMove;
        }
    }
    return bestMoveFound;
}

size_t Board::minimax(const pos& startPos, size_t depth, size_t alpha, size_t beta, bool isMaximisingPlayer) {
    if (depth == 0) {
        return -evaluateBoard();
    }

    auto newGameMoves = generate_all_moves(startPos, true);

    if (isMaximisingPlayer) {
        size_t bestMove = -9999;
        for (size_t i = 0; i < newGameMoves.size(); i++) {
            make_move(startPos, newGameMoves[i]);
            bestMove = std::max(bestMove, minimax(
                 newGameMoves[i], depth - 1, alpha, beta, !isMaximisingPlayer));
            roll_back_move(startPos, newGameMoves[i]);
            alpha = std::max(alpha, bestMove);
            if (beta <= alpha) {
                return bestMove;
            }
        }
        return bestMove;
    }
    else {
        size_t bestMove = 9999;
        for (size_t i = 0; i < newGameMoves.size(); i++) {
            make_move(startPos, newGameMoves[i]);
            bestMove = std::min(bestMove, minimax(
                newGameMoves[i], depth - 1, alpha, beta, !isMaximisingPlayer));
            roll_back_move(startPos, newGameMoves[i]);
            beta = std::min(beta, bestMove);
            if (beta <= alpha) {
                return bestMove;
            }
        }
        return bestMove;
    }
}

int Board::evaluateBoard() {
    auto totalEvaluation = 0;
    for (auto it : _cells)
        totalEvaluation = totalEvaluation + getPieceValue(it.first);
    return totalEvaluation;
}

int Board::getPieceValue(const pos& pos) {
    auto cellState = _cells.at(pos).get_state();
    if (cellState == Cell::State::BLANK) {
        return 0;
    }
    auto absoluteValue = 10; 
    return cellState == Cell::State::WHITE ? absoluteValue : -absoluteValue;
}

void Board::parsing(const std::string& filename) {
    std::ifstream ifs(filename);
    std::string line;
    size_t col = 0;
    size_t row = 0;
    while (std::getline(ifs, line)) {
        if (col % 2 == 0)
            row = 1;
        else
            row = 0;
        for (size_t i = 0; i < line.size(); ++i) {
            auto state = Cell::State::BLANK;
            switch (line[i]) {
            case ' ': continue;
            case '0': {
                const pos positions = pos(row, col);
                _cells.insert({ std::move(positions), std::move(state) });
                break;
            }
            case 'W': {
                const pos positions = pos(row, col);
                state = Cell::State::WHITE;
                _cells.insert({ std::move(positions), std::move(state) });
                break;
            }
            case 'B': {
                const pos positions = pos(row, col);
                state = Cell::State::BLACK;
                _cells.insert({ std::move(positions), std::move(state) });
                break;
            }
            }
            row += 2;
        }
        ++col;
    }
}

void Board::save_to_file() { // быдлокодик небольшой
    std::ofstream out("save_data.txt");
    size_t row = 0;
    size_t col = 0;
    bool flag_space = true;
    for (size_t i = 0; i < _cells.size(); i += 4) {
        if (col % 2 == 0) {
            row = 1;
            flag_space = true;
        }
        else {
            row = 0;
            flag_space = false;
        }
        if (flag_space) {
            out << "  " << _cells.at(pos(row, col)).get_color()
                << "   " << _cells.at(pos(row + 2, col)).get_color()
                << "   " << _cells.at(pos(row + 4, col)).get_color()
                << "   " << _cells.at(pos(row + 6, col)).get_color()
                << std::endl;
        }
        else {
            out << _cells.at(pos(row, col)).get_color()
                << "   " << _cells.at(pos(row + 2, col)).get_color()
                << "   " << _cells.at(pos(row + 4, col)).get_color()
                << "   " << _cells.at(pos(row + 6, col)).get_color()
                << "  " << std::endl;
        }
        ++col;
    }
}
