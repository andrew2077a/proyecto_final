#ifndef MOVPARABOLICO_H
#define MOVPARABOLICO_H

#include "fisicas/fisica.h"

// Tiro parabolico. Se usa en el salto de Rei, en la caida libre (v0 = 0)
// y en el disparo apuntado del monstruo.
//   x = x0 + dirX * v0 * cos(theta) * t
//   y = y0 - v0 * sen(theta) * t + g * t^2 / 2
class MovParabolico : public Fisica
{
private:
    double v0;             // velocidad inicial
    double anguloGrados;
    double g;              // gravedad
    int dirX;              // +1 derecha, -1 izquierda
    double cosTheta;
    double senTheta;

public:
    MovParabolico(double x0, double y0);
    void setParametros(double v0, double anguloGrados, double g, int dirX);
    void actualizar(double dt) override;
};

#endif
