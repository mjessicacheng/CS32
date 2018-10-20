#ifndef WELL_INCLUDED
#define WELL_INCLUDED

#include <vector>

class Screen;

class Well
{
public:
    Well(int w, int h);
    
    void display(Screen& screen, int x, int y);
    
    int getWidth() const;
    int getHeight() const;
    
    void clearVector();
    void addCoord (int x, int y, char c);
    void eraseCoord(int xToErase, int yToErase);
    int getXCoord(int i) const;
    int getYCoord(int i) const;
    char getChar (char i) const;
    void setYcoord(int i, int newValue);
    int numberOfCoords() const;
    bool occupied(int x, int y) const; //tells you if a certain point is occupied
    
    
private:
    std::vector<int> x_coord; //vector class to hold all occupied spots on screen (x-coordinate)
    std::vector<int> y_coord; //vector class to hold all occupied spots on screen (y-coordinate)
    std::vector<char> charas; //vector class to hold chars on screen
    
    int width, height;
};

#endif // WELL_INCLUDED
