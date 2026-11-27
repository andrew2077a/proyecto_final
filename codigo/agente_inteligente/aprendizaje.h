#ifndef APRENDIZAJE_H
#define APRENDIZAJE_H

// Tarea 2 del agente: la memoria (visitas y aciertos por zona).
class Aprendizaje
{
private:
    int numZonas;
    double* visitas;    // veces que se vio a Rei en cada zona
    double* intentos;   // disparos hechos a cada zona
    double* aciertos;   // disparos que dieron en cada zona
    double factorOlvido;   // entre 0 y 1: cuanto se conserva del pasado

    void validarZona(int zona) const;
public:
    Aprendizaje(int numZonas, double factorOlvido);
    ~Aprendizaje();

    void registrar(int zona);
    void registrarResultado(int zona, bool acierto);
    double getFrecuencia(int zona) const;
    double getTasaAcierto(int zona) const;
    int getNumZonas() const;
    void aplicarOlvido();
    void reiniciar();
};

#endif
