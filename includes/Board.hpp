#ifndef BOARD_HPP
#define BOARD_HPP

#include "Cell.hpp"
#include "Logger.hpp"
#include "Exception.hpp"
#include "ParsingUtils.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <stack>
#include <map>
#include <fstream>
#include <filesystem>
#include <list>
#include <limits>

class Board {
public:
    enum class MoveResult {SUCCESSFUL_MOVE, SUCCESSFUL_COMBAT, PROHIBITED};

    using moveWithResult = std::pair<pos, Board::MoveResult>;

    Board();

    Board(const std::string& in, const std::string& out);

    ~Board() = default;

    // Сохраняет результат в файл
    void save();

    // Находит лучший ход для игрока, если player - 'w' для белых,
    // если - 'b' для черных. Depth - глубина просчёта
    void calculateBestMove(const char color, size_t depth);

    // Делает лучший ход
    void makeBestMove();

    // Возвращает лучший ход для игрока
    moveWithResult getBestMove() const;

    posMove getMadeMove() const;

    // Закончилась ли игра
    bool isGameOver() const;

    // Возвращает true если ход валидный, делает его, false ничего не делает
    bool makeMove(const pos& startPos, const pos& endPos, const char color);

    // Вывод доски
    void print(std::ostream& out = std::cout) const;

    void print(std::ofstream& out) const;

private:
    // Пусть к файлу для сохраниния результатов
    std::string _outPath;

    // Представление доски в виде мапы
    std::map<pos, Cell> _cells;

    // Позиция начала лучшего хода
    pos _startPosition;

    // Позиция лучшего хода
    moveWithResult _endPosition;

    // флаг, true - игра закончена, false - иначе
    bool _isGameOver;

    // Размер доски
    const size_t _boardSize = 8;

    // Равен true если ходят белые, false, если - черные
    bool _moveColor;

    // Объект класса Logger для логирования
    // Получаем текущий путь процесса (a.k.a. working directory)
    // Конструируем путь с помощью перегруженного оператора /
    const std::filesystem::path logPath = std::filesystem::current_path() / "log.txt";
    mutable Logger _log;

    // Enum для направления движения дамки
    enum class Way { rightDown, rihgtUp, leftDown, leftUp };

    // Мапа для хранения ходов в которые шашка становится дамкой
    std::map<posMove, bool> _movesBecomeQueen;

    // Для записи истории сделанных ходов и листа побитых шашек при ходе
    std::stack<std::pair<pos, moveWithResult>> _historyMove;
    std::stack<std::list<std::pair<pos, Cell>>> _historyCombat;

    // Парс входных данных из файла, заполнение доски
    void parsing(const std::string& filename);

    // Заполнение доски, шашки устанавливаются на исходные позиции
    void setInitialPlacement();

    // Установить цвет шашки делающей ход
    void setMoveColor(const char color);

    // Есть ли на доске фигуры цвета color
    bool empty(const char color) const;

    // возвращяет список побитых фигур при ходе
    std::list<std::pair<pos, Cell>> getListCombatPieces(const pos& startPos, const pos& endPos);

    // Возвращает возможный ход боя
    std::pair<pos, std::list<std::pair<pos, Cell>>> 
        getPossibleCombatPos(const pos& startPos, const pos& prevPos);

    // Возвращает возможный ход боя для обычной шашки и записывает в 
    // список побитые шашки, вызывается в getPossibleCombatPos
    pos getCombatPosForPieces(const pos& startPos, const pos& prevPos, 
        std::list<std::pair<pos, Cell>>& enemys);

    // Возвращает возможный ход боя для дамки и записывает в 
    // список побитые шашки, вызывается в getPossibleCombatPos
    pos getCombatPosForQueenPieces(const pos& startPos, const pos& prevPos, 
        std::list<std::pair<pos, Cell>>& enemys);

    // Возвращает позицию пешки которую побьет дамка сделав ход из startPos в endPos
    pos getVictimQueen(const pos& startPos, const pos& endPos);

    // Проверка хода обычной шашки
    MoveResult checkMove(const pos& startPos, const pos& endPos, bool direction);

    // Проверка хода дамки
    MoveResult checkQueenMove(const pos& startPos, const pos& endPos);

    // Проверка хода дамки по конкретной диагонали, вызывается в методе checkQueenMove
    MoveResult checkDiagonalMovement(const pos& startPos, const pos& endPos, Board::Way w);

    // Проверка на то стали ли обычная шашка дамкой в данной позиции
    bool checkPiecesBecomeQueen(const pos& startPos, const pos& endPos) const;

    // Если в процессе хода шашка стала дамкой, устанавливаем состояние дамки в true
    void setStateQueen(const pos& startPos, const pos& endPos);

    // Если в процессе хода шашка стала дамкой, устанавливаем состояние дамки в false
    void unsetStateQueen(const pos& startPos, const pos& endPos);

    // Делает ход из startPos в endPos
    void makeMove(const pos& startPos, const moveWithResult& endPos);

    // Возвращает сделаный ход из endPos в startPos
    void revokeMove();

    // Ищет в списке позицию с максимальной оценкой и делает ее лучшим ходом
    void findAndSetBestPos(const std::list<std::pair<int, moveWithResult>>& pos);

    // Обновляет списки фигур при ходе из startPos в endPos
    std::list<pos> generateListPieces(bool color) const;

    // Возвращает список всех возможных ходов для дамки
    std::list<pos> generatePossiblePosQueen(const pos& startPos);

    // Возвращает список всех возможных ходов для дамки кроме тех позиций, 
    // откуда был сделан предидущий ход, используется для поиска хода боя
    std::list<pos> generatePossibleCombatPosQueen(const pos& startPos, const pos& prevPos);

    // Возвращает список всех возможных ходов с их результатом
    std::list<moveWithResult> generateAllMoves(const pos& startPos, bool color);

    // Возвращает бой, если он есть, или последний ход в списке возможных ходов 
    // для белых, первый - для чёрных. По хорошему нужно брать ход с минимальной
    // оценкой для для противника и с максимальной для себя (но их нужно считать)
    moveWithResult getMove(const std::list<moveWithResult>& list);

    //-------------------------AI----------------------------//
    // Находит лучший ход для игрока, depth - глубина просчёта
    std::list<std::pair<int, moveWithResult>> minimaxRoot(size_t depth);

    // Алгоритм минимакса
    int minimax(size_t depth, int alpha, int beta, bool isMaximisingPlayer);

    // Оценка доски
    int evaluate() const;

    // Матрица для оценки
    const int _heuristic[8][8] =
        { {4, 4, 4, 4, 4, 4, 4, 4},
        {4, 3, 3, 3, 3, 3, 3, 4},
        {4, 3, 2, 2, 2, 2, 3, 4},
        {4, 3, 2, 1, 1, 2, 3, 4},
        {4, 3, 2, 1, 1, 2, 3, 4},
        {4, 3, 2, 2, 2, 2, 3, 4},
        {4, 3, 3, 3, 3, 3, 3, 4},
        {4, 4, 4, 4, 4, 4, 4, 4} };
};

#endif  // BOARD_HPP