#include "Board.hpp"

Board::Board() : 
    _startPosition(pos(0,0))
{}

Board::Board(const std::string& filename) :
    _startPosition(pos(0, 0)) {
    parsing(filename);
}

Board::MoveResult Board::checkDiagonalMovement(
    const pos& startPos, const pos& endPos, Board::Way w) {
    Board::MoveResult result = Board::MoveResult::PROHIBITED;
    auto counterEnemy = 0;
    auto start = startPos;
    while (start != endPos) {
        if (w == Board::Way::rightDown) {
            ++start.first;
            ++start.second;
        } else if (w == Board::Way::rihgtUp) {
            ++start.first;
            --start.second;
        } else if (w == Board::Way::leftDown) {
            --start.first;
            ++start.second;
        } else if (w == Board::Way::leftUp) {
            --start.first;
            --start.second;
        }
        auto curСell = start;
        auto currCellState = _cells.at(curСell).getState();
        if (currCellState == Cell::State::BLANK)
            continue;
        else if (currCellState == _cells.at(startPos).getState())
            return result;
        else
            counterEnemy++;
    }
    if (counterEnemy == 0)
        result = Board::MoveResult::SUCCESSFUL_MOVE;
    else if (counterEnemy == 1)
        result = Board::MoveResult::SUCCESSFUL_COMBAT;
    return result;
}

Board::MoveResult Board::checkQueenMove(const pos& startPos, const pos& endPos) {
    Board::MoveResult result = Board::MoveResult::PROHIBITED;
    auto targetCellState = _cells.at(endPos).getState();
    const int dX = startPos.first - endPos.first;
    const int dY = startPos.second - endPos.second;
    if (targetCellState == Cell::State::BLANK) {
        if (dX < 0 && dY < 0) // right down
            return checkDiagonalMovement(startPos, endPos, Board::Way::rightDown);
        else if (dX < 0 && dY > 0) // rihgt up
            return checkDiagonalMovement(startPos, endPos, Board::Way::rihgtUp);
        else if (dX > 0 && dY < 0) // left down
            return checkDiagonalMovement(startPos, endPos, Board::Way::leftDown);
        else if (dX > 0 && dY > 0) // left up
            return checkDiagonalMovement(startPos, endPos, Board::Way::leftUp);
    }
    return result;
}

Board::MoveResult Board::checkMove(const pos& startPos, const pos& endPos, bool direction) {
    Board::MoveResult result = Board::MoveResult::PROHIBITED;
    auto startCellState = _cells.at(startPos).getState();
    if (startCellState == Cell::State::BLANK)
        return result;

    if (_cells.at(startPos).isQueen())
        return checkQueenMove(startPos, endPos);

    const int dX = startPos.first - endPos.first;
    const int dY = startPos.second - endPos.second;
    if (endPos.first >= 0 && endPos.first < _boardSize &&
        endPos.second >= 0 && endPos.second < _boardSize) {
        auto targetCellState = _cells.at(endPos).getState();
        if (targetCellState == Cell::State::BLANK) {      
            if (abs(dX) == 2 && abs(dY) == 2) {
                pos victimPos((startPos.first + endPos.first) / 2,
                    (startPos.second + endPos.second) / 2);
                auto victimCellState = _cells.at(victimPos).getState();
                if (targetCellState != victimCellState &&
                    targetCellState != startCellState &&
                    startCellState != victimCellState)
                    result = Board::MoveResult::SUCCESSFUL_COMBAT;
            } else if ((abs(dX) == 1 && dY == 1 && direction) ||
                (abs(dX) == 1 && dY == -1 && !direction)) {
                result = Board::MoveResult::SUCCESSFUL_MOVE;
                auto res = checkPiecesTransQueen(endPos);
                if (res) {
                    const auto move = std::make_pair(startPos, endPos);
                    _movesTransInQueen.insert({ std::move(move), true });
                }
            }
        }
    }
    return result;
}

