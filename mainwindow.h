#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "defs_Centralized.h"
#include "ECS_Centralized.h"
#include <QTimer>
#include "myThread.h"
#include <QThread>
#include <QMutex>
#include <QDebug>

//With QThreads you move a class to it, but have to make sure the class does not have a parent

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int assignFloorToElevator(int, int, bool);
    void setCurrentElevator(int);
    void setCurrentFloor(int);
    bool checkIfOverCapacity(int);
    bool checkForObstruction(int);

private:
    Ui::MainWindow *ui;
    ECS controlSystem;
    int elevatorNumber=-1;
    bool goingUp=true;

    myThread *threadZero;
    myThread *threadOne;
    myThread *threadTwo;

    QMutex mutexZero;
    QMutex mutexOne;
    QMutex mutexTwo;
    QMutex mutexUi;
    QMutex mutexCurEle;
    QMutex mutexCurFlr;


public slots:
    void onElevatorMoved(int, int, bool);
    bool onOpenDoors(int);
    bool onCloseDoors(int);
    bool openDoors() {return onOpenDoors(elevatorNumber);}
    bool closeDoors() {return onCloseDoors(elevatorNumber);}


    void checkForNewRequests();

private slots:
    void userRequestGoingUp();
     void userRequestGoingDown();

    void destButtonClicked_0();
     void destButtonClicked_1();
      void destButtonClicked_2();
       void destButtonClicked_3();
        void destButtonClicked_4();
         void destButtonClicked_5();
          void destButtonClicked_6();

    void sendSignalHelp();
     void sendSignalDoorObstacles();
      void sendSignalFire();
       void sendSignalOverLoad();
        void sendSignalPowerOut();

     void weightButton_clicked();
};
#endif // MAINWINDOW_H
