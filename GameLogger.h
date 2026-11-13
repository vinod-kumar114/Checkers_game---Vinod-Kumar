#pragma once
#include <fstream>
#include <string>
using namespace std;

// ====================================================================
// GAME LOGGER CLASS
// ====================================================================
class GameLogger {
private:
    ofstream logFile;
public:
    GameLogger() {
        logFile.open("gamelog.txt", ios::app);
        if (logFile.is_open()) {
            logFile << "\n====================================\n";
            logFile << "        NEW GAME STARTED            \n";
            logFile << "====================================\n";
        }
    }
    ~GameLogger() {
        if (logFile.is_open()) {
            logFile << "--- GAME ENDED ---\n";
            logFile.close();
        }
    }
    void logMove(string playerName, int sR, int sC, int eR, int eC, bool wasCapture) { // sR: starting row, sC: starting column; eR: ending row, eC: ending column
        if (logFile.is_open()) {
            logFile << playerName << " moved: (" << sR << "," << sC
                    << ") -> (" << eR << "," << eC << ")";
            if (wasCapture) logFile << "  *** CAPTURE! ***";
            logFile << "\n";
        }
    }
    void logUndo(string playerName) {
        if (logFile.is_open())
            logFile << "!!! " << playerName << " USED UNDO (15s Penalty) !!!\n";
    }
    void logWin(string playerName) {
        if (logFile.is_open())
            logFile << ">>> WINNER: " << playerName << " <<<\n";
    }
};