pos Board::getCombatPosForPieces(const pos& startPos, const pos& prevPos, std::list<pos>& enemys) {
    auto startCellState = (_moveColor == true) ? Cell::State::WHITE : Cell::State::BLACK;
    pos endPos = pos(0, 0);
    pos victimPos = pos(0, 0);
    std::swap(_cells.at(startPos), _cells.at(prevPos));
    auto possiblePos = {
            pos(startPos.first + 2, startPos.second + 2), // direction - never mind
            pos(startPos.first - 2, startPos.second + 2),
            pos(startPos.first + 2, startPos.second - 2),
            pos(startPos.first - 2, startPos.second - 2) };
    for (auto position : possiblePos) {
        if (position == prevPos)
            continue;
        Board::MoveResult result = Board::MoveResult::PROHIBITED;
        const int dX = startPos.first - position.first;
        const int dY = startPos.second - position.second;
        if (position.first >= 0 && position.first < _boardSize &&
            position.second >= 0 && position.second < _boardSize) {
            auto targetCellState = _cells.at(position).getState();
            if (targetCellState == Cell::State::BLANK) {
                if (abs(dX) == 2 && abs(dY) == 2) {
                    victimPos = pos((startPos.first + position.first) / 2,
                        (startPos.second + position.second) / 2);
                    auto victimCellState = _cells.at(victimPos).getState();
                    if (targetCellState != victimCellState &&
                        targetCellState != startCellState &&
                        startCellState != victimCellState) {
                        result = Board::MoveResult::SUCCESSFUL_COMBAT;
                        enemys.push_back(victimPos);
                        endPos = position;
                        auto res = checkPiecesTransQueen(position);
                        if (res)
                            return getCombatPosForQueenPieces(endPos, startPos, enemys);
                    }
                }
            }
        }
    }
    std::swap(_cells.at(startPos), _cells.at(prevPos));
    if (endPos == pos(0, 0))
        return startPos;
    return getCombatPosForPieces(endPos, startPos, enemys);
}

pos Board::getCombatPosForQueenPieces(const pos& startPos, const pos& prevPos, std::list<pos>& enemys) {
    auto startCellState = (_moveColor == true) ? Cell::State::WHITE : Cell::State::BLACK;
    pos endPos = pos(0, 0);
    pos victimPos = pos(0, 0);
    auto possible_pos = generatePossibleCombatPosQueen(startPos, prevPos);

    std::swap(_cells.at(startPos), _cells.at(prevPos));

    for (auto position : possible_pos) {
        auto check = checkQueenMove(startPos, position);
        if (check == Board::MoveResult::SUCCESSFUL_COMBAT) {

             victimPos = getVictimQueen(startPos, position);
             auto result = getCombatPosForQueenPieces(position, startPos, enemys);

             auto it_enemy = std::find(enemys.begin(), enemys.end(), victimPos);
             if (it_enemy == enemys.end()) {
                 enemys.push_back(victimPos);
                 endPos = position;
             }
             if (result != position)
                endPos = result;
        }
        check = Board::MoveResult::PROHIBITED;
    }
   
    std::swap(_cells.at(startPos), _cells.at(prevPos));
    if (endPos == pos(0, 0))
        return startPos;
    return endPos;
}

pos Board::getPossibleCombatPos(const pos& startPos, const pos& prevPos) {
    pos endPos = pos(0, 0);
    pos victimPos = pos(0, 0);
    std::list<pos> enemys;
    if (!_cells.at(startPos).isQueen()) {
        // добавляем побитую шашку
        enemys.push_back(pos((prevPos.first + startPos.first) / 2,
            (prevPos.second + startPos.second) / 2));
        auto combatPos = getCombatPosForPieces(startPos, prevPos, enemys);
        const auto move = std::make_pair(prevPos, combatPos);
        // если в предыдущей позиции состояние BLANK, то попали
        // в дамочное поле возвращаем фигуру на исходное место
        if (_cells.at(prevPos).getState() == Cell::State::BLANK) {
            std::swap(_cells.at(startPos), _cells.at(prevPos));
            _movesTransInQueen.insert({ std::move(move), true });
        }
        // добавляем в мапу ходов для боя
        _combatPieces.insert({ std::move(move), std::move(enemys) });
        return combatPos;
    } else { // бой для дамки
        auto victimPos = getVictimQueen(prevPos, startPos);
        enemys.push_back(victimPos);
        auto combatPos = getCombatPosForQueenPieces(startPos, prevPos, enemys);
        const auto move = std::make_pair(prevPos, combatPos);
        _combatPieces.insert({ std::move(move), std::move(enemys) });
        return combatPos;
    }
}

