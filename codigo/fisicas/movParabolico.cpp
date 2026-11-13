#include "movParabolico.h"
#include <cmath>

namespace {
const double PI = std::acos(-1.0);
}
MovParabolico::MovParabolico(double x0, double y0)
    : Fisica(x0, y0), v0(0.0), anguloGrados(0.0), g(0.0), dirX(1),
    cosTheta(1.0), senTheta(0.0) {}

void MovParabolico::setParametros(double v0, double anguloGrados, double g, int dirX) {
    this->v0 = v0;
    this->anguloGrados = anguloGrados;
    this->g = g;
    this->dirX = (dirX < 0) ? -1 : 1;   // solo se admite +1 o -1

    const double rad = anguloGrados * PI / 180.0;
    cosTheta = std::cos(rad);
    senTheta = std::sin(rad);
}

void MovParabolico::actualizar(double dt) {
    t += dt;
    x = x0 + dirX * v0 * cosTheta * t;
    y = y0 - v0 * senTheta * t + 0.5 * g * t * t;
}
