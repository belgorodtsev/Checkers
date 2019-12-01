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
#include <list>
#include <limits>

class Board
{
public:
    enum class MoveResult {SUCCESSFUL_MOVE, SUCCESSFUL_COMBAT, PROHIBITED};

    Board();

    Board(const std::string& filename);

    ~Board() = default;
    // Сохраняет результат в файл
    void saveToFile(const std::string& filename);
    // Находит лучший ход для игрока, если player - 'w' для белых,
    // если - 'b' для черных. Depth - глубина просчёта
    void minimaxRoot(const char player, size_t depth);
    // Алгоритм минимакса
    int minimax(const pos& startPos, size_t depth, 
        int alpha, int beta, bool isMaximisingPlayer);
    // Делает лучший ход
    void make_best_move();

private:
    // Представление доски в виде мапы
    std::map<pos, Cell> _cells;
    // Список позиций белых фигур
    std::list<pos> _whitePieces;
    // Список позиций чёрных фигур
    std::list<pos> _blackPieces;
    // Позиция начала лучшего хода
    pos _startPosition;
    // Позиция лучшего хода
    std::pair<pos, Board::MoveResult> _endPosition;
    // Размер доски
    const size_t _boardSize = 8;
    // Равен true если ходят белые, false, если - черные
    bool _moveColor;
    // Enum для направления движения дамки
    enum class Way { rightDown, rihgtUp, leftDown, leftUp };
    // Мапа для хранения храния побитых шашек для конкретного кода
    std::map<std::pair<pos, pos>, std::list<pos>> _combatPieces;
    // Мапа для хранения ходов в которые шашка становится дамкой
    std::map<std::pair<pos, pos>, bool> _movesTransInQueen;
    // Парс входных данных из файла
    void parsing(const std::string& filename);
    // Проверка строки входного файла на валидность
    void checkLineBoard(const std::string& line, size_t col);
    // Перевод позици на доске в строку типа 0,0 -> А1
    std::string posToString(const pos& pos);
    // Проверка хода обычной шашки
    MoveResult checkMove(const pos& startPos, const pos& endPos, bool direction);
    // Проверка хода дамки
    MoveResult checkQueenMove(const pos& startPos, const pos& endPos);
    // Проверка хода дамки по конкретной диагонали, вызывается в методе checkQueenMove
    MoveResult checkDiagonalMovement(const pos& startPos, const pos& endPos, Board::Way w);
    // Возвращает возможный ход боя
    pos getPossibleCombatPos(const pos& startPos, const pos& prevPos);
    // Возвращает возможный ход боя для обычной шашки и записывает в 
    // список побитые шашки, вызывается в getPossibleCombatPos
    pos getCombatPosForPieces(const pos& startPos, const pos& prevPos, std::list<pos>& enemys);
    // Возвращает возможный ход боя для дамки и записывает в 
    // список побитые шашки, вызывается в getPossibleCombatPos
    pos getCombatPosForQueenPieces(const pos& startPos, const pos& prevPos, std::list<pos>& enemys);
    // Проверка на то стали ли обычная шашка дамкой в данной позиции
    bool checkPiecesTransQueen(const pos& endPos);
    // Возвращает позицию пешки которую побьет дамка сделав ход из startPos в endPos
    pos getVictimQueen(const pos& startPos, const pos& endPos);
    // Делает ход из startPos в endPos
    void makeMove(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos);
    // Возвращает сделаный ход из endPos в startPos
    void revokeMove(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos);
    // Ищет в списке позицию с максимальной оценкой и делает ее лучшим ходом
    void findAndSetBestPos(const std::list<std::pair<int, std::pair<pos, Board::MoveResult>>>& pos);
    // Обновляет списки фигур при ходе из startPos в endPos
    void updateListPieces(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos);
    // Возвращает список всех возможных ходов для дамки
    std::list<pos> generatePossiblePosQueen(const pos& startPos);
    // Возвращает список всех возможных ходов для дамки кроме тех позиций, 
    // откуда был сделан предидущий ход, используется для поиска хода боя
    std::list<pos> generatePossibleCombatPosQueen(const pos& startPos, const pos& prevPos);
    // Возвращает список всех возможных ходов с их результатом
    std::list<std::pair<pos, Board::MoveResult>> generateAllMoves(
        const pos& startPos, bool direction);
    // Оценка доски
    int evaluate() const;
    // Матрица для оценки
    const int _heuristic[8][8] =
        { {4,4,4,4,4,4,4,4},
        {4,3,3,3,3,3,3,4},
        {4,3,2,2,2,2,3,4},
        {4,3,2,1,1,2,3,4},
        {4,3,2,1,1,2,3,4},
        {4,3,2,2,2,2,3,4},
        {4,3,3,3,3,3,3,4},
        {4,4,4,4,4,4,4,4} };
};