bool Board::checkPiecesTransQueen(const pos& pos) {
    if (_cells.at(pos).isQueen())
        return false;
    if (_moveColor) {
        if (pos.second == 0)
            return true;
    } else {
        if (pos.second == 7)
            return true;
    }
    return false;
}

std::list<pos> Board::generatePossiblePosQueen(const pos& startPos) {
    std::list<pos> possible_pos;
    int right = startPos.first;
    int left = startPos.first;
    int up = startPos.second;
    int down = startPos.second;
    while (1) {
        ++right;
        --left;
        --up;
        ++down;
        if (right <= 7 && down <= 7)
            possible_pos.push_back({ right, down });
        if (right <= 7 && up >= 0)
            possible_pos.push_back({ right, up });
        if (left >= 0 && down <= 7)
            possible_pos.push_back({ left, down });
        if (left >= 0 && up >= 0)
            possible_pos.push_back({ left, up });
        if ((right > 7 && down > 7) && (right > 7 && up < 0) &&
            (left < 0 && down > 7) && (left < 0 && up < 0))
            break;
    }
    return possible_pos;
}

std::list<pos> Board::generatePossibleCombatPosQueen(const pos& startPos, const pos& prevPos) {
    const int dX = startPos.first - prevPos.first;
    const int dY = startPos.second - prevPos.second;
    std::list<pos> possible_pos;
    int right = startPos.first;
    int left = startPos.first;
    int up = startPos.second;
    int down = startPos.second;
    while (1) {
        ++right;
        --left;
        --up;
        ++down;
        if ((right <= 7 && down <= 7) && !(dX < 0 && dY < 0)) // (dX < 0 && dY < 0)
            possible_pos.push_back({ right, down });
        if ((right <= 7 && up >= 0) && !(dX < 0 && dY > 0))
            possible_pos.push_back({ right, up }); // (dX < 0 && dY > 0)
        if ((left >= 0 && down <= 7) && !(dX > 0 && dY < 0))
            possible_pos.push_back({ left, down }); //(dX > 0 && dY < 0)
        if ((left >= 0 && up >= 0) && !(dX > 0 && dY > 0))
            possible_pos.push_back({ left, up }); //(dX > 0 && dY > 0)
        if ((right > 7 && down > 7) && (right > 7 && up < 0) &&
            (left < 0 && down > 7) && (left < 0 && up < 0))
            break;
    }
    return possible_pos;
}

std::list<std::pair<pos, Board::MoveResult>> Board::generateAllMoves(
    const pos& startPos, bool direction) {
    std::list<std::pair<pos, Board::MoveResult>> moves;
    Board::MoveResult result = Board::MoveResult::PROHIBITED;
    if (!_cells.at(startPos).isQueen()) {
        auto possible_pos = {
            pos(startPos.first + 1, startPos.second + 1), // direction - false, move black
            pos(startPos.first - 1, startPos.second + 1), // direction - false
            pos(startPos.first + 1, startPos.second - 1), // direction - true, move white
            pos(startPos.first - 1, startPos.second - 1), // direction - true
            pos(startPos.first + 2, startPos.second + 2), // direction - never mind
            pos(startPos.first - 2, startPos.second + 2),
            pos(startPos.first + 2, startPos.second - 2),
            pos(startPos.first - 2, startPos.second - 2) };
        for (auto position : possible_pos) {
            result = checkMove(startPos, position, direction);
            if (result == Board::MoveResult::SUCCESSFUL_MOVE) {
                moves.push_back({ position, result });
            } else if (result == Board::MoveResult::SUCCESSFUL_COMBAT) {
                // попали ли на дамочное поле с первого боя, 
                // если да то ищем следующие ходы уже как дамка
                auto res = checkPiecesTransQueen(position);
                auto combatPos = pos(0, 0);
                if (res) {
                    _cells.at(position).setQueen(true);
                    const auto move = std::make_pair(startPos, position);
                    _movesTransInQueen.insert({ std::move(move), true });
                    combatPos = getPossibleCombatPos(position, startPos);
                    _cells.at(position).setQueen(false); // возвращаем исходное состояние позиции
                } else {
                    combatPos = getPossibleCombatPos(position, startPos);
                }
                if (combatPos != position)
                    moves.push_back({ combatPos, result });
                else
                    moves.push_back({ position, result });
            }
            result = Board::MoveResult::PROHIBITED;
        }
        return moves;
    } else { // queen pos
        auto possible_pos = generatePossiblePosQueen(startPos);
        for (auto position : possible_pos) {
            result = checkQueenMove(startPos, position);
            if (result == Board::MoveResult::SUCCESSFUL_MOVE) {
                moves.push_back({ position, result });
            } else if (result == Board::MoveResult::SUCCESSFUL_COMBAT) {
                _cells.at(position).setQueen(true);
                auto combatPos = getPossibleCombatPos(position, startPos);
                _cells.at(position).setQueen(false);
                if (combatPos != position)
                    moves.push_back({ combatPos, result });
                else
                    moves.push_back({ position, result });
            }
            result = Board::MoveResult::PROHIBITED;
        }
        return moves;
    }
}

