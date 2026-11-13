#pragma once
#include "Piece.h"
#include "Player.h"
#include "GameLogger.h"

// ====================================================================
// POSITION CLASS
// ====================================================================
class Position { // row nd col holdkrna
private:
    int r, c;
public:
    Position() { r = 0; c = 0; }
    Position(int row, int col) { r = row; c = col; }
    int getRow() const { return r; }
    int getCol() const { return c; }
};

// ====================================================================
// MOVELIST CLASS
// ====================================================================
class MoveList { // jaha pr hamara piece move kr sakta hy, valid moves
private:
    Position moves[8];
    int count;
public:
    MoveList() { count = 0; }
    void add(int row, int col) {
        if (count < 8) { moves[count] = Position(row, col); count++; }
    }
    void clear()                        { count = 0;           }
    int      getCount()           const { return count;        }
    Position getMove(int index)   const { return moves[index]; }
};

// ====================================================================
// BOARD CLASS — implementations Board.cpp mein hain
// ====================================================================
class Board {
private:
    Piece* grid[8][8];

public:
    Board();
    ~Board();
    void   initializePieces();
    void   resetBoard();
    void   loadFromSnapshot(int layout[8][8]);
    Piece* getPiece(int r, int c) const;
    MoveList getValidMovesForPiece(int r, int c, PlayerColor currentTurn);

    bool movePiece(int startR, int startC, int endR, int endC,
                   PlayerColor currentTurn,
                   Player& redPlayer, Player& blackPlayer,
                   GameLogger& logger,
                   bool& outWasCapture,
                   bool& outWasKingPromotion);
};