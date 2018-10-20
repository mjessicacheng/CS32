#include "Well.h"
#include "UserInterface.h"

#include <vector>

Well::Well(int w, int h) {
    width = w;
    height = h;
}

void Well::display(Screen& screen, int x, int y)
{
    for (int i = 0; i<height; i++) {
        screen.gotoXY(x, i+y);
        screen.printChar('@');
        
        screen.gotoXY(x+width+1, i+y);
        screen.printChar('@');
        
    }
    
    for (int i = 0; i<=width+1; i++) {
        screen.gotoXY(i+x, y+height);
        screen.printChar('@');
    }
    
}

int Well::getWidth() const {
    return width;
}

int Well::getHeight() const {
    return height;
}

void Well::clearVector() {
    x_coord.clear();
    y_coord.clear();
    charas.clear();
}

void Well::addCoord(int x, int y, char c) {
    x_coord.push_back(x);
    y_coord.push_back(y);
    charas.push_back(c);
}

void Well::eraseCoord(int xToErase, int yToErase) {
    for (int i = 0; i < x_coord.size(); i++) {
        if (x_coord[i]==xToErase && y_coord[i] == yToErase) { //if exact coordinates are found in registry
            x_coord.erase(x_coord.begin()+i);
            y_coord.erase(y_coord.begin()+i);
            charas.erase(charas.begin()+i);
            break;
        }
    }
}

int Well::getXCoord(int i) const {
    return x_coord[i];
}

int Well::getYCoord(int i) const {
    return y_coord[i];
}

char Well::getChar (char i) const {
    return charas[i];
}

void Well::setYcoord(int i, int newValue) {
    y_coord[i] = newValue;
}

int Well::numberOfCoords() const {
    return x_coord.size(); //x_coord and y_coord should be the same size at all times
}

bool Well::occupied(int x, int y) const {
    for (int i = 0; i < x_coord.size(); i++) {
        if (x_coord[i]==x && y_coord[i]==y) { //if exact coordinates are found in registry
            return true;
        }
    }
    return false;
}
