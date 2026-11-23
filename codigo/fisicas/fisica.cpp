#include "fisica.h"

Fisica::Fisica(double x0, double y0)
    : x0(x0), y0(y0), t(0.0), x(x0), y(y0) {}

Fisica::~Fisica() {}

double Fisica::getX() const {
    return x;
}

double Fisica::getY() const {
    return y;
}

void Fisica::setPosicionInicial(double x0, double y0) {
    this->x0 = x0;
    this->y0 = y0;
    reiniciar();
}

void Fisica::reiniciar() {
    t = 0.0;
    x = x0;
    y = y0;
}

bool Fisica::haTerminado() const {
    return false;
}