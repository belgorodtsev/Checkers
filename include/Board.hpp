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
    // ��������� ��������� � ����
    void saveToFile(const std::string& filename);
    // ������� ������ ��� ��� ������, ���� player - 'w' ��� �����,
    // ���� - 'b' ��� ������. Depth - ������� ��������
    void minimaxRoot(const char player, size_t depth);
    // �������� ���������
    int minimax(const pos& startPos, size_t depth, 
        int alpha, int beta, bool isMaximisingPlayer);
    // ������ ������ ���
    void make_best_move();

private:
    // ������������� ����� � ���� ����
    std::map<pos, Cell> _cells;
    // ������ ������� ����� �����
    std::list<pos> _whitePieces;
    // ������ ������� ������ �����
    std::list<pos> _blackPieces;
    // ������� ������ ������� ����
    pos _startPosition;
    // ������� ������� ����
    std::pair<pos, Board::MoveResult> _endPosition;
    // ������ �����
    const size_t _boardSize = 8;
    // ����� true ���� ����� �����, false, ���� - ������
    bool _moveColor;
    // Enum ��� ����������� �������� �����
    enum class Way { rightDown, rihgtUp, leftDown, leftUp };
    // ���� ��� �������� ������ ������� ����� ��� ����������� ����
    std::map<std::pair<pos, pos>, std::list<pos>> _combatPieces;
    // ���� ��� �������� ����� � ������� ����� ���������� ������
    std::map<std::pair<pos, pos>, bool> _movesTransInQueen;
    // ���� ������� ������ �� �����
    void parsing(const std::string& filename);
    // �������� ������ �������� ����� �� ����������
    void checkLineBoard(const std::string& line, size_t col);
    // ������� ������ �� ����� � ������ ���� 0,0 -> �1
    std::string posToString(const pos& pos);
    // �������� ���� ������� �����
    MoveResult checkMove(const pos& startPos, const pos& endPos, bool direction);
    // �������� ���� �����
    MoveResult checkQueenMove(const pos& startPos, const pos& endPos);
    // �������� ���� ����� �� ���������� ���������, ���������� � ������ checkQueenMove
    MoveResult checkDiagonalMovement(const pos& startPos, const pos& endPos, Board::Way w);
    // ���������� ��������� ��� ���
    pos getPossibleCombatPos(const pos& startPos, const pos& prevPos);
    // ���������� ��������� ��� ��� ��� ������� ����� � ���������� � 
    // ������ ������� �����, ���������� � getPossibleCombatPos
    pos getCombatPosForPieces(const pos& startPos, const pos& prevPos, std::list<pos>& enemys);
    // ���������� ��������� ��� ��� ��� ����� � ���������� � 
    // ������ ������� �����, ���������� � getPossibleCombatPos
    pos getCombatPosForQueenPieces(const pos& startPos, const pos& prevPos, std::list<pos>& enemys);
    // �������� �� �� ����� �� ������� ����� ������ � ������ �������
    bool checkPiecesTransQueen(const pos& endPos);
    // ���������� ������� ����� ������� ������ ����� ������ ��� �� startPos � endPos
    pos getVictimQueen(const pos& startPos, const pos& endPos);
    // ������ ��� �� startPos � endPos
    void makeMove(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos);
    // ���������� �������� ��� �� endPos � startPos
    void revokeMove(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos);
    // ���� � ������ ������� � ������������ ������� � ������ �� ������ �����
    void findAndSetBestPos(const std::list<std::pair<int, std::pair<pos, Board::MoveResult>>>& pos);
    // ��������� ������ ����� ��� ���� �� startPos � endPos
    void updateListPieces(const pos& startPos, const std::pair<pos, Board::MoveResult>& endPos);
    // ���������� ������ ���� ��������� ����� ��� �����
    std::list<pos> generatePossiblePosQueen(const pos& startPos);
    // ���������� ������ ���� ��������� ����� ��� ����� ����� ��� �������, 
    // ������ ��� ������ ���������� ���, ������������ ��� ������ ���� ���
    std::list<pos> generatePossibleCombatPosQueen(const pos& startPos, const pos& prevPos);
    // ���������� ������ ���� ��������� ����� � �� �����������
    std::list<std::pair<pos, Board::MoveResult>> generateAllMoves(
        const pos& startPos, bool direction);
    // ������ �����
    int evaluate() const;
    // ������� ��� ������
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

