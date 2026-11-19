#include "razonamiento.h"

#include "razonamiento.h"
#include "aprendizaje.h"
#include <random>
#include <stdexcept>

namespace {
std::mt19937& generador() {
    static std::mt19937 gen{std::random_device{}()};
    return gen;
}
}

Razonamiento::Razonamiento(int numZonas, double suavizadoMin)
    : numZonas(numZonas), probabilidades(nullptr), suavizadoMin(suavizadoMin)
{
    if (numZonas <= 0) {
        throw std::invalid_argument("Razonamiento: numero de zonas debe ser mayor que 0");
    }
    if (suavizadoMin < 0.0 || suavizadoMin > 1.0) {
        throw std::invalid_argument("Razonamiento: suavizado minimo debe estar entre 0 y 1");
    }
    probabilidades = new double[numZonas];
    // Sin historial todas las zonas son igual de probables
    for (int i = 0; i < numZonas; ++i) {
        probabilidades[i] = 1.0 / numZonas;
    }
}

Razonamiento::~Razonamiento() {
    delete[] probabilidades;
}

// Divide cada probabilidad entre la suma. Si la suma es 0 (no hay nada que repartir) deja todas las zonas igual de probables.
void Razonamiento::normalizar() {
    double suma = 0.0;
    for (int i = 0; i < numZonas; ++i) {
        suma += probabilidades[i];
    }
    for (int i = 0; i < numZonas; ++i) {
        probabilidades[i] = (suma > 0.0) ? probabilidades[i] / suma : 1.0 / numZonas;
    }
}

void Razonamiento::calcularProbabilidades(const Aprendizaje& apr) {
    if (apr.getNumZonas() != numZonas) {
        throw std::invalid_argument("Razonamiento: Aprendizaje con distinto numero de zonas");
    }
    for (int i = 0; i < numZonas; ++i) {
        const double peso = 1.0 + apr.getTasaAcierto(i);
        probabilidades[i] = apr.getFrecuencia(i) * peso;
    }
    normalizar();
}

// Sube a suavizado minimo toda probabilidad menor y vuelve a normalizar.
void Razonamiento::suavizar() {
    for (int i = 0; i < numZonas; ++i) {
        if (probabilidades[i] < suavizadoMin) {
            probabilidades[i] = suavizadoMin;
        }
    }
    normalizar();
}

// Ruleta: sortea r en [0, 1) y recorre las probabilidades acumuladas.
int Razonamiento::decidir() const {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    const double r = dist(generador());

    double acumulada = 0.0;
    for (int i = 0; i < numZonas; ++i) {
        acumulada += probabilidades[i];
        if (r < acumulada) {
            return i;
        }
    }
    return numZonas - 1;   // por redondeo, la suma puede quedar apenas bajo 1

}



