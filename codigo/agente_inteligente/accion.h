#ifndef ACCION_H
#define ACCION_H

#include "nucleo/nivel1.h"
class Proyectil;

// Tarea 4 del agente: convierte la decision en un disparo.
class Accion
{
private:
    double gravedad;
    double anguloDisparo;   // en grados
    double velocidadMax;    // tope de la velocidad inicial
    int danio;

public:
    Accion(const ParamsNivel1& p1, double gravedad);

    Proyectil* ejecutar(int zona, double centroX, double origenX, double origenY);
};

#endif
