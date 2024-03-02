#include "ECS_Centralized.h"

using namespace std;

ECS::ECS(){
    for (int i=0;i<max_elevators;i++){
        elevatorsAvailable[i] = Elevator(++id);
    }
}

// Destructor
ECS::~ECS(){}

// Member function declarations
//general receivement used for any case, when you don't care about the elevator
void ECS::receiveRequest(int floorNumber, bool goingUp){
    if (goingUp){
        floorsToDelegateGoingUp[getToDelegateUp()]= Floor(floorNumber, goingUp);
        setToDelegateUp(getToDelegateUp()+1);
    } else{
        floorsToDelegateGoingDown[getToDelegateDown()]= Floor(floorNumber, goingUp);
        setToDelegateDown(getToDelegateDown()+1);
    }
    return;
}

//specific receivement used to directly send a request to an elevator, i.e after the optimal elevator was calculated
void ECS::receiveRequestToElevator(int e, int floorNumber, bool goingUp){
    if (e < 0 || e > 3) return;
    elevatorsAvailable[e].addToArrayInOrder(Floor(floorNumber, goingUp), goingUp);
    return;
}

//returns elevator assigned to
Elevator ECS::assignPickupRequest(Floor f, bool goingUp){
    Elevator e = calculateOptimalElevator(f, goingUp);
    f.setStatus(true); //in our eyes once we assign the request its service, since its up to an elevator to reach the floor
    e.receivePickupRequest(f, goingUp);
    return (e);
}

//get next request from queue of floors to be serviced returns -1 if no more requests
Elevator ECS::serviceNextRequest(){
    if (getToDelegateUp() != 0){
        Floor temp = floorsToDelegateGoingUp[0];
        for (int x=0; x<getToDelegateUp();x++){
            floorsToDelegateGoingUp[x]=floorsToDelegateGoingUp[x+1];
        }
        setToDelegateUp(getToDelegateUp()-1);
        return assignPickupRequest(temp, true);
    }

    if (getToDelegateDown() != 0){
        Floor temp = floorsToDelegateGoingDown[0];
        for (int x=0; x<getToDelegateDown();x++){
            floorsToDelegateGoingDown[x]=floorsToDelegateGoingDown[x+1];
        }
        setToDelegateDown(getToDelegateDown()-1);
        return assignPickupRequest(temp, false);
    }
    return Elevator(-1);
}

//incase of i.e fire alarm
//returns message to be printed to GUI
string ECS::sendSignal(int elevatorNumber, int event){
    if (elevatorNumber <0 || elevatorNumber > 2) return "Invalid Elevator";
    switch (event){
        case(HELP):
            return elevatorsAvailable[elevatorNumber].presentMessage("Elevator " + to_string(elevatorNumber) + " Connecting to Safety services");

        case(DOOROBSTACLES):
            elevatorsAvailable[elevatorNumber].openDoors();
            return elevatorsAvailable[elevatorNumber].presentMessage("ERROR: Doorway is blocked in elevator " + to_string(elevatorNumber) +  " , can't close doors safely");

        case(FIRE):
            for(int x=0;x<max_elevators;x++){
                elevatorsAvailable[x].presentMessage("FIRE FIRE FIRE, Elevator " + to_string(x) + " proceeding to a safe floor");
                if(elevatorsAvailable[x].getDoorsClosed() == false) elevatorsAvailable[x].closeDoors();
                elevatorsAvailable[x].clearAllRequests();
                if(elevatorsAvailable[x].getCurrentFloor()!=0) elevatorsAvailable[x].setCurrentDirection(false);
            }
            return("FIRE FIRE FIRE, Elevators proceeding to a safe floor");

        case(OVERLOAD):
            return elevatorsAvailable[elevatorNumber].presentMessage("CAUTION MAX WEIGHT EXCEEDED, Elevator " + to_string(elevatorNumber) + " can't move safely");

        case(POWEROUT):
            if(elevatorsAvailable[elevatorNumber].getDoorsClosed() == false) elevatorsAvailable[elevatorNumber].closeDoors();
            elevatorsAvailable[elevatorNumber].clearAllRequests();
             if(elevatorsAvailable[elevatorNumber].getCurrentFloor()!=0) elevatorsAvailable[elevatorNumber].setCurrentDirection(false);
            return elevatorsAvailable[elevatorNumber].presentMessage("LOSS OF POWER, Elevators proceeding to a safe floor");
    }
    return "Invalid Signal";
}

//sends any destination requests to its corresponding elevator
void ECS::receiveDestButtonRequest(int elevatorNumber, int floorNumber){
    if (elevatorNumber < 0 || elevatorNumber > 2) return;
    elevatorsAvailable[elevatorNumber].receiveDestButtonRequest(floorNumber);
}

string ECS::openDoors(int elevatorNumber){
    if (elevatorNumber < 0 || elevatorNumber > 2) return "Invalid Elevator";
    return elevatorsAvailable[elevatorNumber].openDoors();
}

string ECS::closeDoors(int elevatorNumber){
    if (elevatorNumber < 0 || elevatorNumber > 2) return "Invalid Elevator";
    return elevatorsAvailable[elevatorNumber].closeDoors();
}

//Method to figure out the closest elevator going in the direction of the floor is.
Elevator ECS::calculateOptimalElevator(Floor f, bool goingUp){
    Elevator minElevator = elevatorsAvailable[0];
    int distances [max_elevators] = {0, 0, 0};
    int minIndex = 0;
    for (int x = 0; x < max_elevators; x++) {
        int wrapAroundDistance = (6 - abs(f.getFloorNumber() - elevatorsAvailable[x].getCurrentFloor())) * 2;

        // If the elevator is not going in the desired direction, adding the distance it takes to go to ground floor ands back
        if (elevatorsAvailable[x].getCurrentDirection() != goingUp) {
            distances[x] += wrapAroundDistance;
        }

        distances[x] += abs(f.getFloorNumber() - elevatorsAvailable[x].getCurrentFloor());

        //since servicing a floor requires to wait x time
        distances[x]+= (elevatorsAvailable[x].getFloorsWaitingGoingDown() + elevatorsAvailable[x].getFloorsWaitingGoingUp());

        if (distances[x] < distances[minIndex]) minIndex = x;
    }
    return elevatorsAvailable[minIndex];
}

//moves the elevator to a given floor in a given direction
string ECS::moveElevatorToFloor(int eleNum, int floor, bool goingUp){
    return elevatorsAvailable[eleNum].moveToFloor(floor, goingUp);
}

void ECS::setElevatorWeight(int eleNum, int weight){
    if (eleNum < 0 || eleNum > 2) {
        cout<<"ERROR in setting elevator weight"<<endl;
        return;
    }
    cout<<"Setting elevaor number " + to_string(eleNum) + " to weight " + to_string(weight)<<endl;
    elevatorsAvailable[eleNum].setCurrentCapacity(weight);
    return;
}

//checks if the elevator has any more floors to service (i.e destination floors)
bool ECS::hasFloorsToService(int num){
    if (elevatorsAvailable[num].getFloorsWaitingGoingDown() == 0 && elevatorsAvailable[num].getFloorsWaitingGoingUp() == 0) return false;
    return true;
}

void ECS::setElevatorTransit(int eleNum, int inTransit){
    if (eleNum < 0 || eleNum > 2) return;
    elevatorsAvailable[eleNum].setInTransit(inTransit);
    return;
}
