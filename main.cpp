#include <iostream>
#include <SFML/Graphics.hpp>
#include <optional>
#include "Piece.h"
#include "Player.h"       // Player class
#include "GameLogger.h"   // GameLogger class
#include "Board.h"        // Position, MoveList, Board class
#include "ScoreBoard.h"
#include "GameStats.h"

using namespace std;

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER };

// ====================================================================
// GAME SNAPSHOT (Memento Pattern)
// ====================================================================
struct GameSnapshot {
    
    int boardLayout[8][8]; // 0=Empty, 1=Black, 2=BlackKing, 3=Red, 4=RedKing
    int redScore;
    int blackScore;
    PlayerColor turn;
};

// ====================================================================
// CUSTOM STACK CLASS FOR UNDO
// ====================================================================
class HistoryStack {
private:
    GameSnapshot snapshots[100];
    int topIndex;
public:
    HistoryStack() { topIndex = -1; }

    void push(GameSnapshot snap) {
        if (topIndex < 99) {
            snapshots[++topIndex] = snap;
        }
    }

    GameSnapshot pop() {
        GameSnapshot lastSnap = snapshots[topIndex];
        topIndex--;
        return lastSnap;
    }

    bool isEmpty() const { return topIndex == -1; }
    void clear() { topIndex = -1; }
};

// GameLogger → GameLogger.h mein hai
// Player     → Player.h mein haim  b
// (Board.h dono ko include karta hai)

