#include "Game.h"
#include "Well.h"
#include "UserInterface.h"
#include <string>
#include <vector>


const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;

const int WELL_X = 0;
const int WELL_Y = 0;

const int PROMPT_Y = 20;
const int PROMPT_X = 0;

const int SCORE_X = 16;
const int SCORE_Y = 8;

const int ROWS_LEFT_X = 16;
const int ROWS_LEFT_Y = 9;

const int LEVEL_X = 16;
const int LEVEL_Y = 10;

const int NEXT_PIECE_TITLE_X = 16;
const int NEXT_PIECE_TITLE_Y = 3;

const int NEXT_PIECE_X = 16;
const int NEXT_PIECE_Y = 4;

const int RIGHTJUSTIFIED = 33;

Game::Game(int width, int height)
: m_well(width, height), m_screen(SCREEN_WIDTH, SCREEN_HEIGHT), m_level(1)
{
    quit = false;
    m_score = 0;
    rowsCleared = 0;
    spaceIsHit = false;
}

void Game::play()
{
    m_well.display(m_screen, WELL_X, WELL_Y);
    displayStatus();  //  score, rows left, level
    displayPrompt("Press the Enter key to begin playing Chetyris!");
    waitForEnter();  // [in UserInterface.h]
    
    for(;;)
    {
        if ( ! playOneLevel())
            break;
        displayPrompt("Good job!  Press the Enter key to start next level!");
        waitForEnter();
        m_level++;
        newLevel();
    }
    displayPrompt("Game Over!  Press the Enter key to exit!");
    waitForEnter();
}

void Game::displayPrompt(std::string s)
{
    m_screen.gotoXY(PROMPT_X, PROMPT_Y);
    m_screen.printStringClearLine(s);   // overwrites previous text
    m_screen.refresh();
}

void Game::displayStatus()
{
    //I created a const int RIGHTJUSTIFIED so that the numbers can be printed in a right-justified manner.
    
    //display score
    m_screen.gotoXY(SCORE_X, SCORE_Y);
    m_screen.printStringClearLine("Score:     ");
    std::string score = std::to_string(m_score);
    //print from right to left to ensure right justified
    for (int i = 0; i < score.size() ; i++) {
        m_screen.gotoXY(RIGHTJUSTIFIED-i, SCORE_Y);
        m_screen.printChar(score[score.size()-1 -i]);
    }
    
    //display number of rows left
    m_screen.gotoXY(ROWS_LEFT_X, ROWS_LEFT_Y);
    m_screen.printStringClearLine("Rows left: ");
    std::string rowsLeft = std::to_string(5*m_level - rowsCleared);
    //print from right to left to ensure right justified
    for (int i = 0; i < rowsLeft.size() ; i++) {
        m_screen.gotoXY(RIGHTJUSTIFIED-i, ROWS_LEFT_Y);
        m_screen.printChar(rowsLeft[rowsLeft.size()-1 -i]);
    }
    
    //display level
    m_screen.gotoXY(LEVEL_X, LEVEL_Y);
    m_screen.printString("Level:     ");
    std::string level = std::to_string(m_level);
    //print from right to left to ensure right justified
    for (int i = 0; i < level.size() ; i++) {
        m_screen.gotoXY(RIGHTJUSTIFIED-i, LEVEL_Y);
        m_screen.printChar(level[level.size()-1 -i]);
    }
    
    m_screen.refresh();
}

