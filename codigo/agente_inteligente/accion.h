#ifndef ACCION_H
#define ACCION_H

#include "nucleo/nivel1.h"
class Proyectil;

// Tarea 3 del agente: convierte la decision en un disparo. Es la unica parte
// del agente que produce una interaccion visible en el juego.
class Accion
{
private:
    double gravedad;
    double anguloDisparo;
    double velocidadMax;
    int danio;

public:
<<<<<<< HEAD
    Accion(const Nivel1::ParamsNivel1& p1, double gravedad);
=======
    Accion(const ParamsNivel1& p1, double gravedad);
>>>>>>> origin/andres


    Proyectil* ejecutar(int zona, double centroX, double origenX, double origenY);
};

<<<<<<< HEAD
#endif
=======
#endif
>>>>>>> origin/andres
