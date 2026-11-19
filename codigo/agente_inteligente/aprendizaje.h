#ifndef APRENDIZAJE_H
#define APRENDIZAJE_H

// Tarea 4 del agente: la memoria. Cuenta cuantas veces estuvo Rei en cada zona
// y si los disparos a cada zona acertaron.
class Aprendizaje
{
private:
    int numZonas;
    double* visitas;
    double* intentos;
    double* aciertos;
    double factorOlvido;

    void validarZona(int zona) const;
public:
    Aprendizaje(int numZonas, double factorOlvido);
    ~Aprendizaje();


    Aprendizaje(const Aprendizaje&) = delete;
    Aprendizaje& operator=(const Aprendizaje&) = delete;

    void registrar(int zona);
    void registrarResultado(int zona, bool acierto);
    double getFrecuencia(int zona) const;
    double getTasaAcierto(int zona) const;
    int getNumZonas() const;
    void aplicarOlvido();
    void reiniciar();
};

#endif
