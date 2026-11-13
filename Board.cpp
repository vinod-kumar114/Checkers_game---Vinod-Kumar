#include "Board.h"
// Board.h already includes Player.h, GameLogger.h, Piece.h
// Isliye yahan kuch aur include karne ki zaroorat nahi

// ====================================================================
// CONSTRUCTOR
// ====================================================================
Board::Board() {
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            grid[row][col] = nullptr;
    initializePieces();
}

// ====================================================================
// DESTRUCTOR
// ====================================================================
Board::~Board() {
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            if (grid[row][col] != nullptr) {
                delete grid[row][col];
                grid[row][col] = nullptr;
            }
}

// ====================================================================
// STARTING POSITIONS pe pieces lagao
// ====================================================================
void Board::initializePieces() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if      (row < 3 && (row + col) % 2 != 0) grid[row][col] = new BlackPiece();
            else if (row > 4 && (row + col) % 2 != 0) grid[row][col] = new RedPiece();
        }
    }
}

// ====================================================================
// RESET
// ====================================================================
void Board::resetBoard() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (grid[row][col] != nullptr) {
                delete grid[row][col];
                grid[row][col] = nullptr;
            }
        }
    }
    initializePieces();
}

// ====================================================================
// SNAPSHOT SE RESTORE (undo ke liye)
// ====================================================================
void Board::loadFromSnapshot(int layout[8][8]) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (grid[r][c] != nullptr) { delete grid[r][c]; grid[r][c] = nullptr; }
            if      (layout[r][c] == 1) { grid[r][c] = new BlackPiece(); }
            else if (layout[r][c] == 2) { grid[r][c] = new BlackPiece(); grid[r][c]->makeKing(); }
            else if (layout[r][c] == 3) { grid[r][c] = new RedPiece();  }
            else if (layout[r][c] == 4) { grid[r][c] = new RedPiece();  grid[r][c]->makeKing(); }
        }
    }
}

// ====================================================================
// PIECE RETURN KARO
// ====================================================================
Piece* Board::getPiece(int r, int c) const {
    if (r >= 0 && r < 8 && c >= 0 && c < 8) return grid[r][c];
    return nullptr;
}

// ====================================================================
// MOVE EXECUTE KARO
// ====================================================================
bool Board::movePiece(int startR, int startC, int endR, int endC,
                      PlayerColor currentTurn,
                      Player& redPlayer, Player& blackPlayer,
                      GameLogger& logger,
                      bool& outWasCapture,
                      bool& outWasKingPromotion)
{
    outWasCapture       = false;
    outWasKingPromotion = false;

    Piece* p = grid[startR][startC];
    if (p == nullptr || p->getColor() != currentTurn) return false;
    if (grid[endR][endC] != nullptr)                  return false;

    int rowDiff = endR - startR;
    int colDiff = endC - startC;

    if (!p->canMoveDirection(rowDiff)) return false;

    if (abs(rowDiff) == 1 && abs(colDiff) == 1) {
        // Normal move — kuch nahi
    }
    else if (abs(rowDiff) == 2 && abs(colDiff) == 2) {
        int midR = startR + rowDiff / 2;
        int midC = startC + colDiff / 2;
        Piece* mid = grid[midR][midC];
        if (mid == nullptr || mid->getColor() == currentTurn) return false;

        delete grid[midR][midC];
        grid[midR][midC] = nullptr;

        if (currentTurn == PlayerColor::RED) redPlayer.incrementScore();
        else                                 blackPlayer.incrementScore();
        outWasCapture = true;
    }
    else { return false; }

    grid[endR][endC]     = p;
    grid[startR][startC] = nullptr;

    bool wasKing = p->getIsKing();
    if (p->getColor() == PlayerColor::RED   && endR == 0) p->makeKing();
    if (p->getColor() == PlayerColor::BLACK && endR == 7) p->makeKing();
    if (!wasKing && p->getIsKing()) outWasKingPromotion = true;

    string pName = (currentTurn == PlayerColor::RED)
                   ? redPlayer.getName() : blackPlayer.getName();
    logger.logMove(pName, startR, startC, endR, endC, outWasCapture);

    return true;
}

// ====================================================================
// VALID MOVES NIKALO
// ====================================================================
MoveList Board::getValidMovesForPiece(int r, int c, PlayerColor currentTurn) {
    MoveList moves;
    Piece* p = grid[r][c];
    if (p == nullptr || p->getColor() != currentTurn) return moves;

    int dirR[] = {-1, -1,  1, 1};
    int dirC[] = {-1,  1, -1, 1};

    for (int i = 0; i < 4; i++) {
        int s1r = r + dirR[i],      s1c = c + dirC[i];      // Offset calculation, for normal 
        int jpr = r + 2*dirR[i],    jpc = c + 2*dirC[i];    // for a jumping piece

        if (!p->canMoveDirection(dirR[i])) continue;

        // Normal step
        if (s1r>=0 && s1r<8 && s1c>=0 && s1c<8)
            if (grid[s1r][s1c] == nullptr)
                moves.add(s1r, s1c);

        // Jump step
        if (jpr>=0 && jpr<8 && jpc>=0 && jpc<8)
            if (grid[jpr][jpc] == nullptr) {
                Piece* mid = grid[s1r][s1c];
                if (mid != nullptr && mid->getColor() != p->getColor())
                    moves.add(jpr, jpc);
            }
    }
    return moves;
}