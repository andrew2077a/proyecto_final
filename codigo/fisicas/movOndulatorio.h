#ifndef MOVONDULATORIO_H
#define MOVONDULATORIO_H

#include "fisicas/fisica.h"
//   x = x0 + vx * t
//   y = y0 + A * sen(w * t + fase)      (y0 es la linea central de la onda)
class MovOndulatorio : public Fisica
{
private:
    double vx;
    double amplitud;
    double frecuencia;
    double fase;

public:
    MovOndulatorio(double x0, double y0);
    void setParametros(double vx, double amplitud, double frecuencia, double fase);
    void actualizar(double dt) override;
};

#endif