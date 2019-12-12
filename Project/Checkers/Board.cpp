#include "Board.hpp"
#include <IO.hpp>

Board::Board() :
    _startPosition(pos(0, 0)),
    _endPosition({ pos(0,0) ,Board::MoveResult::PROHIBITED }),
    _isGameOver(false),
    _log(logPath.string()) {
    setInitialPlacement();
}

Board::Board(const std::string& in, const std::string& out) :
    _startPosition(pos(0, 0)),
    _endPosition({ pos(0,0) ,Board::MoveResult::PROHIBITED }),
    _isGameOver(false),
    _log(logPath.string()),
    _outPath(out) {
    parsing(in);
}

bool Board::isGameOver() const {
    return _isGameOver;
}

bool Board::empty(const char color) const {
    auto findState = color == WHITE ? Cell::State::WHITE : Cell::State::BLACK;
    for (auto i : _cells) {
        if (i.second.getState() == findState)
            return false;
    }
    return true;
}

bool Board::checkPiecesBecomeQueen(const pos& startPos, const pos& endPos) const {
    if (_cells.at(startPos).isQueen())
        return false;
    if (_cells.at(startPos).getState() == Cell::State::WHITE) {
        if (endPos.second == 0)
            return true;
    }
    else if (_cells.at(startPos).getState() == Cell::State::BLACK) {
        if (endPos.second == 7)
            return true;
    }
    return false;
}

Board::MoveResult Board::checkMove(const pos& startPos, const pos& endPos, bool color) {
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
            } else if ((abs(dX) == 1 && dY == 1 && color) ||
                (abs(dX) == 1 && dY == -1 && !color)) {
                result = Board::MoveResult::SUCCESSFUL_MOVE;
                // проверяем стенет ли в конечной позиции пешка 
                // дамкой, если да, то добавляем этот ход в мапу 
                if (checkPiecesBecomeQueen(startPos, endPos)) {
                    const auto move = std::make_pair(startPos, endPos);
                    _movesBecomeQueen.insert({ std::move(move), true });
                }
            }
        }
    }
    return result;
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
        }
        else if (w == Board::Way::rihgtUp) {
            ++start.first;
            --start.second;
        }
        else if (w == Board::Way::leftDown) {
            --start.first;
            ++start.second;
        }
        else if (w == Board::Way::leftUp) {
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

pos Board::getVictimQueen(const pos& startPos, const pos& endPos) {
    const int dX = startPos.first - endPos.first;
    const int dY = startPos.second - endPos.second;
    auto victimCellState = (_cells.at(startPos).getState() == Cell::State::WHITE)
        ? Cell::State::BLACK : Cell::State::WHITE;
    auto start = startPos;
    while (start != endPos) {
        if (dX < 0 && dY < 0) {
            ++start.first;
            ++start.second;
        }
        else if (dX < 0 && dY > 0) {
            ++start.first;
            --start.second;
        }
        else if (dX > 0 && dY < 0) {
            --start.first;
            ++start.second;
        }
        else if (dX > 0 && dY > 0) {
            --start.first;
            --start.second;
        }
        auto curCell = start;
        auto currCellState = _cells.at(curCell).getState();
        if (currCellState == victimCellState)
            return curCell;
    }
}

pos Board::getCombatPosForPieces(const pos& startPos, const pos& prevPos,
    std::list<std::pair<pos, Cell>>& enemys) {
    auto startCellState = _cells.at(prevPos).getState();
    pos endPos = pos(0, 0);
    pos victimPos = pos(0, 0);
    std::swap(_cells.at(startPos), _cells.at(prevPos));
    auto possiblePos = {
            pos(startPos.first + 2, startPos.second + 2), // direction - never mind
            pos(startPos.first + 2, startPos.second - 2),
            pos(startPos.first - 2, startPos.second + 2),
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
                        enemys.push_back({ victimPos, _cells.at(victimPos) });
                        if (checkPiecesBecomeQueen(startPos, position)) {
                            _cells.at(position).setQueen(true);
                            endPos = getCombatPosForQueenPieces(position, startPos, enemys);
                            _cells.at(position).setQueen(false);
                            if (endPos == position)
                                break;
                        } else {
                            endPos = getCombatPosForPieces(position, startPos, enemys);
                            if (endPos == position)
                                break;
                        }
                    }
                }
            }
        }
    }
    std::swap(_cells.at(startPos), _cells.at(prevPos));
    if (endPos == pos(0, 0))
        return startPos;
    return endPos;
}

