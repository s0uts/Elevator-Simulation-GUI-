#ifndef FLOOR_H
#define FLOOR_H

#include "defs_Centralized.h"
#include <iostream>

class Floor {
public:
    // Constructor
    Floor();
    Floor(int floornumber, bool gU);

    // Destructor
    ~Floor();

    // Member function declarations
    void print();

    //Getters
    int getFloorNumber(){return this->floorNumber;}
    bool getStatus(){return this->serviced;}
    bool getGoingUp(){return this->goingUp;}

    //Setters
    void setFloorNumber(int num){this->floorNumber = num;}
    void setStatus(bool s){this->serviced = s;}
    void setGoingUp(bool gU){this->goingUp=gU;}

private:
    // Private member variables
    int floorNumber=-1;
    bool serviced=true; //By default a floor is serviced
    bool goingUp=false;
};

#endif  
