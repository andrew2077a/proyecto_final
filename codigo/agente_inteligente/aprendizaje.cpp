#include "aprendizaje.h"
#include <stdexcept>

Aprendizaje::Aprendizaje(int numZonas, double factorOlvido)
    : numZonas(numZonas), visitas(nullptr), intentos(nullptr), aciertos(nullptr),
    factorOlvido(factorOlvido)
{
    if (numZonas <= 0) {
        throw std::invalid_argument("Aprendizaje: numZonas debe ser mayor que 0");
    }
    if (factorOlvido < 0.0 || factorOlvido > 1.0) {
        throw std::invalid_argument("Aprendizaje: factorOlvido debe estar entre 0 y 1");
    }
    // () al final: los arreglos nacen en cero
    visitas = new double[numZonas]();
    intentos = new double[numZonas]();
    aciertos = new double[numZonas]();
}

Aprendizaje::~Aprendizaje() {
    delete[] visitas;
    delete[] intentos;
    delete[] aciertos;
}

// Evita salirse de los arreglos
void Aprendizaje::validarZona(int zona) const {
    if (zona < 0 || zona >= numZonas) {
        throw std::out_of_range("Aprendizaje: la zona no existe");
    }
}

// Se vio a Rei en esta zona
void Aprendizaje::registrar(int zona) {
    validarZona(zona);
    visitas[zona] += 1.0;
}

// Resultado de un disparo a esta zona
void Aprendizaje::registrarResultado(int zona, bool acierto) {
    validarZona(zona);
    intentos[zona] += 1.0;
    if (acierto) {
        aciertos[zona] += 1.0;
    }
}

double Aprendizaje::getFrecuencia(int zona) const {
    validarZona(zona);
    return visitas[zona];
}

double Aprendizaje::getTasaAcierto(int zona) const {
    validarZona(zona);
    if (intentos[zona] <= 0.0) return 0.0;   // aun no hay intentos
    return aciertos[zona] / intentos[zona];
}

int Aprendizaje::getNumZonas() const {
    return numZonas;
}

// Los datos viejos pierden peso; la tasa de acierto no cambia
void Aprendizaje::aplicarOlvido() {
    for (int i = 0; i < numZonas; ++i) {
        visitas[i] *= factorOlvido;
        intentos[i] *= factorOlvido;
        aciertos[i] *= factorOlvido;
    }
}

// Borra todo lo aprendido
void Aprendizaje::reiniciar() {
    for (int i = 0; i < numZonas; ++i) {
        visitas[i] = 0.0;
        intentos[i] = 0.0;
        aciertos[i] = 0.0;
    }
}
