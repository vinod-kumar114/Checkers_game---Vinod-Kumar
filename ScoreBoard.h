// ScoreBoard.h
#pragma once
#include <string>
#include <SFML/Graphics.hpp>
using namespace std;

struct ScoreEntry {
    char name[30];
    int wins;
    int totalMoves;
};

class ScoreBoard {
private:
    ScoreEntry entries[10];   // Fixed array, no vector!
    int count;
    const char* filename;

    void sortEntries();       // Private helper - encapsulation

public:
    ScoreBoard(const char* file = "scores.dat");
    void loadFromFile();
    void saveToFile();
    void addResult(string playerName, int wins, int moves);
    void display(sf::RenderWindow& window, sf::Font& font);  // SFML display
    int getCount() const;
    ScoreEntry getEntry(int i) const;
};