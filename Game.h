#ifndef GAME_INCLUDED
#define GAME_INCLUDED

#include "Well.h"
#include "UserInterface.h"
#include <string>
#include "Piece.h"
#include <queue>
// [Add other #include directives as necessary.]

class Game
{
  public:
    Game(int width, int height);
    void play();
    bool playOneLevel();
    void displayPrompt(std::string s);
    void displayStatus();
    // [Add other members as necessary.]
    void newLevel();
    
    int getCurrX() const;
    int getCurrY() const;
    
    void erase(int x, int y);
    void draw(int x, int y, char c);
    
    void immortalize();
    void performSpecialAction();
    void countClearedRows();
    void clearRows();
    
    void VaporBombAction();
    void FoamBombAction(int x, int y, int xUpperBoundary, int xLowerBoundary, int yUpperBoundary, int yLowerBoundary);
    
    void drawPiece();
    void erasePiece();
    
    void queuePieces();
    void spawn();
    void displayNextPiece();
    
    void performMoves(int& time);
    
    void move(int x, int y);
    void moveDown();
    void moveLeft();
    void moveRight();
    void rotate();
    
    bool overlap(int x, int y);
    
    PieceType getCurrentPieceType() const;
    
    
  private:
    Well    m_well;
    Screen  m_screen;
    int     m_level;
    int m_score;
    int rowsCleared;
    // [Add other members as necessary.]
    int curr_x;
    int curr_y;
    Piece* newPiece;
    PieceType currentPieceType;
    bool quit;
    bool spaceIsHit;
    std::queue<PieceType> pieceQueue;
    
};

#endif // GAME_INCLUDED
