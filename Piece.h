#pragma once

// Player ke colors
enum class PlayerColor { NONE, RED, BLACK }; //{0, 1, 2}

// --- BASE CLASS (Abstract) ---
class Piece {
protected:
    PlayerColor color;
    bool isKing;

public:
    Piece(PlayerColor c);
    virtual ~Piece() = default;

    PlayerColor getColor() const;
    bool getIsKing() const;
    void makeKing();

    // Pure Virtual Functions (Polymorphism)
    virtual char getSymbol() const = 0; // to differentiate b/w normal piece and the king
    virtual bool canMoveDirection(int rowDirection) const = 0; 
};

// --- CHILD CLASS 1 ---
class RedPiece : public Piece {
public:
    RedPiece();
    char getSymbol() const override; 
    bool canMoveDirection(int rowDirection) const override;
};

// --- CHILD CLASS 2 ---
class BlackPiece : public Piece {
public:
    BlackPiece();
    char getSymbol() const override; 
    bool canMoveDirection(int rowDirection) const override;
};