#ifndef PIECE_INCLUDED
#define PIECE_INCLUDED


enum PieceType {
    PIECE_I, PIECE_L, PIECE_J, PIECE_T, PIECE_O, PIECE_S, PIECE_Z,
    PIECE_VAPOR, PIECE_FOAM, PIECE_CRAZY, NUM_PIECE_TYPES
};

PieceType chooseRandomPieceType();
class Game;


class Piece {
public:
    /* things all pieces have in common:
     rotate --> declare pure virtual as they have different implementations
     virtual destructor
     cannot have virtual constructor!!
     */
    Piece(Game* game);
    virtual ~Piece();
    
    virtual int getMemberNumber() const =0;
    virtual void updateOrientation()=0; //do nothing for special blocks

    bool canMove(int offsetX, int offsetY) const;
    bool canMoveLeft() const;
    bool canMoveRight() const;
    bool canMoveDown() const;
    virtual bool canRotate() const =0 ; //always false for special pieces
    
    void setCoord(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
    int getX(int x) const;
    int getY(int y) const;
    
    Game* getGameptr() const;
    
private:
    Game* gameptr; //point to game to check to see if neighboring spots are ocupied
    int xCoord [4] ; //special pieces don't change coords
    int yCoord[4];
};

class SpecialPiece: public Piece {
public:
    SpecialPiece(Game*game);
    virtual int getMemberNumber() const=0;
    virtual void updateOrientation();
    virtual bool canRotate() const;
};


class VaporBomb: public SpecialPiece {
public:
    VaporBomb(Game* game);
    virtual int getMemberNumber() const;
};

class FoamBomb: public SpecialPiece {
public:
    FoamBomb(Game* game);
    virtual int getMemberNumber() const;
};



//for all pieces with 4 blocks
class NormalPiece: public Piece {
public:
    NormalPiece(Game* game);
    virtual ~NormalPiece();
    virtual void updateOrientation()=0;
    virtual int getMemberNumber() const;
    virtual int getState() const;
    void setState(int newState);
    
    virtual bool canRotate() const;
    
private:
    int state; //0, 1, 2, or 3; special pieces don't have this
};



class PieceI: public NormalPiece {
public:
    PieceI(Game*game);
    void updateOrientation();
};

class PieceL: public NormalPiece {
public:
    PieceL(Game*game);
    void updateOrientation();
};

class PieceJ: public NormalPiece {
public:
    PieceJ(Game*game);
    void updateOrientation();
};

class PieceT: public NormalPiece {
public:
    PieceT(Game*game);
    void updateOrientation();
};

class PieceO: public NormalPiece {
public:
    PieceO(Game*game);
    void updateOrientation();
    virtual bool canRotate();
};

class PieceS: public NormalPiece {
public:
    PieceS(Game*game);
    void updateOrientation();
};

class PieceZ: public NormalPiece {
public:
    PieceZ(Game*game);
    void updateOrientation();
};

class PieceCrazy: public NormalPiece {
public:
    PieceCrazy(Game*game);
    void updateOrientation();
};

#endif // PIECE_INCLUDED
