#include "Piece.h"
#include "Game.h"

Piece::Piece(Game* game) {
    gameptr = game;
}

Piece::~Piece() {
    
}

int Piece::getX(int x) const {
    return xCoord[x];
}

int Piece::getY(int y) const {
    return yCoord[y];
}

void Piece::setCoord(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    xCoord[0] = x1;
    yCoord[0] = y1;
    xCoord[1] = x2;
    yCoord[1] = y2;
    xCoord[2] = x3;
    yCoord[2] = y3;
    xCoord[3] = x4;
    yCoord[3] = y4;
}

bool Piece::canMove(int offsetX, int offsetY) const {
    for (int i = 0; i < getMemberNumber(); i++) {
        //get coordinates relative to the screen
        int x = getX(i) + gameptr->getCurrX() + offsetX;
        int y = getY(i) + gameptr->getCurrY() + offsetY;
        if (gameptr->overlap(x, y)) { //see if it overlaps with anything in the Well's register
            return false;
        }
    }
    return true;
}

bool Piece::canMoveDown() const {
    return canMove(0, 1);
}

bool Piece::canMoveLeft() const {
    return canMove(-1,0);
}

bool Piece::canMoveRight() const {
    return canMove(1, 0);
}


Game* Piece::getGameptr() const {
    return gameptr;
}




//SPECIAL PIECES
SpecialPiece::SpecialPiece(Game*game): Piece(game) {}


void SpecialPiece::updateOrientation() {
    return; //do nothing
}

bool SpecialPiece::canRotate() const {
    return false;
}



//VAPORBOMB
VaporBomb::VaporBomb(Game*game): SpecialPiece(game) {
    setCoord(1,0, 2,0, 1000,1000, 1000,1000);
    //only first 4 (2 coordinates) are needed; set unlikely values for the rest for testing
}

int VaporBomb::getMemberNumber() const {
    return 2;
}





//FOAMBOMB
FoamBomb::FoamBomb(Game*game): SpecialPiece(game) {
    setCoord(1,1, 1000,1000, 1000,1000, 1000,1000);
    //only first 2 (1 coordinate) is needed; set unlikely values for the rest for testing
}

int FoamBomb::getMemberNumber() const {
    return 1;
}





//////NORMAL PIECES



NormalPiece::NormalPiece(Game* game): Piece(game) {
    state = 0;
}




NormalPiece::~NormalPiece(){}


int NormalPiece::getMemberNumber() const {
    return 4;
}

int NormalPiece::getState() const {
    return state;
}

void NormalPiece::setState(int newState) {
    state = newState;
}


bool NormalPiece::canRotate() const {
    //make a new piece identical to the newPiece, reorient it without printing drawing it, and see if there are any overlaps
    NormalPiece* tempPiece;
    Game* game = getGameptr();
    PieceType p = game->getCurrentPieceType();
    switch (p) {
        case PIECE_I:
            tempPiece = new PieceI(game);
            break;
        case PIECE_L:
            tempPiece = new PieceL(game);
            break;
        case PIECE_J:
            tempPiece = new PieceJ(game);
            break;
        case PIECE_T:
            tempPiece = new PieceT(game);
            break;
        case PIECE_O:
            tempPiece = new PieceO(game);
            break;
        case PIECE_S:
            tempPiece = new PieceS(game);
            break;
        case PIECE_Z:
            tempPiece = new PieceZ(game);
            break;
        case PIECE_CRAZY:
            tempPiece = new PieceCrazy(game);
            break;
        default:
            break;
    }
    
    for (int i = 0; i< getState()+1; i++) { //run one more time to update orientation
        tempPiece->updateOrientation();
    }
    
    if (tempPiece->canMove(0, 0) ) {
        delete tempPiece;
        return true;
    }
    else {
        delete tempPiece;
        return false;
    }
    
}




///////PIECE I


PieceI::PieceI(Game* game): NormalPiece(game) {
    setCoord(0,1, 1,1, 2,1, 3,1); //essentially the bounding box is a construct
}