pos Board::getVictimQueen(const pos& startPos, const pos& endPos) {
    const int dX = startPos.first - endPos.first;
    const int dY = startPos.second - endPos.second;
    auto victimCellState =  (_cells.at(startPos).getState() == Cell::State::WHITE) 
        ? Cell::State::BLACK : Cell::State::WHITE;
    auto start = startPos;
    while (start != endPos) {
        if (dX < 0 && dY < 0) {
            ++start.first;
            ++start.second;
        } else if (dX < 0 && dY > 0) {
            ++start.first;
            --start.second;
        } else if (dX > 0 && dY < 0) {
            --start.first;
            ++start.second;
        } else if (dX > 0 && dY > 0) {
            --start.first;
            --start.second;
        }
        auto curCell = start;
        auto currCellState = _cells.at(curCell).getState();
        if (currCellState == victimCellState)
            return curCell;
    }
}

void Board::makeMove(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos) {
    auto moveResult = endPos.second;
    const auto move = std::make_pair(startPos, endPos.first);
    switch (moveResult) {
    case Board::MoveResult::SUCCESSFUL_MOVE: {
        _cells.at(endPos.first).setState(_cells.at(startPos).getState());
        _cells.at(startPos).setState(Cell::State::BLANK);
        if (_cells.at(startPos).isQueen())
            _cells.at(endPos.first).setQueen(true);
        break;
    }
    case Board::MoveResult::SUCCESSFUL_COMBAT: {
        if (_cells.at(startPos).isQueen())
            _cells.at(endPos.first).setQueen(true);
        _cells.at(endPos.first).setState(_cells.at(startPos).getState());
        _cells.at(startPos).setState(Cell::State::BLANK);
        auto list_victim = _combatPieces.at(move);
        for (auto victim : list_victim)
            _cells.at(victim).setState(Cell::State::BLANK);
        break;
    }
    }
    auto itQueen = _movesTransInQueen.find(move);
    if (itQueen != _movesTransInQueen.end())
        _cells.at(endPos.first).setQueen(true);
}

void Board::revokeMove(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos) {
    auto moveResult = endPos.second;
    auto victimState = _cells.at(endPos.first).getState() == Cell::State::WHITE ?
        Cell::State::BLACK : Cell::State::WHITE;
    const auto move = std::make_pair(startPos, endPos.first);
    switch (moveResult) {
    case Board::MoveResult::SUCCESSFUL_MOVE: {
        _cells.at(startPos).setState(_cells.at(endPos.first).getState());
        _cells.at(endPos.first).setState(Cell::State::BLANK);
        if (_cells.at(endPos.first).isQueen()) {
            _cells.at(endPos.first).setQueen(false);
            _cells.at(startPos).setQueen(true);
        }
        break;
    }
    case Board::MoveResult::SUCCESSFUL_COMBAT: {
        _cells.at(startPos).setState(_cells.at(endPos.first).getState());
        _cells.at(endPos.first).setState(Cell::State::BLANK);
        auto list_victim = _combatPieces.at(move);
        for (auto victim : list_victim)
            _cells.at(victim).setState(victimState);

        if (_cells.at(endPos.first).isQueen()) {
            _cells.at(endPos.first).setQueen(false);
            _cells.at(startPos).setQueen(true);
        }
        break;
    }
    }
}

