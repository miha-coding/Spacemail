#ifndef PLANET_H
#define PLANET_H

#include "vektor.h"

class Planet
{
private:
    Vector2d position;
    double mass;
    double rad;
    short state;//2 StartPlanet, 1 goalPlanet, 0 NormalPlanet

public:
    Planet(double x=0, double y=0, double r=1);
    ~Planet();
    Vector2d getPosition() const{ return position; }
    double getMass() const { return mass; }
    double getRadius() const { return rad; }
    short getState() const { return state; }

    void setPosition(Vector2d pos) { position = pos; }
    void setPosition(double x,double y) { position(0) = x; position(1) = y; }
    void setMass(double m) { mass = m; }
    void setRadius(double r) { rad = r; }
    void setState(short st) { state = st; }
};

#endif // PLANET_H
