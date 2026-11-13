# C++ OOP Checkers

A 2-player (same computer) **Checkers / Draughts** game for Windows, written in **C++** with a graphical window made using **SFML 3**. It was built as an Object-Oriented Programming project, so the code is organised into classes (Board, Piece, Player, ScoreBoard, GameStats, GameLogger, etc.).

---

## 1. What is this?

A classic 8x8 Checkers game where two people take turns on the same PC using the mouse.

- **Black Team** starts at the top of the board and moves first.
- **Red Team** starts at the bottom.
- Pieces move diagonally. A piece that reaches the opposite end becomes a **King** and can move in both directions.
- You capture an opponent's piece by jumping over it.
- The first team to capture **all 12** enemy pieces wins.

### Features

- Graphical 800x900 window with a main menu, pause screen and game-over screen
- Highlighting of valid moves for the selected piece
- **Undo** button (costs a **15-second time penalty**)
- Separate turn timers for both teams
- **3 board themes** (Classic wood, Grey/Black, Blue)
- Match summary at the end (moves, captures, kings made, undos used, longest turn)
- **Leaderboard** that is saved to a file between sessions
- Every move, undo and win is written to a log file

---

## 2. How to play

### Just want to play? (no installation needed)

1. Extract the whole zip. Keep all files in the **same folder**.
2. Double-click **`main.exe`**.

That's it. All required DLLs and the font are already included. Works on 64-bit Windows 10 / 11.

> Do not move `main.exe` alone to another folder. It needs the `.dll` files and `arial.ttf` next to it.

### Controls

| Action | Control |
|---|---|
| Start new game | Press **1** (on menu / pause / game-over screen) |
| Resume game | Press **2** (on pause screen) |
| Exit | Press **3** |
| Pause | **Esc** (during game) |
| Show / hide leaderboard | **L** |
| Change theme | **T** key or the **THEME** button |
| Undo last move | Click the **UNDO** button (bottom panel) |
| Select a piece | Left-click on your own piece |
| Move | Left-click on a highlighted destination square |
| Deselect | Click the same piece again |

### Rules used in this version

- Black moves first, then turns alternate.
- Normal pieces move 1 square diagonally forward; a jump moves 2 squares over an enemy piece.
- Kings can move and jump diagonally in all 4 directions.
- Win condition: capture all 12 enemy pieces.

---

## 3. What is inside the folder

| File | Purpose |
|---|---|
| `main.cpp` | Game loop, menus, drawing, mouse/keyboard handling, undo stack |
| `Board.h / Board.cpp` | 8x8 board, move validation, captures, king promotion |
| `Piece.h / Piece.cpp` | Abstract `Piece` class with `RedPiece` and `BlackPiece` child classes |
| `Player.h` | Player name, colour and score |
| `GameStats.h / GameStats.cpp` | Per-player match statistics |
| `ScoreBoard.h / ScoreBoard.cpp` | Leaderboard with file save/load (`scores.dat`) |
| `GameLogger.h` | Writes moves, undos and winner to `gamelog.txt` |
| `main.exe` | Pre-compiled game (ready to run) |
| `arial.ttf` | Font used by the game |
| `sfml-*-3.dll`, `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll` | Runtime libraries needed by `main.exe` |

Files created automatically when you play: **`scores.dat`** (leaderboard) and **`gamelog.txt`** (game log). Delete them any time to reset.

---

## 4. Concepts and technologies used

**Language / library**
- C++ (C++17)
- SFML 3 (Graphics, Window, System modules) for the window, shapes, text, mouse and keyboard
- Standard library: `fstream`, `string`, `sstream`, `optional`

**OOP concepts**
- **Classes and encapsulation**: private data with public getters/setters (`Board`, `Player`, `GameStats`, `ScoreBoard`)
- **Inheritance**: `RedPiece` and `BlackPiece` derive from `Piece`
- **Abstract class and polymorphism**: `Piece` has pure virtual functions `getSymbol()` and `canMoveDirection()`
- **Constructors and destructors**: `Board` frees all dynamically allocated pieces in its destructor (no memory leaks)
- **Dynamic memory**: `new` / `delete` for pieces
- **Enum classes**: `PlayerColor`, `GameState`

**Data structures and design**
- **Custom Stack** (`HistoryStack`) for the undo feature (fixed array, no `std::stack`)
- **Memento pattern** (`GameSnapshot`): the board is saved before each move so Undo can restore it
- **State machine**: `MENU`, `PLAYING`, `PAUSED`, `GAME_OVER`
- Fixed-size arrays (`MoveList`, leaderboard entries) instead of `vector`
- Bubble sort for ranking the leaderboard
- **File handling**: binary file for scores, text file for logs

---

## 5. How to build it yourself (optional)

You only need this if you want to **edit the code** and recompile.

### Install first

1. **MSYS2** (gives you the g++ compiler): https://www.msys2.org
   Then open the **MSYS2 UCRT64** terminal and run:
   ```
   pacman -S mingw-w64-ucrt-x86_64-gcc
   ```
2. **SFML 3.x** for GCC (UCRT, 64-bit): https://www.sfml-dev.org/download.php
   Extract it to `C:\SFML` so that these folders exist: `C:\SFML\include` and `C:\SFML\lib`.
3. *(Optional)* **Visual Studio Code** with the "C/C++" extension by Microsoft.

> The SFML version and compiler must match. Use SFML **3.x** built for **GCC UCRT 64-bit**, otherwise you will get linker errors or a "missing DLL" error.

### Compile

Open a terminal inside this folder and run:

```
C:\msys64\ucrt64\bin\g++.exe -std=c++17 *.cpp -o main.exe -I C:\SFML\include -L C:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system
```

(If `g++` is already in your PATH, you can just write `g++` instead of the full path.)

### Run

Make sure these are next to `main.exe`: `arial.ttf` and the SFML / MinGW DLLs (copy them from `C:\SFML\bin` and `C:\msys64\ucrt64\bin` if needed), then run:

```
main.exe
```

### Common problems

| Problem | Fix |
|---|---|
| "The code execution cannot proceed because ... .dll was not found" | Keep all `.dll` files in the same folder as `main.exe` |
| Window opens but text is missing | `arial.ttf` is not in the same folder as `main.exe` |
| `SFML/Graphics.hpp: No such file` | The `-I` path is wrong, or SFML is not extracted to `C:\SFML` |
| Linker errors (`undefined reference to sf::...`) | SFML version or compiler does not match (need SFML 3 + GCC UCRT) |

---

## 6. Known limitations

- The **SOUND** button is only drawn on the screen; no audio is implemented yet.
- Only a single jump per turn (no multi-jump chains) and captures are not compulsory.
- The game ends only when a team captures all 12 pieces (a fully blocked position is not detected).
- The leaderboard is keyed by team name ("Black Team" / "Red Team"), so it effectively tracks these two teams.
- Windows only (the included `main.exe` and DLLs are Windows builds; the source itself can be compiled on other systems with SFML 3).
