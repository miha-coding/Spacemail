#include "spacecraft.h"
#include<QDebug>

Spacecraft::Spacecraft()
{
    pos = Vector2d();
    v = Vector2d();

    // Triangle for auxiliary plot
    t.A = Vector2d(-1, 2)*5;
    t.B = Vector2d(1, 2)*5;
    t.C = Vector2d(0, -2)*5;
}


Vector4d Spacecraft::getPosVel() {
    return Vector4d(pos, v);
}

void Spacecraft::setPosVel(Vector4d pv) {
    pos(0) = pv(0);
    pos(1) = pv(1);
    v(0) = pv(2);
    v(1) = pv(3);
}
