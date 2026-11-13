#pragma once
#include "Piece.h"
#include <string>
using namespace std;

// ====================================================================
// PLAYER CLASS
// ====================================================================
class Player {
private:
    string name;
    int score;
    PlayerColor color;
public:
    Player(string n, PlayerColor c) : name(n), score(0), color(c) {}

    void incrementScore() { score++; }
    int  getScore()       const { return score; }
    void setScore(int s)        { score = s;    }
    void resetScore()           { score = 0;    }
    string      getName()  const { return name;  }
    PlayerColor getColor() const { return color; }
};