#ifndef ELEVATOR_H  
#define ELEVATOR_H

#include "Floor_Centralized.h"
#include "defs_Centralized.h"
#include <iostream>
#include <unistd.h>

using namespace std;

class Elevator {
public:
    // Constructor
    Elevator();
    Elevator(int id, int floor = 0, bool goingUp = true);

    // Destructor
    ~Elevator();

    // Member function declarations
    void receivePickupRequest(Floor f, bool goingUp); //ECS would call this method to 'give an elevator a floor'
    void handleSignal(int event);
    string moveToFloor(int floor, bool goingUp);
    string serviceCurrentFloor(); //Encapsulates the functionality of serivicing a flor (i.e open doors, ring bells, etc)
    string openDoors();
    string ringBell();
    string closeDoors();
    void receiveDestButtonRequest(int floor);
    void print(); //Prints elevator info - i.e id, current floor, etc.
    void addToArrayInOrder(Floor f, bool goingUp);
    bool isFloorOneCloserThanTwo(Floor f1, Floor f2);
    string presentMessage(string m);
    void clearAllRequests();
    Floor removeServicedRequest();
    //void serviceNextRequest(); //i.e move to next floor required and stop there
    
    //Getters
    int getElevatorID(){return this->elevatorID;}
    int getCurrentFloor(){return this->currentFloor;}
    bool getCurrentDirection(){return this->goingUp;}
    int getFloorsWaitingGoingUp(){return this->floorsWaitingGoingUp;}
    int getFloorsWaitingGoingDown(){return this->floorsWaitingGoingDown;}
    bool getInTransit(){return this->inTransit;}
    bool getDoorsClosed(){return this->doorsClosed;}
    int getCurrentCapacity(){return this->currentCapacity;}
    
    //Setters
    void setElevatorID(int num){this->elevatorID=num;}
    void setCurrentFloor(int num){this->currentFloor=num;}
    void setCurrentDirection(bool goingUp){this->goingUp=goingUp;}
    void setInTransit(bool s){this->inTransit=s;}
    void setFloorsWaitingGoingUp(int floors){this->floorsWaitingGoingUp=floors;}
    void setFloorsWaitingGoingDown(int floors){this->floorsWaitingGoingDown=floors;}
    void setDoorsClosed(bool b){this->doorsClosed=b;}
    void setCurrentCapacity(int i){this->currentCapacity=i;}


private:
    // Private member variables
    int elevatorID=-1;
    int currentFloor=0; //Ground floor
    bool goingUp = true;
    Floor floorsToServiceGoingUp [max_floors];
    Floor floorsToServiceGoingDown [max_floors];
    int floorsWaitingGoingUp=0;
    int floorsWaitingGoingDown=0;
    bool inTransit=false;
    bool doorsClosed=true;
    int currentCapacity=0;
};

#endif  
