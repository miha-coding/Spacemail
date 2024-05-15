#ifndef SPACECRAFT_H
#define SPACECRAFT_H

#include<QDebug>
#include "vektor.h"

struct triangle { Vector2d A, B, C; };

class Spacecraft
{
private:
    Vector2d pos;
    Vector2d v;
    triangle t;
public:
    Spacecraft();
    Vector2d getPosition() { return pos; }
    Vector2d getVelocity() { return v; }
    Vector4d getPosVel();
    void setPosition(Vector2d p) { pos=p; }
    void setPosition(double x, double y) { pos(0)=x; pos(1)=y; }
    void setVelocity(Vector2d vel) { v=vel;  }
    void setVelocity(double vx, double vy) { v(0)=vx; v(1)=vy; }
    void setPosVel(Vector4d pv);
    struct triangle getTriangle() { return t; }
};

#endif // SPACECRAFT_H