void PieceI:: updateOrientation() {
    switch(getState()) {
        case 0:
            setCoord(1,0, 1,1, 1,2, 1,3);
            setState(1);
            break;
        case 1:
            setCoord(0, 1, 1, 1, 2, 1, 3, 1);
            setState(0);
            break;
    }
}




///////PIECE L

PieceL::PieceL(Game* game): NormalPiece(game) {
    setCoord(0,1, 0,2, 1,1, 2,1);
}

void PieceL:: updateOrientation() {
    switch(getState()) {
        case 0:
            setCoord(1,0, 2,0, 2,1, 2,2);
            setState(1);
            break;
        case 1:
            setCoord(0,2, 1,2, 2,2, 2,1);
            setState(2);
            break;
        case 2:
            setCoord(1,1, 1,2, 1,3, 2,3);
            setState(3);
            break;
        case 3:
            setCoord(0,1, 0,2, 1,1, 2,1);
            setState(0);
            break;
    }
}



///////PIECE J

PieceJ::PieceJ(Game* game): NormalPiece(game) {
    setCoord(1,1, 2,1, 3,1, 3,2);
}

void PieceJ::updateOrientation() {
    switch(getState()) {
        case 0:
            setCoord(2,1, 2,2, 2,3, 1,3);
            setState(1);
            break;
        case 1:
            setCoord(1,1, 1,2, 2,2, 3,2);
            setState(2);
            break;
        case 2:
            setCoord(1,0, 1,1, 1,2, 2,0);
            setState(3);
            break;
        case 3:
            setCoord(1,1, 2,1, 3,1, 3,2);
            setState(0);
            break;
    }
}



///////PIECE T

PieceT::PieceT(Game* game): NormalPiece(game) {
    setCoord(1,0, 0,1, 1,1, 2,1);
}

void PieceT::updateOrientation() {
    switch(getState()) {
        case 0:
            setCoord(1,0, 1,1, 1,2, 2,1);
            setState(1);
            break;
        case 1:
            setCoord(0,1, 1,1, 2,1, 1,2);
            setState(2);
            break;
        case 2:
            setCoord(1,0, 1,1, 1,2, 0,1);
            setState(3);
            break;
        case 3:
            setCoord(1,0, 0,1, 1,1, 2,1);
            setState(0);
            break;
    }
}

///////PIECE O

PieceO::PieceO(Game* game): NormalPiece(game) {
    setCoord(0,0, 1,0, 0,1, 1,1);
}

void PieceO::updateOrientation() {
    //do nothing
}

bool PieceO::canRotate() {
    return false;
}



///////PIECE S

PieceS::PieceS(Game* game): NormalPiece(game) {
    setCoord(0,2, 1,2, 1,1, 2,1);
}

void PieceS::updateOrientation() {
    switch(getState()) {
        case 0:
            setCoord(1,0, 1,1, 2,1, 2,2);
            setState(1);
            break;
        case 1:
            setCoord(0,2, 1,2, 1,1, 2,1);
            setState(0);
            break;
    }
}



///////PIECE Z
PieceZ::PieceZ(Game* game): NormalPiece(game) {
    setCoord(0,1, 1,1, 1,2, 2,2);
}

void PieceZ::updateOrientation() {
    switch(getState()) {
        case 0:
            setCoord(2,0, 2,1, 1,1, 1,2);
            setState(1);
            break;
        case 1:
            setCoord(0,1, 1,1, 1,2, 2,2);
            setState(0);
            break;
    }
}



///////CRAZY PIECE
PieceCrazy::PieceCrazy(Game* game): NormalPiece(game) {
    setCoord(0,0, 1,1, 2,1, 3,0);
}

void PieceCrazy::updateOrientation() {
    switch(getState()) {
        case 0:
            setCoord(3,0, 2,1, 2,2, 3,3);
            setState(1);
            break;
        case 1:
            setCoord(0,3, 1,2, 2,2, 3,3);
            setState(2);
            break;
        case 2:
            setCoord(0,0, 1,1, 1,2, 0,3);
            setState(3);
            break;
        case 3:
            setCoord(0,0, 1,1, 2,1, 3,0);
            setState(0);
            break;
    }
}

