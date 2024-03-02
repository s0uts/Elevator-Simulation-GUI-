#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connect(Oject1, signal1, Object2, slotx)
    threadZero = new myThread(this);
    threadZero->id=0;
    threadOne = new myThread(this);
    threadOne->id=1;
    threadTwo = new myThread(this);
    threadTwo->id=2;
    connect(threadZero, SIGNAL(moveElevator(int, int, bool)), this, SLOT(onElevatorMoved(int, int, bool)));
    connect(threadOne, SIGNAL(moveElevator(int, int, bool)), this, SLOT(onElevatorMoved(int, int, bool)));
    connect(threadTwo, SIGNAL(moveElevator(int, int, bool)), this, SLOT(onElevatorMoved(int, int, bool)));

    connect(threadZero, SIGNAL(openDoors(int)), this, SLOT(onOpenDoors(int)));
    connect(threadOne, SIGNAL(openDoors(int)), this, SLOT(onOpenDoors(int)));
    connect(threadTwo, SIGNAL(openDoors(int)), this, SLOT(onOpenDoors(int)));

    connect(threadZero, SIGNAL(closeDoors(int)), this, SLOT(onCloseDoors(int)));
    connect(threadOne, SIGNAL(closeDoors(int)), this, SLOT(onCloseDoors(int)));
    connect(threadTwo, SIGNAL(closeDoors(int)), this, SLOT(onCloseDoors(int)));

    connect(ui->upButton, SIGNAL(released()), this, SLOT(userRequestGoingUp()));
    connect(ui->downButton, SIGNAL(released()), this, SLOT(userRequestGoingDown()));
    connect(ui->openDoorsButton, SIGNAL(released()), this, SLOT(openDoors()));
    connect(ui->closeDoorsButton, SIGNAL(released()), this, SLOT(closeDoors()));

    connect(ui->destButton_0, SIGNAL(released()), this, SLOT(destButtonClicked_0()));
    connect(ui->destButton_1, SIGNAL(released()), this, SLOT(destButtonClicked_1()));
    connect(ui->destButton_2, SIGNAL(released()), this, SLOT(destButtonClicked_2()));
    connect(ui->destButton_3, SIGNAL(released()), this, SLOT(destButtonClicked_3()));
    connect(ui->destButton_4, SIGNAL(released()), this, SLOT(destButtonClicked_4()));
    connect(ui->destButton_5, SIGNAL(released()), this, SLOT(destButtonClicked_5()));
    connect(ui->destButton_6, SIGNAL(released()), this, SLOT(destButtonClicked_6()));

    connect(ui->helpButton, SIGNAL(released()), this, SLOT(sendSignalHelp()));
    connect(ui->doorObstaclesButton, SIGNAL(released()), this, SLOT(sendSignalDoorObstacles()));
    connect(ui->fireAlarmButton, SIGNAL(released()), this, SLOT(sendSignalFire()));
    connect(ui->overloadAlarmButton, SIGNAL(released()), this, SLOT(sendSignalOverLoad()));
    connect(ui->poweroutAlarmButton, SIGNAL(released()), this, SLOT(sendSignalPowerOut()));

    connect(ui->submitWeight, SIGNAL(released()), this, SLOT(weightButton_clicked()));


    QTimer::singleShot(500, this, SLOT(checkForNewRequests()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete threadZero;
    delete threadOne;
    delete threadTwo;
}

//Needed to implement a non-blocking delay, as regular sleep functions block the code (and would make the elevator 'jump' to the floor
void MainWindow::checkForNewRequests(){
    Elevator e = controlSystem.serviceNextRequest();
    if (e.getElevatorID() != -1){
        elevatorNumber = e.getElevatorID();
        ui->textBrowser->append("Elevator " + QString::number(elevatorNumber) + " accepts");
        Floor f = e.removeServicedRequest();
        if (f.getFloorNumber()!= -1){
            assignFloorToElevator(elevatorNumber, f.getFloorNumber(), f.getGoingUp());
        }
    }

    //This part implements the polling mechanism, which continually checks for new requests every x milliseconds
    QTimer::singleShot(1000, this, SLOT(checkForNewRequests()));
}

bool MainWindow::checkIfOverCapacity(int e){
    if (controlSystem.getElevatorWeight(e) > maxCapacity){
        ui->textBrowser->append("'CAN'T MOVE - Current Weight exceeds max Capacity");
        return true;
    }
    return false;
}

bool MainWindow::checkForObstruction(int e){
    if (controlSystem.getDoorsClosed(e) == false){
        ui->textBrowser->append("CANT MOVE - Doors are open");
        return true;
    }
    return false;
}

void MainWindow::setCurrentElevator(int e){
    mutexCurEle.lock();
    if(elevatorNumber!=-1){
        controlSystem.closeDoors(elevatorNumber); //weird error was happeneing when elevator switches mid pickup
    }
    this->elevatorNumber=e;
    ui->elevatorNumber->clear();
    ui->elevatorNumber->append("Elevator " + QString::number(elevatorNumber));
    mutexCurEle.unlock();
    return;
}

void MainWindow::setCurrentFloor(int f){
    mutexCurFlr.lock();
    ui->floorNumber->clear();
    ui->floorNumber->append("Floor " + QString::number(f));
    mutexCurFlr.unlock();
    return;
}

void MainWindow::onElevatorMoved(int e, int f, bool goingUp){
    mutexUi.lock();
    if (e==0 && goingUp)ui->elevatorZero->move(ui->elevatorZero->x(), ui->elevatorZero->y()-40);
    if (e==0 && !goingUp)ui->elevatorZero->move(ui->elevatorZero->x(), ui->elevatorZero->y()+40);

    if (e==1 && goingUp)ui->elevatorOne->move(ui->elevatorOne->x(), ui->elevatorOne->y()-40);
    if (e==1 && !goingUp)ui->elevatorOne->move(ui->elevatorOne->x(), ui->elevatorOne->y()+40);

    if (e==2 && goingUp)ui->elevatorTwo->move(ui->elevatorTwo->x(), ui->elevatorTwo->y()-40);
    if (e==2 && !goingUp)ui->elevatorTwo->move(ui->elevatorTwo->x(), ui->elevatorTwo->y()+40);

    controlSystem.moveElevatorToFloor(e, f, goingUp);

    if(elevatorNumber == e) setCurrentFloor(f);
    if (elevatorNumber == e) this->goingUp=goingUp;
    mutexUi.unlock();
    return;
}

bool MainWindow::onOpenDoors(int e){
    QString resOpen = QString::fromStdString(controlSystem.openDoors(e));
    ui->textBrowser->append("Elevator " + QString::number(e) +  " " + resOpen);
    return true;
}

bool MainWindow::onCloseDoors(int e){
    QString resClose = QString::fromStdString(controlSystem.closeDoors(e));
    ui->textBrowser->append("Elevator " + QString::number(e) +  " " + resClose);
    return true;
}

//to use when all elevators are servicing, and we need to queue a floor (or queue a dest floor)
int MainWindow::assignFloorToElevator(int e, int f, bool goingUp){
    mutexZero.lock();
    bool zeroInTransit = threadZero->inTransit;
    mutexZero.unlock();

    mutexOne.lock();
    bool oneInTransit = threadOne->inTransit;
    mutexOne.unlock();

    mutexTwo.lock();
    bool twoInTransit = threadTwo->inTransit;
    mutexTwo.unlock();

    if (checkForObstruction(e) || checkIfOverCapacity(e)) {
        controlSystem.receiveRequestToElevator(e, f, goingUp);
        return e;
    }

    if (e==0){
        if (zeroInTransit){ //if elevator is moving and the floor is closer we interrupt, otherwise we add it to the queue of floors
            mutexZero.lock();
            threadZero->addFloor(f);
            mutexZero.unlock();
            return 0;
        }

        if (!zeroInTransit){ //if elevator is idle, can just assign floor
            setCurrentElevator(0);
            threadZero->setObjectName("Thread Zero");
            mutexZero.lock();
            threadZero->goingUp=goingUp;
            threadZero->destDirection=goingUp;
            threadZero->addFloor(f);
            threadZero->getNextFloor();
            mutexZero.unlock();
            threadZero->start();
            return 0;
        }
    } else if (e==1){
        if (oneInTransit){
            mutexOne.lock();
            threadOne->addFloor(f);
            mutexOne.unlock();
            return 1;
        }

        if (!oneInTransit){
            setCurrentElevator(1);
            threadOne->setObjectName("Thread One");
            mutexOne.lock();
            threadOne->goingUp=goingUp;
            threadOne->destDirection=goingUp;
            threadOne->addFloor(f);
            threadOne->getNextFloor();
            mutexOne.unlock();
            threadOne->start();
            return 1;
        }
    } else if(e==2){
        if (twoInTransit){
            mutexTwo.lock();
            threadTwo->addFloor(f);
            mutexTwo.unlock();
            return 2;
        }

        if (!twoInTransit){
            setCurrentElevator(2);
            threadTwo->setObjectName("Thread Two");
            mutexTwo.lock();
            threadTwo->goingUp=goingUp;
            threadTwo->destDirection=goingUp;
            threadTwo->addFloor(f);
            threadTwo->getNextFloor();
            mutexTwo.unlock();
            threadTwo->start();
            return 2;
        }
    }
    return -1;
}

void MainWindow::userRequestGoingUp(){
    QString userFloor = ui->userCurrentFloor->text();
    if (userFloor.isEmpty() || userFloor.toInt() < 0 || userFloor.toInt() > 6) return;
    ui->textBrowser->append("userRequest from floor " + userFloor + " to go up");

    controlSystem.receiveRequest(userFloor.toInt(), true);
    this->goingUp=true;

    return;
}

void MainWindow::userRequestGoingDown(){
    QString userFloor = ui->userCurrentFloor->text();
    if (userFloor.isEmpty() || userFloor.toInt() < 0 || userFloor.toInt() > 6) return;
    ui->textBrowser->append("userRequest from floor " + userFloor + " to go down");

    controlSystem.receiveRequest(userFloor.toInt(), false);
    this->goingUp=false;

    return;
}

void MainWindow::destButtonClicked_0(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Destination Floor Button 0 Clicked in Elevator " + QString::number(elevatorNumber));
    controlSystem.receiveDestButtonRequest(elevatorNumber, 0);
    assignFloorToElevator(elevatorNumber, 0, goingUp); //directly adding it to the current elevator
}

void MainWindow::destButtonClicked_1(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Destination Floor Button 1 Clicked in Elevator " + QString::number(elevatorNumber));
    controlSystem.receiveDestButtonRequest(elevatorNumber, 1);
    assignFloorToElevator(elevatorNumber, 1, goingUp);
}

void MainWindow::destButtonClicked_2(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Destination Floor Button 2 Clicked in Elevator " + QString::number(elevatorNumber));
    controlSystem.receiveDestButtonRequest(elevatorNumber, 2);
    assignFloorToElevator(elevatorNumber, 2, goingUp);
}

void MainWindow::destButtonClicked_3(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Destination Floor Button 3 Clicked in Elevator " + QString::number(elevatorNumber));
    controlSystem.receiveDestButtonRequest(elevatorNumber, 3);
    assignFloorToElevator(elevatorNumber, 3, goingUp);
}

void MainWindow::destButtonClicked_4(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Destination Floor Button 4 Clicked in Elevator " + QString::number(elevatorNumber));
    controlSystem.receiveDestButtonRequest(elevatorNumber, 4);
    assignFloorToElevator(elevatorNumber, 4, goingUp);
}

void MainWindow::destButtonClicked_5(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Destination Floor Button 5 Clicked in Elevator " + QString::number(elevatorNumber));
    controlSystem.receiveDestButtonRequest(elevatorNumber, 5);
    assignFloorToElevator(elevatorNumber, 5, goingUp);
}

void MainWindow::destButtonClicked_6(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Destination Floor Button 6 Clicked in Elevator " + QString::number(elevatorNumber));
    controlSystem.receiveDestButtonRequest(elevatorNumber, 6);
    assignFloorToElevator(elevatorNumber, 6, goingUp);
}

void MainWindow::sendSignalHelp(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Attempting to send signal: Help to Elevator " + QString::number(elevatorNumber));
    QString res = QString::fromStdString(controlSystem.sendSignal(elevatorNumber, HELP));
    ui->textBrowser->append(res);
}

void MainWindow::sendSignalDoorObstacles(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Attempting to send signal: DoorObstacles to Elevator " + QString::number(elevatorNumber));
    QString res = QString::fromStdString(controlSystem.sendSignal(elevatorNumber, DOOROBSTACLES));
    ui->textBrowser->append(res);
}

void MainWindow::sendSignalFire(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Attempting to send signal: Fire to all elevaators");
    QString res = QString::fromStdString(controlSystem.sendSignal(elevatorNumber, FIRE));
    ui->textBrowser->append(res);

    threadZero->requestInterruption();
    threadOne->requestInterruption();
    threadTwo->requestInterruption();

    ui->textBrowser->append("Please exit the elevators in a safe fashion - there is a fire in the building");
}

void MainWindow::sendSignalOverLoad(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Attempting to send signal: OverLoad to Elevator " + QString::number(elevatorNumber));
    QString res = QString::fromStdString(controlSystem.sendSignal(elevatorNumber, OVERLOAD));
    ui->textBrowser->append(res);
}

void MainWindow::sendSignalPowerOut(){
    if (elevatorNumber==-1) return;
    ui->textBrowser->append("Attempting to send signal: PowerOut to Elevator" + QString::number(elevatorNumber));
    ui->textBrowser->append(QString::number(elevatorNumber));
    QString res = QString::fromStdString(controlSystem.sendSignal(elevatorNumber, POWEROUT));
    ui->textBrowser->append(res);

    if (elevatorNumber==0){
        threadZero->requestInterruption();

    } else if (elevatorNumber==1){
        threadOne->requestInterruption();

    } else if (elevatorNumber==2){
        threadTwo->requestInterruption();
    }

    ui->textBrowser->append("Please exit the elevator in a safe fashion - elevator system has lost power");
}


void MainWindow::weightButton_clicked()
{
    if (elevatorNumber==-1) return;
    QString weightSubmitted = ui->elevatorWeight->text();
    if (weightSubmitted.isEmpty()) return;
    int w = weightSubmitted.toInt();
    ui->textBrowser->append("Setting Elevator Weight to " + weightSubmitted);
    controlSystem.setElevatorWeight(elevatorNumber, w);

}
