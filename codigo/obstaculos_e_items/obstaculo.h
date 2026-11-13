#ifndef OBSTACULO_H
#define OBSTACULO_H
#include "entidades/entidad.h"
class Obstaculo: public Entidad{
private:
    double espera;
    double esperaActual;

public:
    Obstaculo(TipoSprite tipo, double x, double y, double ancho, double alto,double espera);
    ~Obstaculo()override;
    void alTocarRei(Rei& rei,int danio)override;
    void actualizar(double dt)override;
    bool esSolido()const;
};

#endif // OBSTACULO_H
