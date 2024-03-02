#include "myThread.h"

myThread::myThread(QObject *parent) : QThread(parent)
{
//for some reason keep this space - otherwise get a weird malloc() error
}

//the thread acts as an elevator, moving up and down a grid, "stopping" at each given floor, and finally remains idle at either the top or bottom position.
void myThread::run(){
    while (this->destFloor!= -1){
        mutex.lock();
        this->inTransit=true;
        mutex.unlock();
        while (this->currentFloor != this->destFloor){
            this->msleep(500);

            if (QThread::currentThread()->isInterruptionRequested()) { //i.e return to ground floor
                mutex.lock();
                this->goingUp=false;
                this->destDirection=false;
                this->destFloor=0;
                mutex.unlock();
            }

            if (goingUp && this->currentFloor == 6){ //if reached top floor, start going down
                mutex.lock();
                this->goingUp=false;
                mutex.unlock();
            } else if (!goingUp && this->currentFloor == 0){ //if reached bottom floor, start going up
                mutex.lock();
                this->goingUp=true;
                mutex.unlock();
            }

            if (goingUp){
                mutex.lock();
                this->currentFloor+=1;
                mutex.unlock();
            } else {
                mutex.lock();
                this->currentFloor-=1;
                mutex.unlock();
            }

            emit moveElevator(this->id, this->currentFloor, this->goingUp);

            this->msleep(1000);
        }

        emit openDoors(this->id);
        this->msleep(1000); //simulates closing doors
        emit closeDoors(this->id);

        mutex.lock();
        this->inTransit=false;
        mutex.unlock();

        getNextFloor();

        if (this->destFloor==-1){ //checks if elevator is not at an appropiate idle floor
            if (this->destDirection && this->currentFloor!=6){
                mutex.lock();
                this->goingUp=true;
                mutex.unlock();
                addFloor(6);
                getNextFloor();

            } else if (!this->destDirection && this->currentFloor!=0){
                mutex.lock();
                this->goingUp=false;
                mutex.unlock();
                addFloor(0);
                getNextFloor();
            }
        }
    }
}

//helper to add a floor to the array
void myThread::addFloor(int f){
    if (this->floors == 7) return;
    mutex.lock();
    this->floorsToService[this->floors] = f;
    this->floors+=1;
    mutex.unlock();
}

//helper to get next floor in the array
void myThread::getNextFloor(){
    if (this->floors == 0) {
        mutex.lock();
        this->destFloor=-1;
        mutex.unlock();
        return;
    }
    mutex.lock();
    this->destFloor = this->floorsToService[this->floors-1];
    this->floors-=1;
    mutex.unlock();
    return;
}

//helper to clear the array - ended up not using, caused some weird issues (maybe due to concurrency scheduling)
void myThread::clearFloors(){
    mutex.lock();
    for (int i=0;i<this->floors;i++){
        this->floorsToService[i]=-1;
    }
    this->floors=0;
    mutex.unlock();
}
