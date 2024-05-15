#ifndef PAINTER_H
#define PAINTER_H

#include <QVector>
#include "spacecraft.h"
#include "planet.h"
#include <QMutex>
#include <QWaitCondition>
#include <QPixmap>
#include <QThread>
#include<QSemaphore>
#include<QLabel>
#include<QObject>
#include<memory>
//#include "game.h"

class Painter : public QThread
{
    Q_OBJECT
private:
    std::shared_ptr<Spacecraft> sc;
    std::shared_ptr<QVector<Planet>> planetList;
    QPixmap *pix;
    std::shared_ptr<QSemaphore> mutex;
    QLabel *imgLab;
    //Game *g;
    Vector2d zero;
    std::shared_ptr<Vector2d> ptA, ptB;
    volatile bool keepRunning;  // Might be changed by main program (game.cpp).
    QColor col;
public:
    Painter(std::shared_ptr<Spacecraft> s, std::shared_ptr<QVector<Planet>> pl,
            QPixmap *p, std::shared_ptr<QSemaphore> m, QLabel *i, std::shared_ptr<Vector2d> pointA,
            std::shared_ptr<Vector2d> pointB, QThread *parent = nullptr);
    void plotLine(Vector2d *A, Vector2d *B);
    //void plotLines();
    void plotPlanets();
    void plotPlanet(const Planet& pl, QColor &col);
    void plot_visual_direction(QPixmap *pic, float angle, QColor col);
    double px2m(double p) { return p*200000; }
    double m2px(double m) { return m/200000; }
    void setZero(const Vector2d &z) { zero = z; }
    //void setPtA(Vector2d *z) { ptA = z; }
    //void setPtB(Vector2d *z) { ptB = z; }
    void setKeepRunning(bool b) { keepRunning = b; }
    void setColor(QColor c) { col = c; }
protected:
    void run();
signals:
    void paintFinished();
};

#endif // PAINTER_H