void Board::updateListPieces(
    const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos) {
    auto moveResult = endPos.second;
    switch (moveResult) {
    case Board::MoveResult::SUCCESSFUL_MOVE: {
        if (_cells.at(startPos).getState() == Cell::State::WHITE) {
            auto it = std::find(_whitePieces.begin(), _whitePieces.end(), startPos);
            _whitePieces.erase(it);
            _whitePieces.push_front(endPos.first);
        }
        else {
            auto it = std::find(_blackPieces.begin(), _blackPieces.end(), startPos);
            _blackPieces.erase(it);
            _blackPieces.push_front(endPos.first);
        }
        break;
    }
    case Board::MoveResult::SUCCESSFUL_COMBAT: {
        if (_cells.at(startPos).getState() == Cell::State::WHITE) {
            auto it = std::find(_whitePieces.begin(), _whitePieces.end(), startPos);
            _whitePieces.erase(it);
            _whitePieces.push_front(endPos.first);
            auto list_victim = _combatPieces.at(std::make_pair(startPos, endPos.first));
            for (auto victim : list_victim) {
                auto it_victim = std::find(_blackPieces.begin(), _blackPieces.end(), victim);
                _blackPieces.erase(it_victim);
            }          
        }
        else {
            auto it = std::find(_blackPieces.begin(), _blackPieces.end(), startPos);
            _blackPieces.erase(it);
            _blackPieces.push_front(endPos.first);
            auto list_victim = _combatPieces.at(std::make_pair(startPos, endPos.first));
            for (auto victim : list_victim) {
                auto it_victim = std::find(_whitePieces.begin(), _whitePieces.end(), victim);
                _whitePieces.erase(it_victim);
            }
        }
        break;
    }
    }
}

void Board::findAndSetBestPos(
    const std::list<std::pair<int, std::pair<pos, Board::MoveResult>>>& end_moves) {
    std::list<pos> pieces = _moveColor == true ? _whitePieces : _blackPieces;

    auto max = std::max_element(end_moves.begin(), end_moves.end());
    // check if there are options when you need to combat
    // нужно сделать так чтобы искал бой с лучшим счётом 
    auto max_combat_move = std::find_if(end_moves.begin(), end_moves.end(),
        [](const std::pair<int, std::pair<pos, Board::MoveResult>>& val)
    {return val.second.second == Board::MoveResult::SUCCESSFUL_COMBAT; });
    // if combat - best_move is combat
    std::pair<pos, Board::MoveResult> best_pos;
    if (max_combat_move != end_moves.end() &&
        max_combat_move->second.second == Board::MoveResult::SUCCESSFUL_COMBAT) {
        _endPosition = max_combat_move->second;
        auto distance = std::distance(end_moves.begin(), max_combat_move);
        auto it = pieces.begin();
        std::advance(it, distance);
        _startPosition = *it;
    }
    else {
        _endPosition = max->second;
        auto distance = std::distance(end_moves.begin(), max);
        auto it = pieces.begin();
        std::advance(it, distance);
        _startPosition = *it;
    }
}

void Board::minimaxRoot(const char player, size_t depth) {
    if (player != WHITE && player != BLACK)
        throw std::logic_error("Invalid input");

    std::list<pos> pieces = player == WHITE ? _whitePieces : _blackPieces;
    _moveColor = player == WHITE ? true : false;
    bool isMaximisingPlayer = _moveColor;

    std::list<std::pair<int, std::pair<pos, Board::MoveResult>>> endMoves;
    int bestScore = std::numeric_limits<int>::min();
    for (auto piece : pieces) {
        auto moves = generateAllMoves(piece, isMaximisingPlayer);
        if (moves.empty()) {
            //если вернул пустой вектор то игра проиграна, обработать ситуацию нужно
        }
        int score = std::numeric_limits<int>::min();
        auto bestMove = std::pair(pos(0, 0), Board::MoveResult::PROHIBITED);
        // если есть combat_move то стоит сделать массив только из них и оценивать их
        for (auto move : moves) {
            makeMove(piece, move);
            auto value = minimax(move.first, depth - 1,
                std::numeric_limits<int>::min(), 
                std::numeric_limits<int>::max(), 
                !isMaximisingPlayer);
            revokeMove(piece, move);
            if (value >= score) {
                score = value;
                bestMove = move;
            }
        }
        endMoves.push_back({ score, bestMove });
    }
    findAndSetBestPos(endMoves);
}

