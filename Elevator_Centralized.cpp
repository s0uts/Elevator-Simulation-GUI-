#include "Elevator_Centralized.h"

using namespace std;

// Constructor
Elevator::Elevator(){
    Elevator(-1);
}

Elevator::Elevator(int id, int floor, bool goingUp){
    setElevatorID(id);
    setCurrentFloor(floor);
    setCurrentDirection(goingUp);
}

// Destructor
Elevator::~Elevator(){}

// Member function declarations
//ECS would call this method to 'give an elevator a floor'
//need to add direction
void Elevator::receivePickupRequest(Floor f, bool goingUp){
    addToArrayInOrder(f, goingUp);
    return;
}

//Removes a floor from the array that was deem serviced by the ECS
Floor Elevator::removeServicedRequest(){
    for (int i=0;i<getFloorsWaitingGoingUp();i++){
        if (floorsToServiceGoingUp[i].getStatus()==true){
            for (int z=i;z<getFloorsWaitingGoingUp()-1;z++){
                floorsToServiceGoingUp[z]=floorsToServiceGoingUp[z+1];
            }
            setFloorsWaitingGoingUp(getFloorsWaitingGoingUp()-1);
            return floorsToServiceGoingUp[i];
        }
    }

    for (int j=0;j<getFloorsWaitingGoingDown();j++){
         if (floorsToServiceGoingDown[j].getStatus()==true){
             for (int z=j;z<getFloorsWaitingGoingDown()-1;z++){
                 floorsToServiceGoingDown[z]=floorsToServiceGoingDown[z+1];
             }
             setFloorsWaitingGoingDown(getFloorsWaitingGoingDown()-1);
             return floorsToServiceGoingDown[j];
         }
    }
    return Floor(-1, false);
}

//adds a floor to one of the arrays in order
void Elevator::addToArrayInOrder(Floor f, bool goingUp){
    if (goingUp){
        for (int x=getFloorsWaitingGoingUp()-1;x>0;x--){
            if (isFloorOneCloserThanTwo(floorsToServiceGoingUp[x-1], f)==false){
                floorsToServiceGoingUp[x]=f;
                setFloorsWaitingGoingUp(getFloorsWaitingGoingUp()+1);
                return;
            }
            floorsToServiceGoingUp[x]=floorsToServiceGoingUp[x-1];
        }

        //in this case, the floor should be the first floor
        floorsToServiceGoingUp[0]=f;
        setFloorsWaitingGoingUp(getFloorsWaitingGoingUp()+1);
    } else {
        for (int x=getFloorsWaitingGoingDown()-1;x>0;x--){
            if (isFloorOneCloserThanTwo(floorsToServiceGoingDown[x-1], f)==false){
                floorsToServiceGoingDown[x]=f;
                setFloorsWaitingGoingDown(getFloorsWaitingGoingDown()+1);
                return;
            }
            floorsToServiceGoingDown[x]=floorsToServiceGoingDown[x-1];
        }

        floorsToServiceGoingDown[0]=f;
        setFloorsWaitingGoingDown(getFloorsWaitingGoingDown()+1);
    }
    return;
}


//returns true if the first floor given is current to the elevators current position (compared to the second floor)
bool Elevator::isFloorOneCloserThanTwo(Floor f1, Floor f2){
    int curFloor = getCurrentFloor();
    bool goingUp = getCurrentDirection();
    int floorOne = f1.getFloorNumber();
    int floorTwo = f2.getFloorNumber();

    if (floorOne == curFloor){return true;}
    if (floorOne > curFloor && goingUp){
        if (floorOne < floorTwo){return true;}
    }
    if (floorOne < curFloor && !goingUp){
        if (floorOne > floorTwo){return true;}
    }
    return false;
}

//moves the elevator to a given floor
string Elevator::moveToFloor(int floor, bool goingUp){
    if (floor < 0 || floor > 6) return "Invalid Floor";

    if (getCurrentCapacity() > maxCapacity){
        return "ERROR: Maximum Weight Exceeded";
    }

    if (getDoorsClosed() == false){
        return "ERROR: Doors not Closed";
    }

    setInTransit(true);
    setCurrentDirection(goingUp);
    setCurrentFloor(floor);

    if (getCurrentFloor() == 6 && goingUp){
        setCurrentDirection("down");
    }

    if (getCurrentFloor() == 0 && !goingUp){
        setCurrentDirection("up");
    }
    cout<<"Elevator id " << getElevatorID() << " Moving to floor " << floor << " in direction " << getCurrentDirection()<<" from floor "<<getCurrentFloor()<<endl;
    return "Elevator id " + to_string(getElevatorID()) + " Moving to floor " + to_string(floor);
}

