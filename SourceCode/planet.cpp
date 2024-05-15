#include "planet.h"
#include <QtGlobal>
#include <QDateTime>
#include <stdlib.h>
#include<time.h>
#include<QDebug>
Planet::Planet(double x,double y,double r)
{
    position(0) = x;
    position(1) = y;
    qDebug() << "Planet::Planet: Position = " << position(0) << ", " << position(1);
    this->rad = r; //Later: random Radius
    this->mass = 18468*pow(rad,3)*3.14; //accuracy is not so important for the weight (density of earth is used)
    this->state=0; //std state: Normal Planet
}

Planet::~Planet() { }
