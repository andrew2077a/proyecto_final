#ifndef OBSTACULO_H
#define OBSTACULO_H
#include "entidades/entidad.h"

// Elemento fijo del nivel 2; lo que hace depende del tipo de sprite (BLOQUE, FOSA o PUAS)
class Obstaculo: public Entidad{
private:
    double espera;          // segundos entre un daño de las puas y el siguiente
    double esperaActual;    // lo que falta para que puedan dañar otra vez

public:
    Obstaculo(TipoSprite tipo, double x, double y, double ancho, double alto,double espera);
    ~Obstaculo()override;
    void alTocarRei(Rei& rei,int danio)override;
    void actualizar(double dt)override;
    bool esSolido()const;
};

#endif // OBSTACULO_H
