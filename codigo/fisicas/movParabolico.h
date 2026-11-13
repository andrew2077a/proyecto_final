#ifndef MOVPARABOLICO_H
#define MOVPARABOLICO_H

#include "fisicas/fisica.h"
class MovParabolico : public Fisica
{
private:
    double v0;
    double anguloGrados;
    double g;
    int dirX;
    double cosTheta;
    double senTheta;

public:
    MovParabolico(double x0, double y0);
    void setParametros(double v0, double anguloGrados, double g, int dirX);
    void actualizar(double dt) override;
};

#endif