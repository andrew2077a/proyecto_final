#ifndef PERCEPCION_H
#define PERCEPCION_H

class Rei;

// Tarea 1 del agente: observa a Rei y ubica su posicion en una zona.
// Reduce infinitas posiciones a pocas zonas manejables, lo que hace posible
// llevar una estadistica de donde suele estar Rei.
class Percepcion
{
private:
    int numZonas;
    double xMin;
    double xMax;

public:
    Percepcion(int numZonas, double xMin, double xMax);

    int percibir(const Rei& rei) const;
    int zonaDe(double x) const;
    double centroDeZona(int zona) const;
};

#endif