pos Board::getCombatPosForQueenPieces(const pos& startPos, const pos& prevPos, 
    std::list<std::pair<pos, Cell>>& enemys) {
    auto startCellState = _cells.at(prevPos).getState();
    pos endPos = pos(0, 0);
    auto possible_pos = generatePossibleCombatPosQueen(startPos, prevPos);
    if (possible_pos.empty())
        return startPos;
    // перемещаем пешку по ходу боя
    std::swap(_cells.at(startPos), _cells.at(prevPos));
    for (auto position : possible_pos) {
        auto check = checkQueenMove(startPos, position);
        if (check == Board::MoveResult::SUCCESSFUL_COMBAT) {
             // поиск побитой пешки
             auto victimPos = getVictimQueen(startPos, position);
             auto result = getCombatPosForQueenPieces(position, startPos, enemys);
             auto it_enemy = std::find_if(enemys.begin(), enemys.end(), 
                 [&] (const std::pair<pos, Cell>& val) {return val.first == victimPos; });
             if (it_enemy == enemys.end()) {
                 enemys.push_back({ victimPos, _cells.at(victimPos) });
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

std::pair<pos, std::list<std::pair<pos, Cell>>> Board::getPossibleCombatPos(
    const pos& startPos, const pos& prevPos) {
    pos endPos = pos(0, 0);
    std::list<std::pair<pos, Cell>> enemys(0);
    if (!_cells.at(startPos).isQueen()) {
        // добавляем побитую шашку
        auto victimPos = pos((prevPos.first + startPos.first) / 2,
            (prevPos.second + startPos.second) / 2);
        enemys.push_back({ victimPos, _cells.at(victimPos) });
        auto combatPos = getCombatPosForPieces(startPos, prevPos, enemys);
        const auto move = posMove(prevPos, combatPos);
        // если попали в дамочное поле то устанавливаем состояни едамки обратно
        if (_cells.at(combatPos).isQueen()) {
            // добавляем в мапу перехода в дамку
            _movesBecomeQueen.insert({ std::move(move), true });
            _cells.at(combatPos).setQueen(false); // возвращаем исходное состояние позиции
        }
        return { combatPos, enemys };

    } else { // бой для дамки
        auto victimPos = getVictimQueen(prevPos, startPos);
        enemys.push_back({ victimPos, _cells.at(victimPos) });
        auto combatPos = getCombatPosForQueenPieces(startPos, prevPos, enemys);
        const auto move = posMove(prevPos, combatPos);
        return { combatPos, enemys };
    }
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
    std::list<pos> possible_pos(0);
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

std::list<pos> Board::generateListPieces(bool color) const {
    auto findState = color ? Cell::State::WHITE : Cell::State::BLACK;
    std::list<pos> pieces(0);
    for (auto i : _cells) {
        if (i.second.getState() == findState)
            pieces.push_back(i.first);
    }
    return pieces;
}

std::list<Board::moveWithResult> Board::generateAllMoves(const pos& startPos, bool color) {
    std::list<moveWithResult> moves(0);
    Board::MoveResult result = Board::MoveResult::PROHIBITED;
    if (!_cells.at(startPos).isQueen()) { // обычная пешка
        auto possible_pos = {
            pos(startPos.first + 1, startPos.second + 1), // color - false, move black
            pos(startPos.first - 1, startPos.second + 1), // color - false
            pos(startPos.first + 1, startPos.second - 1), // color - true, move white
            pos(startPos.first - 1, startPos.second - 1), // color - true
            pos(startPos.first + 2, startPos.second + 2), // color - never mind
            pos(startPos.first + 2, startPos.second - 2),
            pos(startPos.first - 2, startPos.second + 2),
            pos(startPos.first - 2, startPos.second - 2) };
        for (auto position : possible_pos) {
            result = checkMove(startPos, position, color);
            if (result == Board::MoveResult::SUCCESSFUL_MOVE) {
                moves.push_back({ position, result });
            } else if (result == Board::MoveResult::SUCCESSFUL_COMBAT) {
                // попали ли на дамочное поле с первого боя, 
                // если да то ищем следующие ходы уже как дамка
                if (checkPiecesBecomeQueen(startPos, position)) {
                    _cells.at(position).setQueen(true);
                    auto combatPos = getPossibleCombatPos(position, startPos);
                    const auto move = posMove(startPos, combatPos.first);
                    // добавляем в соответствующую мапу ход при котором пешка становится дамкой
                    _movesBecomeQueen.insert({ std::move(move), true });
                    _cells.at(position).setQueen(false); // возвращаем исходное состояние позиции
                    moves.push_back({ combatPos.first, result });
                } else {
                    auto combatPos = getPossibleCombatPos(position, startPos);
                    moves.push_back({ combatPos.first, result });
                }
            }
            result = Board::MoveResult::PROHIBITED;
        }
        return moves;
    } else { // если дамка
        auto possible_pos = generatePossiblePosQueen(startPos);
        for (auto position : possible_pos) {
            result = checkQueenMove(startPos, position);
            if (result == Board::MoveResult::SUCCESSFUL_MOVE) {
                moves.push_back({ position, result });
            } else if (result == Board::MoveResult::SUCCESSFUL_COMBAT) {
                _cells.at(position).setQueen(true);
                auto combatPos = getPossibleCombatPos(position, startPos);
                _cells.at(position).setQueen(false);
                moves.push_back({ combatPos.first, result });
            }
            result = Board::MoveResult::PROHIBITED;
        }
        return moves;
    }
}

std::list<std::pair<pos, Cell>> Board::getListCombatPieces(
    const pos& startPos, const pos& endPos) {
    auto victimState = _cells.at(startPos).getState() == Cell::State::WHITE ?
        Cell::State::BLACK : Cell::State::WHITE;
    Board::MoveResult result = Board::MoveResult::PROHIBITED;
    if (!_cells.at(startPos).isQueen()) { // обычная пешка
        auto possible_pos = {
            pos(startPos.first + 2, startPos.second + 2),
            pos(startPos.first + 2, startPos.second - 2),
            pos(startPos.first - 2, startPos.second + 2),
            pos(startPos.first - 2, startPos.second - 2) };
        for (auto position : possible_pos) {
            result = checkMove(startPos, position, true);
            if (result == Board::MoveResult::SUCCESSFUL_COMBAT) {
                // попали ли на дамочное поле с первого боя, 
                // если да то ищем следующие ходы уже как дамка
                if (checkPiecesBecomeQueen(startPos, position)) {
                    _cells.at(position).setQueen(true);
                    auto combatPos = getPossibleCombatPos(position, startPos);
                    _cells.at(position).setQueen(false); // возвращаем исходное состояние позиции
                    if (combatPos.first == endPos)
                        return combatPos.second;
                } else {
                    auto combatPos = getPossibleCombatPos(position, startPos);
                    if (combatPos.first == endPos)
                        return combatPos.second;
                }
            }
            result = Board::MoveResult::PROHIBITED;
        }
    } else { // если дамка
        auto possible_pos = generatePossiblePosQueen(startPos);
        for (auto position : possible_pos) {
            result = checkQueenMove(startPos, position);
            if (result == Board::MoveResult::SUCCESSFUL_COMBAT) {
                _cells.at(position).setQueen(true);
                auto combatPos = getPossibleCombatPos(position, startPos);
                _cells.at(position).setQueen(false);
                if (combatPos.first == endPos)
                    return combatPos.second;
            }
            result = Board::MoveResult::PROHIBITED;
        }
    }
}

void Board::setStateQueen(const pos& startPos, const pos& endPos) {
    auto itQueen = _movesBecomeQueen.find(posMove(startPos, endPos));
    if (itQueen != _movesBecomeQueen.end())
        _cells.at(endPos).setQueen(true);
}

void Board::unsetStateQueen(const pos& startPos, const pos& endPos) {
    // Если пешка стала дамкой в процессе хода то убираем состояние дамки
    auto itQueen = _movesBecomeQueen.find(posMove(startPos, endPos));
    if (itQueen != _movesBecomeQueen.end())
        _cells.at(endPos).setQueen(false);
}

void Board::makeMove(const pos& startPos, const moveWithResult& endPos) {
    auto moveResult = endPos.second;
    auto blankCell = Cell(Cell::State::BLANK, false);
    switch (moveResult) {
    case Board::MoveResult::SUCCESSFUL_MOVE: {
        _cells.at(endPos.first) = _cells.at(startPos);
        _cells.at(startPos) = blankCell;
        setStateQueen(startPos, endPos.first);
        break;
    }
    case Board::MoveResult::SUCCESSFUL_COMBAT: {
        auto listVictim = getListCombatPieces(startPos, endPos.first);
        _historyCombat.push(listVictim);
        for (auto victim : listVictim)
            _cells[victim.first] = blankCell;
        _cells.at(endPos.first) = _cells.at(startPos);
        _cells.at(startPos) = blankCell;
        setStateQueen(startPos, endPos.first);
        break;
    }
    }
    _historyMove.push({ startPos, endPos });
}

void Board::revokeMove() {
    auto endPos = _historyMove.top().second;
    auto startPos = _historyMove.top().first;
    _historyMove.pop();
    auto moveResult = endPos.second;
    auto blankCell = Cell(Cell::State::BLANK, false);
    switch (moveResult) {
    case Board::MoveResult::SUCCESSFUL_MOVE: {
        unsetStateQueen(startPos, endPos.first);
        _cells.at(startPos) = _cells.at(endPos.first);
        _cells.at(endPos.first) = blankCell;
        break;
    }
    case Board::MoveResult::SUCCESSFUL_COMBAT: { 
        unsetStateQueen(startPos, endPos.first);
        _cells.at(startPos) = _cells.at(endPos.first);
        _cells.at(endPos.first) = blankCell;
        auto listVictim = _historyCombat.top();
        _historyCombat.pop();
        for (auto victim : listVictim)
            _cells[victim.first] = victim.second;
        break;
    }
    }
}

void Board::findAndSetBestPos(const std::list<std::pair<int, moveWithResult>>& end_moves) {
    auto max = end_moves.begin();

    auto countProhibited = std::count_if(end_moves.begin(), end_moves.end(),
        [](const std::pair<int, moveWithResult>& val)
    {return val.second.second == Board::MoveResult::PROHIBITED; });
    if (countProhibited == end_moves.size()) {
        _isGameOver = true;
        return;
    }

    // есть ли варианты боя, то ищем лучшую оценку среди них
    auto countCombat = std::count_if(end_moves.begin(), end_moves.end(),
        [](const std::pair<int, moveWithResult>& val)
    {return val.second.second == Board::MoveResult::SUCCESSFUL_COMBAT; });

    if (countCombat != 0) {
        // возьмём минимальный score, относительно которого будем искать максимальную оценку для боя
        auto score = std::numeric_limits<int>::min();
        for (auto it = end_moves.begin(); it != end_moves.end(); ++it) {
            if ((score < it->first) && (it->second.second == Board::MoveResult::SUCCESSFUL_COMBAT)) {
                max = it;
                score = it->first;
            }
        }
    } else {
        max = std::max_element(end_moves.begin(), end_moves.end());
    }
    _endPosition = max->second;
    auto distance = std::distance(end_moves.begin(), max);
    // находин начальную позицию лучшего хода в листе пешек
    auto pieces = generateListPieces(_moveColor);
    auto it = pieces.begin();
    std::advance(it, distance);
    _startPosition = *it;
}

Board::moveWithResult Board::getMove(const std::list<Board::moveWithResult>& list) {
    auto it = std::find_if(list.begin(), list.end(),
        [](const moveWithResult& val)
    {return val.second == Board::MoveResult::SUCCESSFUL_COMBAT; });

    if (it == list.end()) {
        if (_moveColor)
            return list.front();
        else
            return list.back();
    } else {
        return *it;
    }
}

void Board::setMoveColor(const char color) {
    try {
        if (color != WHITE && color != BLACK)
            throw Exception("Invalid input!");
    } catch (Exception& except) {
        except.fail(_log, _outPath);
    }
    _moveColor = color == WHITE ? true : false;
}

// ------------------------- AI ------------------------------ // 
void Board::calculateBestMove(const char color, size_t depth) {
    if (_cells.empty())
        return;

    setMoveColor(color);
    auto endMoves = minimaxRoot(depth);
    findAndSetBestPos(endMoves);
}

std::list<std::pair<int, Board::moveWithResult>> Board::minimaxRoot(size_t depth) {
    // список лучших ходов для каждой шашки
    std::list<std::pair<int, moveWithResult>> endMoves(0);
    bool isMaximisingPlayer = true;
    auto pieces = generateListPieces(_moveColor);
    int bestScore = std::numeric_limits<int>::min();
    for (auto piece : pieces) {
        auto moves = generateAllMoves(piece, _moveColor);
        auto bestMove = std::pair(pos(0, 0), Board::MoveResult::PROHIBITED);
        int score = std::numeric_limits<int>::min();
        if (moves.empty()) {
            endMoves.push_back({ score, bestMove });
            continue;
        }
        for (auto move : moves) {
            makeMove(piece, move);
            auto value = minimax(
                !_moveColor,
                depth - 1,
                std::numeric_limits<int>::min(), 
                std::numeric_limits<int>::max(), 
                !isMaximisingPlayer);
            revokeMove();
            if (value >= score) {
                score = value;
                bestMove = move;
            }
        }
        endMoves.push_back({ score, bestMove });
    }
    return endMoves;
}

int Board::minimax(bool color, size_t depth, int alpha, int beta, bool isMaximisingPlayer) {
    if (depth == 0)
        return evaluate();

    auto pieces = generateListPieces(color);

    if (isMaximisingPlayer) {
        int bestScore = std::numeric_limits<int>::min();
        for (auto piece : pieces) {
            auto moves = generateAllMoves(piece, isMaximisingPlayer);
            if (moves.empty())
                continue;
            auto move = getMove(moves);
            makeMove(piece, move);
            bestScore = std::max(bestScore,
                minimax(!color, depth - 1, alpha, beta, !isMaximisingPlayer));
            revokeMove();
            alpha = std::max(alpha, bestScore);
            if (beta <= alpha)
                return bestScore;
        }
        return bestScore;
    } else {
        int bestScore = std::numeric_limits<int>::max();
        for (auto piece : pieces) {
            auto moves = generateAllMoves(piece, isMaximisingPlayer);
            if (moves.empty())
                continue;
            auto move = getMove(moves);
            makeMove(piece, move);
            bestScore = std::min(bestScore,
                minimax(!color, depth - 1, alpha, beta, !isMaximisingPlayer));
            revokeMove();
            beta = std::min(beta, bestScore);
            if (beta <= alpha)
                return bestScore;
        }
        return bestScore;
    }
}

int Board::evaluate() const {
    int whiteScore = 0;
    int blackScore = 0;
    for (auto i : _cells) {
        if (i.second.getState() == Cell::State::WHITE) {
            if (_cells.at(i.first).isQueen())
                whiteScore += _heuristic[i.first.first][i.first.second] * 2;
            else
                whiteScore += _heuristic[i.first.first][i.first.second];
        } else if (i.second.getState() == Cell::State::BLACK) {
            if (_cells.at(i.first).isQueen())
                blackScore += _heuristic[i.first.first][i.first.second] * 2;
            else
                blackScore += _heuristic[i.first.first][i.first.second];
        }
    }
    int score = _moveColor ? (whiteScore - blackScore) : (blackScore - whiteScore);
    return score;
}
//----------------------------------------------------------//

std::pair<pos, Board::MoveResult> Board::getBestMove() const {
    try {
        if (_endPosition == std::make_pair(pos(0, 0), Board::MoveResult::PROHIBITED))
            throw Exception("Position not calculated!");
    } catch (Exception& except) {
        except.fail(_log, _outPath);
    }
    return _endPosition;
}

posMove Board::getMadeMove() const {
    try {
        if (_endPosition == std::make_pair(pos(0, 0), Board::MoveResult::PROHIBITED))
            throw Exception("Position not calculated!");
    }
    catch (Exception& except) {
        except.fail(_log, _outPath);
    }
    return { _startPosition, _endPosition.first };
}

void Board::makeBestMove() {
    if (_cells.empty())
        return;
    // если _isGameOver = true значит нет возможных ходом игра закончена
    if (_isGameOver) {
        _log << INFO << ((_moveColor) ?
            "Black won, no possible moves. Game over!" :
            "White won, no possible moves. Game over!") << std::endl;
        return;
    }
    try {
        if ((_endPosition == std::make_pair(pos(0, 0), Board::MoveResult::PROHIBITED) ||
            _startPosition == pos(0, 0)))
            throw Exception("Position not calculated!");
    } catch (Exception& except) {
        except.fail(_log, _outPath);
    }
    makeMove(_startPosition, _endPosition);
    if (empty(WHITE) || empty(BLACK))
        _isGameOver = true;
    // Запись хода в лог
    auto colorInfo = (_moveColor) ? "White`s move:" : "Black`s move:";
    _log << INFO << (colorInfo + pu::posToString(_startPosition) + "->" 
        + pu::posToString(_endPosition.first)) << std::endl;
    if (_moveColor && empty(BLACK))
        _log << INFO << "White won. Game over!" << std::endl;
    else if (!_moveColor && empty(WHITE))
        _log << INFO << "Black won. Game over!" << std::endl;
}

bool Board::makeMove(const pos& startPos, const pos& endPos, const char color) {
    // Для того чтобы проверить код, найдём список всех возможных ходов из начальной
    // позиции, если список пуст, возвращаем false, если среди найенных нет endPos, 
    // возвращаем false, находим - true. Генерация всех ходов необходима для того, 
    // чтобы заполнить мапы побитых фигур и для проверки возможности хода
    setMoveColor(color);
    bool direction = (color == BLACK) ? false : true;
    auto moves = generateAllMoves(startPos, direction);

    if (moves.empty())
        return false;

    auto it = std::find_if(moves.begin(), moves.end(),
        [&](const moveWithResult& val)
    {return val.first == endPos; });

    if (it != moves.end()) {
        makeMove(startPos, { endPos, it->second });
        if (empty(WHITE) || empty(BLACK))
            _isGameOver = true;
        // Запись хода в лог
        auto colorInfo = (_moveColor) ? "White`s move:" : "Black`s move:";
        _log << INFO << (colorInfo + pu::posToString(startPos) + "->" 
            + pu::posToString(endPos)) << std::endl;
        if (_moveColor && empty(BLACK))
            _log << INFO << "White won. Game over!" << std::endl;
        else if (!_isGameOver && empty(WHITE))
            _log << INFO << "Black won. Game over!" << std::endl;
        return true;
    } else {
        return false;
    }
}

void Board::parsing(const std::string& filename) {
    std::ifstream ifs(filename);
    std::string line;
    size_t countW = 0;
    size_t countB = 0;
    size_t col = -1;
    size_t row = 0;
    const char space = ' ';
    const char whiteDiagonal = '_'; // Игра ведётся только на чёрных
    bool isQueen = false;
    while (std::getline(ifs, line)) {
        if (col == -1) { // check first str A B C D E F G H
            if (!pu::checkFirstLine(line, _log, _outPath))
                return;
            ++col;
            continue;
        }
        if (col < _boardSize)
            if (!pu::checkLineBoard(line, col, _log, _outPath)) {
                _cells.clear();
                return;
            }
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
                ++countW;
                break;
            }
            case BLACK: {
                const pos positions = pos(row, col);
                state = Cell::State::BLACK;
                isQueen = false;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
                ++countB;
                break;
            }
            case WHITE_QUEEN: {
                const pos positions = pos(row, col);
                state = Cell::State::WHITE;
                isQueen = true;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
                ++countW;
                break;
            }
            case BLACK_QUEEN: {
                const pos positions = pos(row, col);
                state = Cell::State::BLACK;
                isQueen = true;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
                ++countB;
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
    try {
        if (countW > 12 || countW == 0)
            throw Exception("Invalid input, exceeded the number of white pieces!");
        if (countB > 12 || countB == 0)
            throw Exception("Invalid input, exceeded the number of black pieces!");
    } catch (Exception& except) {
        except.fail(_log, _outPath);
        _cells.clear();
        return;
    }
}

void Board::setInitialPlacement() {
    size_t col = 0;
    size_t row = 0;
    auto isQueen = false;
    while (col != _boardSize) {
        if (col % 2 == 0)
            row = 1;
        else
            row = 0;
        for (size_t i = 0; i < 4; ++i) {
            if (col < 3) {
                const pos positions = pos(row, col);
                auto state = Cell::State::BLACK;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
            } else if (col > 4) {
                const pos positions = pos(row, col);
                auto state = Cell::State::WHITE;
                isQueen = false;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
            } else {
                const pos positions = pos(row, col);
                auto state = Cell::State::BLANK;
                isQueen = false;
                auto cell = Cell(state, isQueen);
                _cells.insert({ std::move(positions), std::move(cell) });
            }
            row += 2;
        }
        ++col;
    }
}

void Board::print(std::ostream& out) const {
    size_t row = 0;
    size_t col = 0;
    bool flagSpace = true;
    out << "  A B C D E F G H\n";
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
            out << std::to_string(col + 1) + " _ " + _cells.at(pos(row, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 2, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 4, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 6, col)).getSymbolPieces() + '\n';
        }
        else {
            out << std::to_string(col + 1) + ' ' + _cells.at(pos(row, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 2, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 4, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 6, col)).getSymbolPieces()
                + " _\n";
        }
        ++col;
    }
}

void Board::print(std::ofstream& out) const {
    size_t row = 0;
    size_t col = 0;
    bool flagSpace = true;
    out << "  A B C D E F G H\n";
    for (size_t i = 0; i < _cells.size(); i += 4) {
        if (col % 2 == 0) {
            row = 1;
            flagSpace = true;
        } else {
            row = 0;
            flagSpace = false;
        }
        if (flagSpace) {
            out << std::to_string(col + 1) + " _ " + _cells.at(pos(row, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 2, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 4, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 6, col)).getSymbolPieces() + '\n';
        } else {
            out << std::to_string(col + 1) + ' ' + _cells.at(pos(row, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 2, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 4, col)).getSymbolPieces()
                + " _ " + _cells.at(pos(row + 6, col)).getSymbolPieces()
                + " _\n";
        }
        ++col;
    }
}

void Board::save() {
    if (_cells.empty())
        return;
    std::ofstream out(_outPath);
    print(out);
    // если нет возможных ходов
    if (_isGameOver && _endPosition.first == pos(0, 0)) {
        if (_moveColor)
            out << "White won, no possible moves. Game over!\n";
        else
            out << "Black won, no possible moves. Game over!\n";
        return;
    }
    auto colorInfo = (_moveColor) ? "White`s move:" : "Black`s move:";
    out << colorInfo + pu::posToString(_startPosition) + "->" + pu::posToString(_endPosition.first) + '\n';
    
    if (_isGameOver && _moveColor)
        out << "White won. Game over!\n";
    else if (_isGameOver && !_moveColor)
        out << "Black won. Game over!\n";
}
