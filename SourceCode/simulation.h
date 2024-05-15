#ifndef SIMULATION_H
#define SIMULATION_H

#include<QPixmap>
#include "spacecraft.h"
#include "planet.h"
#include<QThread>
#include<QVector>
#include<QSemaphore>
#include<memory>

class Simulation : public QThread
{
    Q_OBJECT
private:
    QPixmap *pix;
    std::shared_ptr<QVector<Planet>> planetList;
    std::shared_ptr<Spacecraft> sc;
    std::shared_ptr<QSemaphore> mutex;
    double gamma; //gravitational constant
    double pi2; //2*pi
    double maxSimulationTime;
    double v,alpha; //from Textfields
    int picWidth,picHeight;
    int zero[2];
    std::shared_ptr<Vector2d> ptA, ptB;
    volatile bool keepRunning;  // Might be changed by main process (game.cpp)
    int colPlanetNr;  // collision planet number

public:
    Simulation(std::shared_ptr<Spacecraft> s, std::shared_ptr<QVector<Planet>> pl,
               std::shared_ptr<QSemaphore> m, std::shared_ptr<Vector2d> pointA,
               std::shared_ptr<Vector2d> pointB, QThread *parent=nullptr);
    ~Simulation();
    int collision(Vector4d A, Vector4d B);
    Vector4d F();
    double px2m(double p) { return p*200000; }
    double m2px(double m) { return m/200000; }
    void setV(double vel) { v = vel; }
    void setAlpha(double a) { alpha = a; }
    void setpicWidth(int pw) { picWidth = pw; }
    void setpicHeight(int ph) { picHeight = ph; }
    void setZero(int z[2]) { zero[0] = z[0]; zero[1] = z[1]; }
    //void setPtA(Vector2d *z) { ptA = z; }
    //void setPtB(Vector2d *z) { ptB = z; }
    void setKeepRunning(bool b) { keepRunning = b; }
    void resetColPlNr() { colPlanetNr = -1; }
    bool getKR() { return keepRunning; }
    int getColPlNr() { return colPlanetNr; }

protected:
    void run();
signals:
    void collision();
};

#endif // SIMULATION_H
