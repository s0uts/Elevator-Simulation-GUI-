#ifndef ECS_H
#define ECS_H

#include "Elevator_Centralized.h"
#include "Floor_Centralized.h"
#include <iostream>
#include <QThread>

class ECS {
public:
    // Constructor
    ECS();

    // Destructor
    ~ECS();

    // Member function declarations
    void receiveRequest(int floorNumber, bool goingUp);
    void receiveRequestToElevator(int e, int floorNumber, bool goingUp);
    Elevator assignPickupRequest(Floor f, bool goingUp);
    Elevator serviceNextRequest(); //get next request from queue of floors to be serviced.
    string sendSignal(int elevatorNumber, int event); //incase of i.e fire alarm
    void receiveDestButtonRequest(int elevatorNumber, int floorNumber);
    string openDoors(int elevatorNumber);
    string closeDoors(int elevatorNumber);
    Elevator calculateOptimalElevator(Floor f, bool goingUp); //Method to figure out the closest elevator going in the direction of the floor is.
    string moveElevatorToFloor(int eleNum, int floor, bool goingUp);

    //Getters
    int getToDelegateUp(){return this->toDelegateUp;}
    int getToDelegateDown(){return this->toDelegateDown;}
    int getElevatorFloor(int num){return elevatorsAvailable[num].getCurrentFloor();}
    bool getElevatorDirection(int num){return elevatorsAvailable[num].getCurrentDirection();}
    int getElevatorWeight(int num){return elevatorsAvailable[num].getCurrentCapacity();}
    bool getDoorsClosed(int num){return elevatorsAvailable[num].getDoorsClosed();}
    bool hasFloorsToService(int num);
    
    //Setters
    void setToDelegateUp(int num){this->toDelegateUp=num;}
    void setToDelegateDown(int num){this->toDelegateDown=num;}
    void setElevatorWeight(int eleNum, int weight);
    void setElevatorTransit(int eleNum, int inTransit);

private:
    // Private member variables
    Floor floorsToDelegateGoingUp [max_floors];
    Floor floorsToDelegateGoingDown [max_floors];
    int toDelegateUp=0;
    int toDelegateDown=0;
    Elevator elevatorsAvailable [max_elevators];
    int id=-1;
};

#endif  