// Position, MoveList, Board — ab Board.h / Board.cpp mein hain
// ====================================================================
// MAIN FUNCTION
// ====================================================================
int main() {
    Board      gameBoard;
    GameLogger logger;

    Player blackPlayer("Black Team", PlayerColor::BLACK);
    Player redPlayer  ("Red Team",   PlayerColor::RED);

    // ── NEW OBJECTS ──────────────────────────────────────────────────
    ScoreBoard scoreBoard;   // Saves/loads scores from scores.dat
    GameStats stats;         // Tracks detailed match statistics
    bool showLeaderboard = false;  // Toggle with L key
    // ─────────────────────────────────────────────────────────────────

    PlayerColor currentTurn = PlayerColor::BLACK;
    GameState   currentState = GameState::MENU;

    sf::RenderWindow window(sf::VideoMode({800, 900}), "C++ OOP Checkers");
    sf::Font font;
    bool fontLoaded = font.openFromFile("arial.ttf");

    int currentTheme = 0;
    sf::Color lightSquares, darkSquares;

    sf::Clock turnClock;
    float blackTotalTime = 0.0f;
    float redTotalTime   = 0.0f;

    PlayerColor winnerColor = PlayerColor::NONE;
    int selectedRow = -1;
    int selectedCol = -1;
    MoveList possibleMoves;

    HistoryStack moveHistory;

    // ================================================================
    // MAIN LOOP
    // ================================================================
    while (window.isOpen()) {

        // ── EVENT HANDLING ──────────────────────────────────────────
        while (const std::optional<sf::Event> event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) window.close();

            // ── KEYBOARD ──────────────────────────────────────────
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                // --- MENU / PAUSED / GAME_OVER keys ---
                if (currentState == GameState::MENU ||
                    currentState == GameState::PAUSED ||
                    currentState == GameState::GAME_OVER)
                {
                    if (keyPressed->code == sf::Keyboard::Key::Num1) {
                        // Start / restart game
                        gameBoard.resetBoard();
                        blackPlayer.resetScore();
                        redPlayer.resetScore();
                        stats.reset();                  // NEW: reset stats for new game
                        currentTurn  = PlayerColor::BLACK;
                        winnerColor  = PlayerColor::NONE;
                        currentState = GameState::PLAYING;
                        blackTotalTime  = 0.0f;
                        redTotalTime    = 0.0f;
                        showLeaderboard = false;
                        turnClock.restart();
                        selectedRow = -1; selectedCol = -1;
                        moveHistory.clear();
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Num2 &&
                             currentState == GameState::PAUSED) {
                        currentState = GameState::PLAYING;
                        turnClock.restart();
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::Num3) {
                        window.close();
                    }
                    // Toggle leaderboard on MENU and GAME_OVER screens
                    else if (keyPressed->code == sf::Keyboard::Key::L) {
                        showLeaderboard = !showLeaderboard;
                    }
                }

                // --- PLAYING keys ---
                else if (currentState == GameState::PLAYING) {
                    if (keyPressed->code == sf::Keyboard::Key::Escape) {
                        currentState = GameState::PAUSED;
                        if (currentTurn == PlayerColor::BLACK)
                            blackTotalTime += turnClock.getElapsedTime().asSeconds();
                        else
                            redTotalTime += turnClock.getElapsedTime().asSeconds();
                    }

                    // T = cycle theme
                    if (keyPressed->code == sf::Keyboard::Key::T)
                        currentTheme = (currentTheme + 1) % 3;


                    // L = show/hide leaderboard overlay  (NEW)
                    if (keyPressed->code == sf::Keyboard::Key::L)
                        showLeaderboard = !showLeaderboard;
                }
            }

            // ── MOUSE CLICK ───────────────────────────────────────
            if (const auto* mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseClick->button == sf::Mouse::Button::Left) {

                    int clickedCol = mouseClick->position.x / 100;
                    int clickedRow = mouseClick->position.y / 100;

                    // ── HUD BUTTONS (bottom panel) ─────────────────
                    if (currentState == GameState::PLAYING &&
                        mouseClick->position.y >= 825 && mouseClick->position.y <= 865)
                    {
                        // THEME button
                        if (mouseClick->position.x >= 320 && mouseClick->position.x <= 400)
                            currentTheme = (currentTheme + 1) % 3;

                        // UNDO button
                        if (mouseClick->position.x >= 210 && mouseClick->position.x <= 290) {
                            if (!moveHistory.isEmpty()) {
                                GameSnapshot lastState = moveHistory.pop();

                                // 15-second penalty
                                if (currentTurn == PlayerColor::BLACK) blackTotalTime += 15.0f;
                                else                                    redTotalTime   += 15.0f;

                                string pName = (currentTurn == PlayerColor::RED)
                                               ? redPlayer.getName() : blackPlayer.getName();
                                logger.logUndo(pName);
                                stats.recordUndo(currentTurn);  // NEW: track undo in stats

                                gameBoard.loadFromSnapshot(lastState.boardLayout);
                                redPlayer.setScore(lastState.redScore);
                                blackPlayer.setScore(lastState.blackScore);
                                currentTurn = lastState.turn;

                                turnClock.restart();
                                selectedRow = -1; selectedCol = -1; possibleMoves.clear();
                            }
                        }

                        
                    }

                    // ── BOARD CLICKS ───────────────────────────────
                    if (currentState == GameState::PLAYING && clickedRow < 8) {

                        if (selectedRow == -1 && selectedCol == -1) {
                            // Select a piece
                            Piece* p = gameBoard.getPiece(clickedRow, clickedCol);
                            if (p != nullptr && p->getColor() == currentTurn) {
                                selectedRow = clickedRow; selectedCol = clickedCol;
                                possibleMoves = gameBoard.getValidMovesForPiece(
                                    selectedRow, selectedCol, currentTurn);
                            }
                        } else {
                            // Deselect same square
                            if (selectedRow == clickedRow && selectedCol == clickedCol) {
                                selectedRow = -1; selectedCol = -1; possibleMoves.clear();
                            } else {
                                // Attempt move — snapshot BEFORE the move
                                GameSnapshot snap;
                                snap.turn       = currentTurn;
                                snap.redScore   = redPlayer.getScore();
                                snap.blackScore = blackPlayer.getScore();
                                for (int r = 0; r < 8; r++) {
                                    for (int c = 0; c < 8; c++) {
                                        Piece* ps = gameBoard.getPiece(r, c);
                                        if (!ps)                                     snap.boardLayout[r][c] = 0;
                                        else if (ps->getColor() == PlayerColor::BLACK) snap.boardLayout[r][c] = ps->getIsKing() ? 2 : 1;
                                        else                                           snap.boardLayout[r][c] = ps->getIsKing() ? 4 : 3;
                                    }
                                }

                                // ── NEW: capture/king flags ────────
                                bool wasCapture      = false;
                                bool wasKingPromotion = false;

                                bool success = gameBoard.movePiece(
                                    selectedRow, selectedCol,
                                    clickedRow,  clickedCol,
                                    currentTurn,
                                    redPlayer, blackPlayer,
                                    logger,
                                    wasCapture,       // NEW param
                                    wasKingPromotion  // NEW param
                                );

                                if (success) {
                                    moveHistory.push(snap);

                                    // ── NEW: record stats ──────────
                                    float turnTime = turnClock.getElapsedTime().asSeconds();
                                    stats.recordMove(currentTurn, turnTime);
                                    if (wasCapture)       stats.recordCapture(currentTurn);
                                    if (wasKingPromotion) stats.recordKingPromotion(currentTurn);


                                    // Update timers
                                    if (currentTurn == PlayerColor::BLACK)
                                        blackTotalTime += turnTime;
                                    else
                                        redTotalTime += turnTime;
                                    turnClock.restart();

                                    // Switch turn
                                    currentTurn = (currentTurn == PlayerColor::BLACK)
                                                  ? PlayerColor::RED : PlayerColor::BLACK;

                                    // Win check
                                    if (redPlayer.getScore() == 12) {
                                        winnerColor  = PlayerColor::RED;
                                        currentState = GameState::GAME_OVER;
                                        logger.logWin(redPlayer.getName());

                                        scoreBoard.addResult(
                                            redPlayer.getName(), 1,
                                            stats.getTotalMoves(PlayerColor::RED));
                                    }
                                    else if (blackPlayer.getScore() == 12) {
                                        winnerColor  = PlayerColor::BLACK;
                                        currentState = GameState::GAME_OVER;
                                        logger.logWin(blackPlayer.getName());

                                        scoreBoard.addResult(
                                            blackPlayer.getName(), 1,
                                            stats.getTotalMoves(PlayerColor::BLACK));
                                    }
                                }
                                selectedRow = -1; selectedCol = -1; possibleMoves.clear();
                            }
                        }
                    }
                }
            }
        } // end event loop

        // ── THEME COLORS ────────────────────────────────────────────
        if      (currentTheme == 0) { lightSquares = sf::Color(240,217,181); darkSquares = sf::Color(181,136, 99); }
        else if (currentTheme == 1) { lightSquares = sf::Color(220,220,220); darkSquares = sf::Color( 80, 80, 80); }
        else                        { lightSquares = sf::Color(173,216,230); darkSquares = sf::Color(  0,  0,139); }

        window.clear(sf::Color(30, 30, 40));

        // ================================================================
        // DRAW — MENU / PAUSED
        // ================================================================
        if (currentState == GameState::MENU || currentState == GameState::PAUSED) {
            if (fontLoaded) {
                sf::Text title(font);
                title.setString(currentState == GameState::MENU ? "OOP CHECKERS" : "GAME PAUSED");
                title.setCharacterSize(60); title.setFillColor(sf::Color::White);
                title.setPosition({180.f, 200.f}); window.draw(title);

                sf::Text opt1(font); opt1.setString("Press 1 to Start New Game");
                opt1.setCharacterSize(30); opt1.setFillColor(sf::Color::Cyan);
                opt1.setPosition({220.f, 350.f}); window.draw(opt1);

                if (currentState == GameState::PAUSED) {
                    sf::Text opt2(font); opt2.setString("Press 2 to Resume");
                    opt2.setCharacterSize(30); opt2.setFillColor(sf::Color::Green);
                    opt2.setPosition({220.f, 420.f}); window.draw(opt2);
                }

                sf::Text opt3(font); opt3.setString("Press 3 to Exit");
                opt3.setCharacterSize(30); opt3.setFillColor(sf::Color::Red);
                opt3.setPosition({220.f, 490.f}); window.draw(opt3);

                // NEW: leaderboard hint on menu
                sf::Text lHint(font); lHint.setString("Press L to view Leaderboard");
                lHint.setCharacterSize(22); lHint.setFillColor(sf::Color::Yellow);
                lHint.setPosition({220.f, 560.f}); window.draw(lHint);

                // NEW: draw leaderboard overlay if toggled
                if (showLeaderboard) scoreBoard.display(window, font);
            }
        }

        // ================================================================
        // DRAW — GAME OVER
        // ================================================================
        else if (currentState == GameState::GAME_OVER) {
            if (fontLoaded) {
                sf::Text title(font);
                title.setString(winnerColor == PlayerColor::RED ? "RED WINS!" : "BLACK WINS!");
                title.setCharacterSize(70);
                title.setFillColor(winnerColor == PlayerColor::RED
                                   ? sf::Color(255,100,100) : sf::Color(150,150,150));
                title.setPosition({180.f, 120.f}); window.draw(title);

                // NEW: show match stats summary
                if (fontLoaded) {
                    string summary = stats.getSummaryString(
                        blackPlayer.getName(), redPlayer.getName());
                    // Draw summary line by line
                    int lineY = 220;
                    string line;
                    for (char ch : summary) {
                        if (ch == '\n') {
                            sf::Text lineText(font); lineText.setString(line);
                            lineText.setCharacterSize(16);
                            lineText.setFillColor(sf::Color(200,200,200));
                            lineText.setPosition({80.f, (float)lineY});
                            window.draw(lineText);
                            lineY += 22; line.clear();
                        } else { line += ch; }
                    }
                }

                sf::Text opt1(font); opt1.setString("Press 1 to Play Again");
                opt1.setCharacterSize(26); opt1.setFillColor(sf::Color::Cyan);
                opt1.setPosition({250.f, 680.f}); window.draw(opt1);

                sf::Text opt3(font); opt3.setString("Press 3 to Exit");
                opt3.setCharacterSize(26); opt3.setFillColor(sf::Color::Red);
                opt3.setPosition({250.f, 720.f}); window.draw(opt3);

                sf::Text lHint(font); lHint.setString("Press L to view Leaderboard");
                lHint.setCharacterSize(20); lHint.setFillColor(sf::Color::Yellow);
                lHint.setPosition({250.f, 760.f}); window.draw(lHint);

                // NEW: draw leaderboard overlay if toggled
                if (showLeaderboard) scoreBoard.display(window, font);
            }
        }

        // ================================================================
        // DRAW — PLAYING
        // ================================================================
        else if (currentState == GameState::PLAYING) {

            // -- Board squares --
            for (int r = 0; r < 8; r++) {
                for (int c = 0; c < 8; c++) {
                    sf::RectangleShape square({100.f, 100.f});
                    square.setPosition({c * 100.f, r * 100.f});
                    square.setFillColor((r + c) % 2 == 0 ? lightSquares : darkSquares);
                    window.draw(square);
                }
            }

            // -- Valid move highlights --
            for (int i = 0; i < possibleMoves.getCount(); i++) {
                sf::RectangleShape hl({100.f, 100.f});
                hl.setPosition({possibleMoves.getMove(i).getCol() * 100.f,
                                possibleMoves.getMove(i).getRow() * 100.f});
                hl.setFillColor(sf::Color(0, 255, 0, 100));
                window.draw(hl);
            }

            // -- Selected piece highlight --
            if (selectedRow != -1 && selectedCol != -1) {
                sf::RectangleShape hl({100.f, 100.f});
                hl.setPosition({selectedCol * 100.f, selectedRow * 100.f});
                hl.setFillColor(sf::Color(255, 255, 0, 150));
                window.draw(hl);
            }

            // -- Pieces --
            for (int r = 0; r < 8; r++) {
                for (int c = 0; c < 8; c++) {
                    Piece* p = gameBoard.getPiece(r, c);
                    if (p != nullptr) {
                        sf::CircleShape pieceShape(40.f);
                        pieceShape.setPosition({c * 100.f + 10.f, r * 100.f + 10.f});
                        pieceShape.setFillColor(p->getColor() == PlayerColor::RED
                                                ? sf::Color(200,50,50) : sf::Color(40,40,40));
                        pieceShape.setOutlineThickness(2.f);
                        pieceShape.setOutlineColor(sf::Color(20,20,20));
                        window.draw(pieceShape);

                        if (p->getIsKing()) {
                            sf::ConvexShape crown; crown.setPointCount(7);
                            crown.setPoint(0, sf::Vector2f(-18.f,  12.f));
                            crown.setPoint(1, sf::Vector2f( 18.f,  12.f));
                            crown.setPoint(2, sf::Vector2f( 22.f, -15.f));
                            crown.setPoint(3, sf::Vector2f(  8.f,  -2.f));
                            crown.setPoint(4, sf::Vector2f(  0.f, -22.f));
                            crown.setPoint(5, sf::Vector2f( -8.f,  -2.f));
                            crown.setPoint(6, sf::Vector2f(-22.f, -15.f));
                            crown.setFillColor(sf::Color(255,215,0));
                            crown.setPosition({c * 100.f + 50.f, r * 100.f + 50.f});
                            window.draw(crown);
                        }
                    }
                }
            }

            // -- HUD Panel --
            sf::RectangleShape hudPanel({800.f, 100.f});
            hudPanel.setPosition({0.f, 800.f});
            hudPanel.setFillColor(sf::Color(25, 25, 35));
            window.draw(hudPanel);

            // UNDO button
            sf::RectangleShape undoBtn({80.f, 40.f}); undoBtn.setPosition({210.f, 825.f});
            undoBtn.setFillColor(sf::Color(100,50,50)); undoBtn.setOutlineThickness(2.f);
            undoBtn.setOutlineColor(sf::Color::White); window.draw(undoBtn);

            // THEME button
            sf::RectangleShape themeBtn({80.f, 40.f}); themeBtn.setPosition({320.f, 825.f});
            themeBtn.setFillColor(sf::Color(60,60,80)); themeBtn.setOutlineThickness(2.f);
            themeBtn.setOutlineColor(sf::Color::White); window.draw(themeBtn);

            // NEW: SOUND button
            if (fontLoaded) {
                sf::Text uText(font); uText.setString("UNDO");
                uText.setCharacterSize(16); uText.setFillColor(sf::Color::White);
                uText.setPosition({225.f, 835.f}); window.draw(uText);

                sf::Text tText(font); tText.setString("THEME");
                tText.setCharacterSize(16); tText.setFillColor(sf::Color::White);
                tText.setPosition({330.f, 835.f}); window.draw(tText);

                sf::Text sText(font); sText.setString("SOUND");
                sText.setCharacterSize(16); sText.setFillColor(sf::Color::White);
                sText.setPosition({427.f, 835.f}); window.draw(sText);

                // Turn indicator
                sf::Text turnText(font); turnText.setCharacterSize(22);
                turnText.setString((currentTurn == PlayerColor::BLACK) ? "TURN: BLACK" : "TURN: RED");
                turnText.setFillColor((currentTurn == PlayerColor::BLACK)
                                      ? sf::Color(150,150,150) : sf::Color(255,100,100));
                turnText.setPosition({15.f, 830.f}); window.draw(turnText);

                // Score
                sf::Text scoreText(font); scoreText.setCharacterSize(18);
                scoreText.setString("Score\nB=" + to_string(blackPlayer.getScore()) +
                                    "  R=" + to_string(redPlayer.getScore()));
                scoreText.setFillColor(sf::Color::White);
                scoreText.setPosition({530.f, 810.f}); window.draw(scoreText);

                // Timers
                float bDisp = blackTotalTime + (currentTurn == PlayerColor::BLACK
                              ? turnClock.getElapsedTime().asSeconds() : 0);
                float rDisp = redTotalTime   + (currentTurn == PlayerColor::RED
                              ? turnClock.getElapsedTime().asSeconds() : 0);
                int bM = (int)bDisp/60, bS = (int)bDisp%60;
                int rM = (int)rDisp/60, rS = (int)rDisp%60;

                sf::Text timeText(font); timeText.setCharacterSize(14);
                timeText.setString("B: " + to_string(bM) + "m" + to_string(bS) +
                                   "s  R: " + to_string(rM) + "m" + to_string(rS) + "s");
                timeText.setFillColor(sf::Color::Yellow);
                timeText.setPosition({530.f, 848.f}); window.draw(timeText);

                // Hint bar
                sf::Text hintText(font); hintText.setCharacterSize(11);
                hintText.setString("ESC=Pause | L=Leaderboard | T=Theme | Undo adds 15s penalty");
                hintText.setFillColor(sf::Color(120,120,120));
                hintText.setPosition({10.f, 878.f}); window.draw(hintText);

                // NEW: draw leaderboard overlay if toggled during play
                if (showLeaderboard) scoreBoard.display(window, font);
            }
        }

        window.display();
    }
    return 0;
}