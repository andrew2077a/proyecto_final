#include "ondaEmpuje.h"
#include <cmath>
#include <stdexcept>

OndaEmpuje::OndaEmpuje(double x0, double y0)
    : Fisica(x0, y0), F(0.0), R(1.0), k(1.0), dirX(1), distancia(0.0),
    v0(0.0), desplazamiento(0.0), delta(0.0), umbralFin(1.0) {}

void OndaEmpuje::setParametros(double F, double R, double k, int dirX) {
    if (R <= 0.0 || k <= 0.0) {
        throw std::invalid_argument("OndaEmpuje: R y k deben ser mayores que 0");
    }
    this->F = F;
    this->R = R;
    this->k = k;
    this->dirX = (dirX < 0) ? -1 : 1;
    calcularV0();
}

void OndaEmpuje::setDistancia(double d) {
    distancia = d;
    calcularV0();
}

// v0 = F * (1 - d/R); fuera del radio (d >= R) no hay empuje.
void OndaEmpuje::calcularV0() {
    double d = distancia;
    if (d < 0.0) d = 0.0;
    v0 = (d >= R) ? 0.0 : F * (1.0 - d / R);
}

void OndaEmpuje::actualizar(double dt) {
    t += dt;
    const double nuevo = (v0 / k) * (1.0 - std::exp(-k * t));
    delta = dirX * (nuevo - desplazamiento);   // con signo: Rei solo lo suma
    desplazamiento = nuevo;
    x = x0 + dirX * desplazamiento;
    y = y0;
}

double OndaEmpuje::getDelta() const {
    return delta;
}

bool OndaEmpuje::haTerminado() const {
    return v0 * std::exp(-k * t) < umbralFin;
}

void OndaEmpuje::reiniciar() {
    Fisica::reiniciar();
    desplazamiento = 0.0;
    delta = 0.0;
}