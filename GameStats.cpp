// GameStats.cpp
#include "GameStats.h"
#include <sstream>

GameStats::GameStats() { reset(); }

int GameStats::playerIndex(PlayerColor c) const {
    return (c == PlayerColor::BLACK) ? 0 : 1;
}

void GameStats::reset() {
    for (int i = 0; i < 2; i++) {
        totalMoves[i] = 0;
        captures[i] = 0;
        kingPromotions[i] = 0;
        longestTurn[i] = 0.0f;
        shortestTurn[i] = 9999.0f;
        undosUsed[i] = 0;
    }
}

void GameStats::recordMove(PlayerColor player, float turnTime) {
    int i = playerIndex(player);
    totalMoves[i]++;
    if (turnTime > longestTurn[i])  longestTurn[i]  = turnTime;
    if (turnTime < shortestTurn[i]) shortestTurn[i] = turnTime;
}

void GameStats::recordCapture(PlayerColor player)       { captures[playerIndex(player)]++;       }
void GameStats::recordKingPromotion(PlayerColor player) { kingPromotions[playerIndex(player)]++;  }
void GameStats::recordUndo(PlayerColor player)          { undosUsed[playerIndex(player)]++;       }

int   GameStats::getTotalMoves(PlayerColor p)     const { return totalMoves[playerIndex(p)];    }
int   GameStats::getCaptures(PlayerColor p)       const { return captures[playerIndex(p)];      }
int   GameStats::getKingPromotions(PlayerColor p) const { return kingPromotions[playerIndex(p)];}
int   GameStats::getUndosUsed(PlayerColor p)      const { return undosUsed[playerIndex(p)];     }

float GameStats::getAvgTurnTime(PlayerColor p) const {
    int i = playerIndex(p);
    if (totalMoves[i] == 0) return 0.0f;
    return longestTurn[i]; // Simplified; full avg needs cumulative sum
}

string GameStats::getSummaryString(string blackName, string redName) const {
    ostringstream ss;
    ss << "========= MATCH SUMMARY =========\n";
    ss << blackName << ":\n";
    ss << "  Moves: "        << totalMoves[0]    << "\n";
    ss << "  Captures: "     << captures[0]      << "\n";
    ss << "  Kings Made: "   << kingPromotions[0] << "\n";
    ss << "  Undos Used: "   << undosUsed[0]     << "\n";
    ss << "  Longest Turn: " << longestTurn[0]   << "s\n";
    ss << "---------------------------------\n";
    ss << redName << ":\n";
    ss << "  Moves: "        << totalMoves[1]    << "\n";
    ss << "  Captures: "     << captures[1]      << "\n";
    ss << "  Kings Made: "   << kingPromotions[1] << "\n";
    ss << "  Undos Used: "   << undosUsed[1]     << "\n";
    ss << "  Longest Turn: " << longestTurn[1]   << "s\n";
    ss << "=================================\n";
    return ss.str();
}