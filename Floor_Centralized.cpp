#include "Floor_Centralized.h"

// Constructor
Floor::Floor(){
    Floor(-1, false);
}

Floor::Floor(int floorNum, bool gU){
    setFloorNumber(floorNum);
    setGoingUp(gU);
}

// Destructor
Floor::~Floor(){}

void Floor::print(){
    std::cout << "Floornumber " << getFloorNumber() << "serviced? " << getStatus()<<std::endl;
    return;
}


