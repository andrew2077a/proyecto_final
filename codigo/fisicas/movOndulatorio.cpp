#include "movOndulatorio.h"
#include <cmath>
MovOndulatorio::MovOndulatorio(double x0, double y0)
    : Fisica(x0, y0), vx(0.0), amplitud(0.0), frecuencia(0.0), fase(0.0) {}

void MovOndulatorio::setParametros(double vx, double amplitud, double frecuencia, double fase) {
    this->vx = vx;
    this->amplitud = amplitud;
    this->frecuencia = frecuencia;
    this->fase = fase;
}

void MovOndulatorio::actualizar(double dt) {
    t += dt;
    x = x0 + vx * t;
    y = y0 + amplitud * std::sin(frecuencia * t + fase);
}