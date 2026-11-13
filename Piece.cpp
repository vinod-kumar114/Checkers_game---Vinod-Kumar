#include "Piece.h"

// ==========================================
// Base Class 'Piece' ke functions
// ==========================================
Piece::Piece(PlayerColor c) {
    color = c;
    isKing = false;
}

PlayerColor Piece::getColor() const { 
    return color; 
}

bool Piece::getIsKing() const { 
    return isKing; 
}

void Piece::makeKing() { 
    isKing = true; 
}

// ==========================================
// Child Class 'RedPiece' ke functions
// ==========================================
RedPiece::RedPiece() : Piece(PlayerColor::RED) {
}

char RedPiece::getSymbol() const {
    if (isKing) return 'R';
    else return 'r';
}

bool RedPiece::canMoveDirection(int rowDirection) const {
    if (isKing) return true; // King kahin bhi ja sakta hai
    if (rowDirection < 0) return true; // Normal Red sirf upar (-1, -2) jata hai
    return false;
}

// ==========================================
// Child Class 'BlackPiece' ke functions
// ==========================================
BlackPiece::BlackPiece() : Piece(PlayerColor::BLACK) {
}

char BlackPiece::getSymbol() const {
    if (isKing) return 'B';
    else return 'b';
}

bool BlackPiece::canMoveDirection(int rowDirection) const {
    if (isKing) return true; // King kahin bhi ja sakta hai
    if (rowDirection > 0) return true; // Normal Black sirf neeche (+1, +2) jata hai
    return false;
}