#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QMutex>
#include <QDebug>
#include <iostream>
#include "defs_Centralized.h"

class myThread : public QThread
{
    Q_OBJECT
public:
    explicit myThread(QObject *parent = 0);
    void run();
    void getNextFloor();
    void addFloor(int f);
    void clearFloors();

    QMutex mutex;
    int id=-1;
    int currentFloor=0;
    int destFloor=0; //starts at ground floor

    bool goingUp=true; //starting from ground floor and going up by default
    bool inTransit=false;
    bool destDirection=false;

    int floorsToService [max_floors];
    int floors=0;


signals:
    void moveElevator(int, int, bool);
    void openDoors(int);
    void closeDoors(int);

public slots:


};

#endif // MYTHREAD_H