//used to present any error messages
string Elevator::presentMessage(string m){
    cout<<m<<endl;
    return m;
}

//Encapsulates the functionality of serivicing a floor (i.e open doors, ring bells, etc)
string Elevator::serviceCurrentFloor(){
   return openDoors() + ", " + closeDoors();
}

string Elevator::openDoors(){
    std::cout<< "Opening doors for 10 seconds"<<endl;
    setDoorsClosed(false);
    return ringBell() + ", Opening doors for 10 seconds";
}
string Elevator::ringBell(){
    std::cout<< "Ringing bell"<<endl;
    return "Ringing bell";
}

string Elevator::closeDoors(){
    std::cout<< "Closing doors"<<endl;
    setDoorsClosed(true);
    return ringBell() + ", Closing doors";
}

//an elevator receives and deals with its own destination floor requests, to reduce complexity
void Elevator::receiveDestButtonRequest(int floor){
    if (floor == currentFloor) {
        serviceCurrentFloor();
    } else if (floor < currentFloor){
        receivePickupRequest(Floor(floor, false), false);
    } else {
        receivePickupRequest(Floor(floor, true), true);
    }
    return;
}

//Used in signals like fire alarm, where the elevators need to return to ground floor immediatly
void Elevator::clearAllRequests(){
    setFloorsWaitingGoingDown(0);
    setFloorsWaitingGoingUp(0);
    for (int x=0;x<max_floors;x++){
        floorsToServiceGoingDown[x].setFloorNumber(-1);
        floorsToServiceGoingUp[x].setFloorNumber(-1);
    }
}

//Prints elevator info - i.e id, current floor, etc.
void Elevator::print(){
    std::cout<<"Elevator Id = " << getElevatorID() << " current floor is " << getCurrentFloor() << " with direction " << getCurrentDirection() << " and status: " << getInTransit()<<endl;
    for (int x=0; x<getFloorsWaitingGoingUp(); x++){
        floorsToServiceGoingUp[x].print();
    }
    for (int x=0; x<getFloorsWaitingGoingDown(); x++){
        floorsToServiceGoingDown[x].print();
    }
    return;
}

/*
//i.e move to next floor required and stop there
void Elevator::serviceNextRequest(){
    setInTransit(true);
    if (getCurrentDirection() && floorsToServiceGoingUp[0].getFloorNumber()!=-1){
        moveToFloor(floorsToServiceGoingUp[0].getFloorNumber(), true);
        serviceCurrentFloor();
        floorsToServiceGoingUp[0].setStatus(true);

        //getting rid of serviced floor from array, by moving every element up one
        for (int x=0;x<getFloorsWaitingGoingUp();x++){
            floorsToServiceGoingUp[x]=floorsToServiceGoingUp[x+1];
        }
        setFloorsWaitingGoingUp(getFloorsWaitingGoingUp()-1);
        serviceCurrentFloor();
    } else if (!getCurrentDirection() && floorsToServiceGoingDown[0].getFloorNumber()!=-1) {
        moveToFloor(floorsToServiceGoingDown[0].getFloorNumber(), false);
        serviceCurrentFloor();
        floorsToServiceGoingDown[0].setStatus(true);

        for (int x=0;x<getFloorsWaitingGoingDown()-1;x++){
            floorsToServiceGoingDown[x]=floorsToServiceGoingDown[x+1];
        }
        setFloorsWaitingGoingDown(getFloorsWaitingGoingDown()-1);
        serviceCurrentFloor();
    }

    if (getFloorsWaitingGoingDown() == 0 && getFloorsWaitingGoingUp() == 0){
        setInTransit(false);
        cout<<"Elevator " << getElevatorID() << " is idle as floor " << getCurrentFloor()<<endl;
    }

    return;
}
*/
