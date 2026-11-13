#ifndef ONDAEMPUJE_H
#define ONDAEMPUJE_H

#include "fisicas/fisica.h"
class OndaEmpuje : public Fisica
{
private:
    double F;
    double R;
    double k;
    int dirX;
    double distancia;
    double v0;
    double desplazamiento;
    double delta;
    double umbralFin;

    void calcularV0();

public:
    OndaEmpuje(double x0, double y0);
    void setParametros(double F, double R, double k, int dirX);
    void setDistancia(double d);
    void actualizar(double dt) override;
    double getDelta() const;
    bool haTerminado() const override;
    void reiniciar() override;
};

#endif
