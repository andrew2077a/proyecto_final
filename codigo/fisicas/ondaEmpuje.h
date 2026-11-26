#ifndef ONDAEMPUJE_H
#define ONDAEMPUJE_H

#include "fisicas/fisica.h"

// Onda de empuje de la explosion de una bomba del nivel 2. Empuja a Rei
// con una velocidad que se va frenando (movimiento amortiguado):
//   v(t)      = v0 * e^(-k t)
//   desplaz.  = (v0 / k) * (1 - e^(-k t))
// v0 depende de la distancia a la bomba: v0 = F * (1 - d / R)
class OndaEmpuje : public Fisica
{
private:
    double F;               // fuerza del empuje a distancia cero
    double R;               // radio de alcance
    double k;               // amortiguamiento: mayor k, el empuje se frena antes
    int dirX;               // sentido del empuje: +1 o -1
    double distancia;       // de la bomba a Rei
    double v0;              // velocidad inicial, ya calculada
    double desplazamiento;  // recorrido total acumulado
    double delta;           // recorrido de este frame, con signo
    double umbralFin;       // por debajo de esta velocidad la onda termina

    void calcularV0();

public:
    OndaEmpuje(double x0, double y0);
    void setParametros(double F, double R, double k, int dirX);
    void setDistancia(double d);
    void actualizar(double dt) override;
    double getDelta() const;   // lo que Rei debe sumar a su x en este frame
    bool haTerminado() const override;
    void reiniciar() override;
};

#endif
