#include "vektor.h"
#include<QDebug>
#include<cmath>


Vector2d& Vector2d::operator=(const Vector2d &v2) {
    x = v2.x;
    y = v2.y;
    return *this;
}

Vector2d& Vector2d::operator+=(const Vector2d &v2) {
    x += v2.x;
    y += v2.y;
    return *this;
}

Vector2d& Vector2d::operator-=(const Vector2d &v2) {
    x -= v2.x;
    y -= v2.y;
    return *this;
}

Vector2d& Vector2d::operator*=(const double c) {
    x *= c;
    y *= c;
    return *this;
}

Vector2d& Vector2d::operator/=(const double c) {
    if(c==0) {
        qDebug() << "Division by 0";
        throw "Division by 0";
    }
    x /= c;
    y /= c;
    return *this;
}

Vector2d operator+(const Vector2d &v1, const Vector2d &v2) {
    return Vector2d(v1.x+v2.x, v1.y+v2.y);
}

Vector2d operator-(const Vector2d &v1, const Vector2d &v2) {
    return Vector2d(v1.x-v2.x, v1.y-v2.y);
}

double operator*(const Vector2d &v1, const Vector2d &v2) {
    return v1.x*v2.x + v1.y*v2.y;
}

Vector2d operator*(const Vector2d &v, const double d) {
    return Vector2d(v.x*d, v.y*d);
}

Vector2d operator*(const double d, const Vector2d &v) {
    return v*d;
}

double Vector2d::operator()(unsigned int i) const {
    switch(i){
    case 0:
        return x;
    case 1:
        return y;
    default:
        throw "Vector2d::operator(): index out of range!";
    }
}

double& Vector2d::operator()(unsigned int i) {
    switch(i){
    case 0:
        return x;
    case 1:
        return y;
    default:
        throw "Vector2d::operator(): index out of range!";
    }
}

double Vector2d::norm(int i) {
    return pow((pow(x, i)+pow(y, i)), 1./i);
}

QPoint Vector2d::rot_trans(const double angle, const Vector2d &s) {
    return QPoint(cos(angle)*x-sin(angle)*y+s(0), sin(angle)*x+cos(angle)*y+s(1));
}


// 4D Vector


Vector4d& Vector4d::operator=(const Vector4d &v2) {
    v = v2.v;
    w = v2.w;
    x = v2.x;
    y = v2.y;
    return *this;
}

Vector4d& Vector4d::operator+=(const Vector4d &v2) {
    v += v2.v;
    w += v2.w;
    x += v2.x;
    y += v2.y;
    return *this;
}

Vector4d& Vector4d::operator-=(const Vector4d &v2) {
    v -= v2.v;
    w -= v2.w;
    x -= v2.x;
    y -= v2.y;
    return *this;
}

Vector4d& Vector4d::operator*=(const double c) {
    v *= c;
    w *= c;
    x *= c;
    y *= c;
    return *this;
}

Vector4d& Vector4d::operator/=(const double c) {
    if(c==0) {
        qDebug() << "Division by 0";
        throw "Vector4d::operator/=: Division by 0";
    }
    v /= c;
    w /= c;
    x /= c;
    y /= c;
    return *this;
}

Vector4d operator+(const Vector4d &v1, const Vector4d &v2) {
    return Vector4d(v1.v+v2.v, v1.w+v2.w, v1.x+v2.x, v1.y+v2.y);
}

Vector4d operator-(const Vector4d &v1, const Vector4d &v2) {
    return Vector4d(v1.v-v2.v, v1.w-v2.w, v1.x-v2.x, v1.y-v2.y);
}

Vector4d operator*(const Vector4d &v, const double d) {
    return Vector4d(v.v*d, v.w*d, v.x*d, v.y*d);
}

Vector4d operator/(const Vector4d &v, const double d) {
    if(d==0) {
        qDebug() << "Division by 0";
        throw "Vector4d::operator/: Division by 0";
    }
    return Vector4d(v.v/d, v.w/d, v.x/d, v.y/d);
}

double Vector4d::operator()(unsigned int i) const {
    switch(i){
    case 0:
        return v;
    case 1:
        return w;
    case 2:
        return x;
    case 3:
        return y;
    default:
        throw "Vector4d::operator(): Index out of range!";
    }
}


double& Vector4d::operator()(unsigned int i){
    switch(i){
    case 0:
        return v;
    case 1:
        return w;
    case 2:
        return x;
    case 3:
        return y;
    default:
        throw "Vector4d::operator(): Index out of range!";
    }
}
