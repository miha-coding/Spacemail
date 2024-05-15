#ifndef VEKTOR_H
#define VEKTOR_H

#include<QDebug>
#include<cmath>
#include<iostream>
#include<QPoint>

class Vector2d{
private:
    double x, y;
public:
    Vector2d(double vx=0, double vy=0): x(vx), y(vy) {};
    Vector2d(const Vector2d &v) : x(v.x), y(v.y) {};
    Vector2d& operator=(const Vector2d &v2);
    Vector2d& operator+=(const Vector2d &v2);
    Vector2d& operator-=(const Vector2d &v2);
    Vector2d& operator*=(const double c);
    Vector2d& operator/=(const double c);
    friend Vector2d operator+(const Vector2d &v1, const Vector2d &v2);
    friend Vector2d operator-(const Vector2d &v1, const Vector2d &v2);
    friend double operator*(const Vector2d &v1, const Vector2d &v2);
    friend Vector2d operator*(const Vector2d &v, const double d);
    friend Vector2d operator*(const double d, const Vector2d &v);
    //Vector2d operator+(const Vector2d &v2);
    double operator()(unsigned int i) const;
    double& operator()(unsigned int i);
    double norm(int i=2);
    QPoint rot_trans(const double angle, const Vector2d &s);
    void print() { std::cout << x << " " << y << std::endl; }
    //evtl auch inplace: void rotate(const float angle);
    // ...
};

class Vector4d{
private:
    double v, w, x, y;
public:
    Vector4d(double vv=0, double vw=0, double vx=0, double vy=0): v(vv), w(vw), x(vx), y(vy) {};
    Vector4d(const Vector4d &v) : v(v.v), w(v.w), x(v.x), y(v.y) {};
    Vector4d(const Vector2d &v1, const Vector2d &v2) : v(v1(0)), w(v1(1)), x(v2(0)), y(v2(1)) {};
    Vector4d& operator=(const Vector4d &v2);
    Vector4d& operator+=(const Vector4d &v2);
    Vector4d& operator-=(const Vector4d &v2);
    Vector4d& operator*=(const double c);
    Vector4d& operator/=(const double c);
    friend Vector4d operator+(const Vector4d &v1, const Vector4d &v2);
    friend Vector4d operator-(const Vector4d &v1, const Vector4d &v2);
    friend Vector4d operator*(const Vector4d &v, const double d);
    friend Vector4d operator/(const Vector4d &v, const double d);
    double operator()(unsigned int i) const;
    double& operator()(unsigned int i);
    void print() { std::cout << v << " " << w << " " << x << " " << y << std::endl; }
    //evtl auch inplace: void rotate(const float angle);
    // ...
};

#endif // VEKTOR_H
