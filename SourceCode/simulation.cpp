#include "simulation.h"
#include "vectorop.cpp"
#include<QPainter>
#include<QFile>
#include "planet.h"
#include<QTime>

Simulation::Simulation(std::shared_ptr<Spacecraft> s, std::shared_ptr<QVector<Planet> > pl,
                       std::shared_ptr<QSemaphore> m, std::shared_ptr<Vector2d> pointA,
                       std::shared_ptr<Vector2d> pointB, QThread *parent)
    : QThread(parent), planetList(pl), sc(s), mutex(m), ptA(pointA), ptB(pointB)
{
    gamma = 6.673848*pow(10, -11);
    pi2 = 6.2831853071795864769252;

    sc.get()->setPosition(0, 0);

    keepRunning = 1;
    colPlanetNr = -1;

}

Simulation::~Simulation() { }

void Simulation::run() {
    sc.get()->setPosition(0,0); //Start condition; later: position of Startplanet

    double angle = alpha*pi2/360;
    sc.get()->setVelocity(v*sin(angle)*1000, -v*cos(angle)*1000); // km/s->m/s
    double h = 0.01; //Simulation Stepwidth
    Vector4d tmp, tmp2; //Help-vector

    //Startvalues
    Vector4d A = sc.get()->getPosVel();
    Vector4d B = Vector4d(A); //Copy A;
    int i = -1;

    int continueSimulation = 2;
    while(continueSimulation==2 && keepRunning == 1) {

        //explicit euler (bad, but this is only a game and not science ;) )
        B += F() * h;
        sc.get()->setPosVel(B);

        i = i+1;

        //Plot if a collision occurs or the time is up
        if(!(1-i%10000)){ //consider (and plot) only every 10000th step
            continueSimulation = collision(A,B);
            if(continueSimulation<0) continueSimulation *= -1;
            //Waiting to change ptA and ptB (possible in use for plotting in painter)
            while(mutex.get()->available()!=0) {
                this->msleep(1);
            }

            if(continueSimulation!=2) {
                B += (A-B) * continueSimulation;
            }

            (*ptA.get())(0) = A(0); (*ptA.get())(1) = A(1);
            (*ptB.get())(0) = B(0); (*ptB.get())(1) = B(1);

            mutex.get()->release();
            A = sc.get()->getPosVel();
        }

    }
    if(keepRunning==1) emit collision(); //while-loop terminated, cause a collision occured;
    qDebug()<<"Simulation.run: End of Method";
}

int Simulation::collision(Vector4d A, Vector4d B) {
    // Checks if the space ship collided with a planet.

    Vector2d x;
    double r,t,d; // (r)adius, t is the point on line between a and b in which the
                  // distance between planet and line is minimal, (d)istance
    Vector2d tmp, tmp2; //Help-vector for deleteing adresses for preventing memory-gaps.

    Vector2d a, b;

    a(0) = A(0);a(1) = A(1);
    b(0) = B(0);b(1) = B(1);

    for(int i = 0;i<planetList.get()->size();i++) {
        x = planetList.get()->operator[](i).getPosition();
        r = planetList.get()->operator[](i).getRadius();
        tmp = b-a;
        tmp2 = x-a;

        t = tmp*tmp2/(pow(tmp.norm(2), 2));

        //minimal distance is not on the line a b => distance is minimal to point a or b
        if(t<0 || t>1) {
            tmp = a-x;
            tmp2 = b-x;
            d = min(tmp.norm(2),tmp2.norm(2));
        }
        else {
            tmp = b+t*(b-a);  // point on the line with minimal distance to planet
            tmp2 = tmp-x;  // distance vector
            d = tmp2.norm(2);
        }


        if (d<=r) { //distance smaller than radius => collision
            colPlanetNr = i;
            qDebug() << "Simulation::Collision: Collision with Planet " << i << " at distance" << d;
            qDebug() << "line-parameter t" << t;

            if(planetList.get()->operator[](i).getState()==1) {
                qDebug() << "Simulation::Collision: Zielplanet erreicht";
                return -t;
            }
            else{
                return t;
            }
        }

    }
    //no collision detected
    return 2;
}

Vector4d Simulation::F(/* get all Data from sc instance */) {
    // ODE
    Vector4d z;
    Vector2d tmp = sc.get()->getVelocity();
    z(0) = tmp(0); //Ort
    z(1) = tmp(1); //Ort

    double x = 0;
    double y = 0;
    double val = 0;

    Vector2d scPos, plPos;
    double aux; //Auxiliary variable for the following for-loop
    for(int i=0;i<planetList.get()->size();i++) {
        scPos = sc.get()->getPosition();
        plPos = planetList.get()->operator[](i).getPosition();

        val = scPos(0) - plPos(0);//planet Position Px -> m
        tmp = scPos-plPos;
        aux = pow(tmp.norm(2), 3);
        x += planetList.get()->operator[](i).getMass()*val/aux;
        val = scPos(1) - plPos(1);
        y += planetList.get()->operator[](i).getMass()*val/aux;
    }

    z(2) = -gamma*x; //Velocity
    z(3) = -gamma*y; //Velocity

    return z;
}
