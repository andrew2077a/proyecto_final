#ifndef PERCEPCION_H
#define PERCEPCION_H

class Rei;

// Tarea 1 del agente: ubica a Rei en una zona (pocas zonas en vez de infinitas posiciones).
class Percepcion
{
private:
    int numZonas;
    double xMin;   // inicio del rango dividido en zonas
    double xMax;   // fin del rango

public:
    Percepcion(int numZonas, double xMin, double xMax);

    int percibir(const Rei& rei) const;
    int zonaDe(double x) const;
    double centroDeZona(int zona) const;
};

#endif
