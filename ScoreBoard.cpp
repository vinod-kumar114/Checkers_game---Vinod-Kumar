// ScoreBoard.cpp
#include "ScoreBoard.h"
#include <fstream>
#include <cstring>
#include <SFML/Graphics.hpp>

ScoreBoard::ScoreBoard(const char* file) {
    filename = file;
    count = 0;
    for (int i = 0; i < 10; i++) {
        strcpy(entries[i].name, "---");
        entries[i].wins = 0;
        entries[i].totalMoves = 0;
    }
    loadFromFile();
}

void ScoreBoard::loadFromFile() {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return;
    file.read((char*)&count, sizeof(int));
    if (count > 10) count = 10;
    file.read((char*)entries, sizeof(ScoreEntry) * count);
    file.close();
}

void ScoreBoard::saveToFile() {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) return;
    file.write((char*)&count, sizeof(int));
    file.write((char*)entries, sizeof(ScoreEntry) * count);
    file.close();
}

void ScoreBoard::sortEntries() {
    // Bubble sort by wins (descending)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (entries[j].wins < entries[j + 1].wins) {
                ScoreEntry temp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
            }
        }
    }
}

void ScoreBoard::addResult(string playerName, int wins, int moves) {
    // Check if player already exists
    for (int i = 0; i < count; i++) {
        if (string(entries[i].name) == playerName) {
            entries[i].wins += wins;
            entries[i].totalMoves += moves;
            sortEntries();
            saveToFile();
            return;
        }
    }
    // New player
    if (count < 10) {
        strncpy(entries[count].name, playerName.c_str(), 29);
        entries[count].wins = wins;
        entries[count].totalMoves = moves;
        count++;
        sortEntries();
        saveToFile();
    }
}

void ScoreBoard::display(sf::RenderWindow& window, sf::Font& font) {
    // Dark overlay background
    sf::RectangleShape bg({600.f, 500.f});
    bg.setPosition({100.f, 150.f});
    bg.setFillColor(sf::Color(20, 20, 30, 240));
    bg.setOutlineThickness(3.f);
    bg.setOutlineColor(sf::Color(255, 215, 0));
    window.draw(bg);

    sf::Text title(font);
    title.setString("=== LEADERBOARD ===");
    title.setCharacterSize(32);
    title.setFillColor(sf::Color(255, 215, 0));
    title.setPosition({180.f, 170.f});
    window.draw(title);

    sf::Text header(font);
    header.setString("Rank   Player          Wins   Moves");
    header.setCharacterSize(18);
    header.setFillColor(sf::Color(150, 150, 255));
    header.setPosition({120.f, 220.f});
    window.draw(header);

    for (int i = 0; i < count && i < 8; i++) {
        sf::Text row(font);
        string line = to_string(i + 1) + ".     " + 
                      string(entries[i].name) + "          " + 
                      to_string(entries[i].wins) + "      " + 
                      to_string(entries[i].totalMoves);
        row.setString(line);
        row.setCharacterSize(20);
        row.setFillColor(i == 0 ? sf::Color(255, 215, 0) : sf::Color::White);
        row.setPosition({120.f, 255.f + i * 35.f});
        window.draw(row);
    }

    sf::Text hint(font);
    hint.setString("Press L to close leaderboard");
    hint.setCharacterSize(16);
    hint.setFillColor(sf::Color(120, 120, 120));
    hint.setPosition({250.f, 610.f});
    window.draw(hint);
}

int ScoreBoard::getCount() const { return count; }
ScoreEntry ScoreBoard::getEntry(int i) const { return entries[i]; }