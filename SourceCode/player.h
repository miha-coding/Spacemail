#ifndef PLAYER_H
#define PLAYER_H

#include<QString>
#include<QColor>

class Player
{
private:
    QString name;
    QColor col;
    double lastVelocity;
    double lastAngle;
public:
    Player(QString n,QColor c=QColor(),double v=0,double a=0);
    void setName(QString n) { name = n; }
    void setCol(QColor c) { col = c; }
    void setCol(QString c) { col.setNamedColor(c); }
    void setLastV(double v) { lastVelocity = v; }
    void setLastA(double a) { lastAngle = a; }
    QString getName() const { return name; }
    QColor getCol() const { return col; }
    double getLastAngle() const { return lastAngle; }
    double getLastVelocity() const { return lastVelocity; }
};

#endif // PLAYER_H
