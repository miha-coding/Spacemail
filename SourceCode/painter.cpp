#include "painter.h"
#include<QPainter>
#include<QPolygon>
#include<QPoint>
#include<spacecraft.h>

Painter::Painter(std::shared_ptr<Spacecraft> s, std::shared_ptr<QVector<Planet> > pl, QPixmap *p,
                 std::shared_ptr<QSemaphore> m, QLabel *i, std::shared_ptr<Vector2d> pointA,
                 std::shared_ptr<Vector2d> pointB, QThread *parent) :
    QThread(parent), sc(s), planetList(pl), pix(p), mutex(m), imgLab(i), ptA(pointA), ptB(pointB)
{
    keepRunning = 1;
    col = QColor(0,255,0);
}

void Painter::run() {
    while(keepRunning){
        while(mutex.get()->available()==0) { if(!keepRunning) return; };
        //draw
        plotLine(ptA.get(), ptB.get());
        //qDebug() << "PlotLine(A.B)";
        mutex.get()->acquire(); // #mut == 0
        msleep(100);

    }

}

void Painter::plotLine(Vector2d *A, Vector2d *B) {
    QPainter paint;

    bool ok = paint.begin(pix);
    if(!ok) qDebug() << "Painter::plotLine: Painter Fehler";
    else {
        paint.setPen(col);
        //qDebug() << "Painter Points" << m2px(A[0])+zero[0] << m2px(A[1])+zero[1] << m2px(B[0])+zero[0] << m2px(B[1])+zero[1];
        paint.drawLine(m2px((*A)(0))+zero(0) , m2px((*A)(1))+zero(1) , m2px((*B)(0))+zero(0) , m2px((*B)(1))+zero(1));
        paint.end();
        emit paintFinished();
    }

}

void Painter::plotPlanets() {
    // Plot all planets.

    QColor col(255,255,255);
    for(int i=0;i<planetList.get()->size();i++) {
        plotPlanet(planetList.get()->operator[](i), col);
    }

}

void Painter::plotPlanet(const Planet& pl, QColor &col) {
    QPainter paint;
    paint.begin(pix);
    paint.setPen(col);
    paint.setBrush(col); // Draw a filled ellipse
    Vector2d r = pl.getPosition();
    double rad = pl.getRadius();
    paint.drawEllipse(QPointF(zero(0)+m2px(r(0)), zero(1)+m2px(r(1))), m2px(rad), m2px(rad));
    paint.end();
}

void Painter::plot_visual_direction(QPixmap *pic, float angle, QColor col) {
    QPainter paint;
    bool ok = paint.begin(pic);
    paint.setPen(col);
    if(!ok) {
        qDebug() << "painter::plot_vis_dir: Painter could not be initialized!";
    }

    struct triangle t = sc.get()->getTriangle();
    QPolygon triangle;
    triangle << t.A.rot_trans(angle, zero) << t.C.rot_trans(angle, zero) << t.B.rot_trans(angle, zero);// << t.A.rot_trans(angle, zero);
    paint.drawPolygon(triangle);
    paint.end();

}