int Board::minimax(const pos& startPos, size_t depth,
    int alpha, int beta, bool isMaximisingPlayer) {
    if (depth == 0)
        return evaluate();

    auto moves = generateAllMoves(startPos, _moveColor);

    if (isMaximisingPlayer) {
        int bestScore = std::numeric_limits<int>::min();
        for (auto move : moves) {
            makeMove(startPos, move);
            bestScore = std::max(bestScore, minimax(
                 move.first, depth - 1, alpha, beta, !isMaximisingPlayer));
            revokeMove(startPos, move);
            alpha = std::max(alpha, bestScore);
            if (beta <= alpha) {
                return bestScore;
            }
        }
        return bestScore;
    } else {
        int bestScore = std::numeric_limits<int>::max();
        for (auto move : moves) {
            makeMove(startPos, move);
            bestScore = std::min(bestScore, minimax(
                move.first, depth - 1, alpha, beta, isMaximisingPlayer));
            revokeMove(startPos, move);
            beta = std::min(beta, bestScore);
            if (beta <= alpha) {
                return bestScore;
            }
        }
        return bestScore;
    }
}

int Board::evaluate() const {
    int whiteScore = 0;
    int blackScore = 0;
    for (auto i : _cells) {
        if (i.second.getState() == Cell::State::WHITE)
            whiteScore += _heuristic[i.first.first][i.first.second];
        else if (i.second.getState() == Cell::State::BLACK)
            blackScore += _heuristic[i.first.first][i.first.second];
    }
    int score = _moveColor == true ? (whiteScore - blackScore) : (blackScore - whiteScore);
    return score;
}

void Board::make_best_move() {
    updateListPieces(_startPosition, _endPosition);
    makeMove(_startPosition, _endPosition);
    if (_cells.at(_startPosition).isQueen())
        _cells.at(_startPosition).setQueen(false);
}

void Board::checkLineBoard(const std::string& line, size_t col) {
    if (line.size() > 17 || line[0] - '0' != col + 1 || line[1] != ' ') // size line symbol
        throw std::logic_error("Invalid input");

    const char space = ' ';
    const char whiteDiagonal = '_'; // Игра ведётся только на чёрных
    if (col % 2 == 0) { // start from '_'
        for (size_t i = 2; i < line.size(); i += 4) {
            if (i == 14) {
                if (line[i] != whiteDiagonal || line[i + 1] != space ||
                    (line[i + 2] != BLACK && line[i + 2] != WHITE && line[i + 2] != BLANK &&
                        line[i + 2] != BLACK_QUEEN && line[i + 2] != WHITE_QUEEN))
                    throw std::logic_error("Invalid input");
                break;
            }
            if (line[i] != whiteDiagonal || line[i + 1] != space || (line[i + 2] != BLACK
                && line[i + 2] != WHITE && line[i + 2] != BLANK && line[i + 2] != BLACK_QUEEN && 
                line[i + 2] != WHITE_QUEEN) || line[i + 3] != space)
                throw std::logic_error("Invalid input");
        }
    }
    else { // start from 'b' or 'w'
        for (size_t i = 2; i < line.size(); i += 4) {
            if (i == 14) {
                if ((line[i] != BLACK && line[i] != WHITE && line[i] != BLANK &&
                     line[i] != WHITE_QUEEN && line[i] != BLACK_QUEEN) ||
                    line[i + 1] != space || line[i + 2] != whiteDiagonal)
                    throw std::logic_error("Invalid input");
                break;
            } 
            if ((line[i] != BLACK && line[i] != WHITE && line[i] != BLANK&&
                     line[i] != WHITE_QUEEN && line[i] != BLACK_QUEEN)
                || line[i + 1] != space || line[i + 2] != whiteDiagonal || line[i + 3] != space)
                throw std::logic_error("Invalid input");
        }
    }
}