bool Game::playOneLevel()
{
    queuePieces(); //load first piece
    for (;;) {
        
        queuePieces();
        spawn(); //newPiece is allocated to a new Piece
        displayNextPiece();
        
        if (!newPiece->canMove(0, 0)) {
            delete newPiece;
            return false; //game over if there is overlap right off the bat
        }
        
        for (;;) { //movement of one piece
            discardPendingKeys();
            int a = 1000-(100*(m_level-1)); //time limit that varies by level
            int time = std::max(a,100); //whichever is larger, a or 100, is the time limit per move
            
            if (newPiece->canMoveDown()) {
                Timer timer;
                while (timer.elapsed() < time) {
                    performMoves(time);
                    if (quit) {
                        delete newPiece;
                        return false;
                    }
                    if (spaceIsHit) {
                        break;
                    }
                }
                moveDown();
            }
            else { //if there's something beneath the piece
                if (spaceIsHit) {
                    spaceIsHit = false;
                    break; //after hitting space, else loop is invoked; break again to immortalize piece immediately
                }
                Timer timer2;
                while (timer2.elapsed() < time) {
                    performMoves(time); //one more action loop after it hits bottom
                    if (quit) {
                        delete newPiece;
                        return false;
                    }
                    if (spaceIsHit) {
                        break;
                    }
                }
                if (!newPiece->canMoveDown()) { //check to see if it can move down again; if it can't break; if it can, continue loop
                    break;
                }
                else {
                    moveDown();
                }
            }
        }
        
        immortalize(); //newPiece is deleted in the function if game is not quit
        countClearedRows();
        displayStatus();
        
        if (rowsCleared >= m_level*5) {
            for (int i = 0; i < pieceQueue.size(); i++) {
                pieceQueue.pop();
            } //pop off all queued pieces
            return true;
        }
        
        clearRows();
        m_screen.refresh();
    }
}

void Game::newLevel() {
    //erase things in the well
    for (int column = 1; column <= m_well.getWidth(); column++) {
        for (int row = 0; row < m_well.getHeight(); row++) {
            erase(column, row);
        }
    }
    m_well.clearVector(); //clear all stored coordinates
    rowsCleared = 0; //reset rows cleared timer
    displayStatus(); //display updated status
    m_screen.refresh();
}


void Game::draw(int x, int y, char c) {
    m_screen.gotoXY(x+WELL_X, y+WELL_Y);
    m_screen.printChar(c);
}

void Game::erase(int x, int y) {
    m_screen.gotoXY(x+WELL_X, y+WELL_Y);
    m_screen.printChar(' '); //essentially delete the character by making it a space/invisible
}



void Game::immortalize() {
    //make blocks that are dropped stay forever
    if (currentPieceType == PIECE_VAPOR || currentPieceType == PIECE_FOAM) {
        performSpecialAction(); //blocks are immortalized here for special pieces
    }
    else {
        int runs = newPiece->getMemberNumber();
        for (int i = 0; i < runs; i++) {
            int x = newPiece->getX(i) + curr_x;
            int y = newPiece->getY(i) + curr_y;
            draw(x, y, '$'); //draw over # symbols
            m_well.addCoord(x, y, '$'); //add coord to Well's register
        }
    }
    m_screen.refresh();
    delete newPiece; //delete current Piece to get ready for next spawn
}


void Game::performSpecialAction() {
    if (currentPieceType == PIECE_VAPOR) {
        VaporBombAction();
        erasePiece();
    }
    else if (currentPieceType == PIECE_FOAM) {
        int x = curr_x + 1; //foam piece is always at (1,1) in the bounding box
        int y = curr_y + 1;
        FoamBombAction(x, y, x+2, x-2, y+2, y-2); //x+2, x-2, y+2, y-2 are boundaries that the FoamBomb can reach
    }
}

void Game::VaporBombAction() {
    int X = curr_x + 1; //we know that the piece is always at (1,0) and (2,0) of bounding box
    int Y = curr_y + 0;
    for (int column = 0; column < 2; column++) { //do this for each of 2 columns the block is in
        for (int row = 1; row <= 2; row ++) {
            if (Y-row >= 0) { //for rows above
                erase(X+column, Y-row);
                //delete corresponding coordinates
                m_well.eraseCoord(X+column, Y-row);
            }
            
            if (Y+row < m_well.getHeight()) { //for rows below
                erase(X+column, Y+row); //for rows beneath
                //loop through well and delete corresponding coordinates
                m_well.eraseCoord(X+column, Y+row);
            }
        }
    }
}

