// GameStats.h
#pragma once
#include "Piece.h"
#include <string>
using namespace std;

class GameStats {
    // Per-player stats (index 0 = Black, index 1 = Red)
    int totalMoves[2];
    int captures[2];
    int kingPromotions[2];
    float longestTurn[2];   // in seconds
    float shortestTurn[2];
    int undosUsed[2];
    
    int playerIndex(PlayerColor c) const;  // Private helper

public:
    GameStats();
    void reset();
    
    void recordMove(PlayerColor player, float turnTime);
    void recordCapture(PlayerColor player);
    void recordKingPromotion(PlayerColor player);
    void recordUndo(PlayerColor player);
    
    // Getters
    int getTotalMoves(PlayerColor p) const;
    int getCaptures(PlayerColor p) const;
    int getKingPromotions(PlayerColor p) const;
    float getAvgTurnTime(PlayerColor p) const;
    int getUndosUsed(PlayerColor p) const;
    
    // Display post-game summary
    string getSummaryString(string blackName, string redName) const;
};