void Board::parsing(const std::string& filename) {
    std::ifstream ifs(filename);
    std::string line;
    size_t col = -1;
    size_t row = 0;
    const char space = ' ';
    const char whiteDiagonal = '_'; // Игра ведётся только на чёрных
    bool isQueen = false;
    while (std::getline(ifs, line)) {
        if (col == -1) { // check first str A B C D E F G H
            if (line[0] != space && line[1] != space)
                throw std::logic_error("Invalid input");

            int codeSymbol = 65; // code - 'A'
            for (size_t i = 2; i < line.size(); i += 2) {
                if (line[i] != codeSymbol || line[i + 1] != space)
                    if (line[i + 1] != space && i != 16) // else out of range
                        throw std::logic_error("Invalid input");
                ++codeSymbol;
            }
            ++col;
            continue;
        }
        if (col < _boardSize)
            checkLineBoard(line, col);
        if (col % 2 == 0)
            row = 1;
        else
            row = 0;
        for (size_t i = 0; i < line.size(); ++i) {
            auto state = Cell::State::BLANK;
            switch (line[i]) {
            case space: continue;
            case whiteDiagonal: continue;
            case BLANK: {
                const pos positions = pos(row, col);
                isQueen = false;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell)});
                break;
            }
            case WHITE: {
                const pos positions = pos(row, col);
                state = Cell::State::WHITE;
                isQueen = false;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
                _whitePieces.push_front(positions);
                break;
            }
            case BLACK: {
                const pos positions = pos(row, col);
                state = Cell::State::BLACK;
                isQueen = false;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
                _blackPieces.push_front(positions);
                break;
            }
            case WHITE_QUEEN: {
                const pos positions = pos(row, col);
                state = Cell::State::WHITE;
                isQueen = true;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
                _whitePieces.push_front(positions);
                break;
            }
            case BLACK_QUEEN: {
                const pos positions = pos(row, col);
                state = Cell::State::BLACK;
                isQueen = true;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
                _blackPieces.push_front(positions);
                break;
            }
            default: continue;
            }
            row += 2;
        }
        ++col;
        if (col == 8)
            break;
    }
    if (_whitePieces.size() > 12 || _whitePieces.size() == 0)
        throw std::logic_error("Invalid input, exceeded the number of white pieces");
    else if (_blackPieces.size() > 12 || _blackPieces.size() == 0)
        throw std::logic_error("Invalid input, exceeded the number of black pieces");
}

std::string Board::posToString(const pos& pos) {
    std::string str_pos;
    switch (pos.first) {
    case 0: {
        str_pos += 'A';
        break;
    }
    case 1: {
        str_pos += 'B';
        break;
    }
    case 2: {
        str_pos += 'C';
        break;
    }
    case 3: {
        str_pos += 'D';
        break;
    }
    case 4: {
        str_pos += 'E';
        break;
    }
    case 5: {
        str_pos += 'F';
        break;
    }
    case 6: {
        str_pos += 'G';
        break;
    }
    case 7: {
        str_pos += 'H';
        break;
    }
    }
    str_pos += std::to_string(pos.second + 1);
    return str_pos;
}

void Board::saveToFile(const std::string& filename) {
    std::ofstream out(filename);
    std::string outStr;
    size_t row = 0;
    size_t col = 0;
    bool flagSpace = true;
    outStr += "  A B C D E F G H\n";
    for (size_t i = 0; i < _cells.size(); i += 4) {
        if (col % 2 == 0) {
            row = 1;
            flagSpace = true;
        }
        else {
            row = 0;
            flagSpace = false;
        }
        if (flagSpace) {
            outStr += std::to_string(col + 1) + " _ " + _cells.at(pos(row, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 2, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 4, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 6, col)).getSymbolPieces() + '\n';
        } else {
            outStr += std::to_string(col + 1) + ' ' + _cells.at(pos(row, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 2, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 4, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 6, col)).getSymbolPieces()
                + " _\n";
        }
        ++col;
    }
    outStr += (_moveColor == true) ? "White`s move: " : "Black`s move: ";
    outStr += posToString(_startPosition) + "->" + posToString(_endPosition.first) + '\n';
    if (_blackPieces.size() == 0)
        outStr += "White won. Gane over!\n";
    else if (_whitePieces.size() == 0)
        outStr += "Black won. Gane over!\n";

    out << outStr;
}