void Game::FoamBombAction(int x, int y, int xUpperBoundary, int xLowerBoundary, int yUpperBoundary, int yLowerBoundary) {
    
    if (x <= 0 || x > m_well.getWidth() || y <0 || y >= m_well.getHeight()) {
        return;
    }
    draw(x, y, '*');
    m_well.addCoord(x, y, '*');
    
    
    if (x-1 >= xLowerBoundary && !overlap(x-1, y)) { //if left spot isn't occupied
        FoamBombAction(x-1, y, xUpperBoundary, xLowerBoundary, yUpperBoundary, yLowerBoundary);
    }
    
    if (y-1 >= yLowerBoundary && !overlap(x, y-1)) { //if above spot isn't occupied
        FoamBombAction(x, y-1, xUpperBoundary, xLowerBoundary, yUpperBoundary, yLowerBoundary);
    }
    
    if (x+1 <= xUpperBoundary && !overlap(x+1, y)) { //if right spot isn't occupied
        FoamBombAction(x+1, y, xUpperBoundary, xLowerBoundary, yUpperBoundary, yLowerBoundary);
    }
    
    if (y+1 <= yUpperBoundary && !overlap(x, y+1)) { //if left below isn't occupied
        FoamBombAction(x, y+1, xUpperBoundary, xLowerBoundary, yUpperBoundary, yLowerBoundary);
    }
    
    return;
    
}

void Game::countClearedRows() {
    int cleared = 0;
    for (int row = 0; row < m_well.getHeight(); row++) { //loop through each row, trying to find full rows
        bool rowFound = true;
        for (int column = 1; column <= m_well.getWidth(); column++) {
            if (!m_well.occupied(column, row)) {
                rowFound = false;
                break;
            }
        }
        if (rowFound) {
            rowsCleared++;
            cleared++;
        }
    }
    //add points accordingly
    if (cleared == 1 || cleared == 2) {
        m_score+= (cleared * 100);
    }
    if (cleared == 3) {
        m_score += 400;
    }
    if (cleared == 4) {
        m_score +=800;
    }
    if (cleared == 5) {
        m_score +=1600;
    }
}


void Game::clearRows() {
    for (int row = 0; row < m_well.getHeight(); row++) {
        bool rowFound = true;
        for (int column = 1; column <= m_well.getWidth(); column++) { //find full rows
            if (!m_well.occupied(column, row)) {
                rowFound = false;
                break;
            }
        }
        if (rowFound) { //if a full row is found
            for (int column = 1; column <= m_well.getWidth(); column++) {
                erase(column, row); //erase the row on screen
                
                m_well.eraseCoord(column, row); //erase coordinates of the row from Well's register
            }
            
            //shift everything above one row down
            for (int a = row-1; a>=0; a--) {
                for (int i = 0; i<m_well.numberOfCoords(); i++) {
                    int Y = m_well.getYCoord(i);
                    int X = m_well.getXCoord(i);
                    if (Y==a && X > 0 && X <= m_well.getWidth()) {
                        //shift coordinates one row down
                        m_well.setYcoord(i, Y+1);
                        
                        //shift display one row down
                        //erase old coordinates
                        erase(X, Y);
                        //draw new coordinates
                        draw(X, m_well.getYCoord(i), m_well.getChar(i));
                    }
                }
            }
            m_screen.refresh();
        }
    }
}


void Game::drawPiece() {
    int runs = newPiece->getMemberNumber(); //once for each box of the piece
    for (int i = 0; i < runs; i++) {
        int x = newPiece->getX(i) + curr_x; //draw according to position of bounding box
        int y = newPiece->getY(i) + curr_y;
        draw(x, y, '#');
    }
}

void Game::erasePiece() {
    int runs = newPiece->getMemberNumber(); //once for each box of the piece
    for (int i = 0; i < runs; i++) {
        int x = newPiece->getX(i) + curr_x;
        int y = newPiece->getY(i) + curr_y;
        erase(x, y);
    }
}


int Game::getCurrX() const{
    return curr_x;
}

int Game::getCurrY() const {
    return curr_y;
}


void Game::queuePieces() {
    pieceQueue.push(chooseRandomPieceType());
}



void Game::spawn() {
    currentPieceType = pieceQueue.front();
    pieceQueue.pop();
    switch (currentPieceType)
    {
        case PIECE_I:
            // make the next piece an I piece
            newPiece = new PieceI(this);
            break;
        case PIECE_L:
            // make the next piece an L piece
            newPiece = new PieceL(this);
            break;
        case PIECE_J:
            newPiece = new PieceJ(this);
            break;
        case PIECE_T:
            newPiece = new PieceT(this);
            break;
        case PIECE_O:
            newPiece = new PieceO(this);
            break;
        case PIECE_S:
            newPiece = new PieceS(this);
            break;
        case PIECE_Z:
            newPiece = new PieceZ(this);
            break;
        case PIECE_CRAZY:
            newPiece = new PieceCrazy(this);
            break;
        case PIECE_VAPOR:
            newPiece = new VaporBomb(this);
            break;
        case PIECE_FOAM:
            newPiece = new FoamBomb(this);
            break;
        default:
            break;
    }
    
    //introduce to starting position
    curr_x = 4;
    curr_y = 0; //position of current piece's bounding box
    drawPiece();
}

