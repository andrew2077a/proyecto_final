#include "percepcion.h"
#include "entidades/rei.h"
#include <stdexcept>

Percepcion::Percepcion(int numZonas, double xMin, double xMax)
    : numZonas(numZonas), xMin(xMin), xMax(xMax)
{
    if (numZonas <= 0) {
        throw std::invalid_argument("Percepcion: numZonas debe ser mayor que 0");
    }
    if (xMax <= xMin) {
        throw std::invalid_argument("Percepcion: xMax debe ser mayor que xMin");
    }
}

int Percepcion::percibir(const Rei& rei) const {
    return zonaDe(rei.getX());
}

// zona = parte entera de (x - xMin) / (xMax - xMin) * numZonas,
// con x limitada al rango para que siempre caiga en 0 .. numZonas-1.
int Percepcion::zonaDe(double x) const {
    if (x < xMin) x = xMin;
    int zona = static_cast<int>((x - xMin) / (xMax - xMin) * numZonas);
    if (zona >= numZonas) zona = numZonas - 1;   // x >= xMax cae en la ultima zona
    return zona;
}

// x del centro de la zona: xMin + (zona + 0,5) * (xMax - xMin) / numZonas
double Percepcion::centroDeZona(int zona) const {
    if (zona < 0 || zona >= numZonas) {
        throw std::out_of_range("Percepcion: la zona no existe");
    }
    return xMin + (zona + 0.5) * (xMax - xMin) / numZonas;
}