void Game::displayNextPiece() {
    m_screen.gotoXY(NEXT_PIECE_TITLE_X, NEXT_PIECE_TITLE_Y);
    m_screen.printString("Next piece:");
    //The next piece should be displayed with the upper-left corner of its bounding box at (16,4).
    
    //first, clear old piece
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            erase(x+NEXT_PIECE_X-WELL_X, y+NEXT_PIECE_Y-WELL_Y);
        }
    }
    m_screen.refresh();
    
    Piece* nextPiece;
    PieceType nPiece = pieceQueue.back();
    switch (nPiece) {
        case PIECE_I:
            nextPiece = new PieceI(this);
            break;
        case PIECE_L:
            nextPiece = new PieceL(this);
            break;
        case PIECE_J:
            nextPiece = new PieceJ(this);
            break;
        case PIECE_T:
            nextPiece = new PieceT(this);
            break;
        case PIECE_O:
            nextPiece = new PieceO(this);
            break;
        case PIECE_S:
            nextPiece = new PieceS(this);
            break;
        case PIECE_Z:
            nextPiece = new PieceZ(this);
            break;
        case PIECE_CRAZY:
            nextPiece = new PieceCrazy(this);
            break;
        case PIECE_VAPOR:
            nextPiece = new VaporBomb(this);
            break;
        case PIECE_FOAM:
            nextPiece = new FoamBomb(this);
            break;
        default:
            break;
    }
    
    int runs = nextPiece->getMemberNumber(); //update runs. once for each box of the piece
    for (int i = 0; i < runs; i++) {
        int x = nextPiece->getX(i) + NEXT_PIECE_X;
        int y = nextPiece->getY(i) + NEXT_PIECE_Y;
        draw(x - WELL_X, y-WELL_Y, '#');
    }
    delete nextPiece;
    
    m_screen.refresh();
}


void Game::performMoves(int& time) {
    char ch;
    if (getCharIfAny(ch)) {
        switch (ch) {
            case ARROW_UP:
                rotate();
                break;
                
            case ARROW_LEFT:
                if (currentPieceType == PIECE_CRAZY) {
                    moveRight();
                }
                else {
                    moveLeft();
                }
                break;
                
            case ARROW_RIGHT:
                if (currentPieceType == PIECE_CRAZY) {
                    moveLeft();
                }
                else {
                    moveRight();
                }
                break;
                
            case ARROW_DOWN:
                time=0;
                break; //break out of loop, restart timer
            case ' ':
                while (newPiece->canMoveDown()) {
                    moveDown();
                }
                spaceIsHit = true;
                break;
            case 'q':
            case 'Q':
                quit = true;
                break;
        }
    }
}

void Game::move(int x, int y) {
    erasePiece();
    curr_x+=x;
    curr_y+=y;
    drawPiece();
    m_screen.refresh();
}

void Game::moveDown() {
    if (newPiece->canMoveDown()) {
        move(0,1);
    }
}

void Game::moveLeft() {
    if (newPiece->canMoveLeft()) {
        move(-1,0);
    }
}

void Game::moveRight() {
    if (newPiece->canMoveRight()) {
        move(1,0);
    }
}


void Game::rotate() {
    if (newPiece->canRotate()) {
        erasePiece();
        newPiece->updateOrientation();
        drawPiece();
        m_screen.refresh();
    }
}


bool Game::overlap(int x, int y) {
    //takes in coordinates of one spot on the grid, checks to see if it's occupied/overlaps with walls
    if (x <=0 || x >m_well.getWidth() || y <0 || y>= m_well.getHeight()) {
        return true;
    }
    
    return m_well.occupied(x, y);
    
}


PieceType Game::getCurrentPieceType() const{
    return currentPieceType;
}


