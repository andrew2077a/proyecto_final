#ifndef RAZONAMIENTO_H
#define RAZONAMIENTO_H

class Aprendizaje;

// Tarea 3 del agente: decide a que zona atacar segun lo aprendido.
class Razonamiento
{
private:
    int numZonas;
    double* probabilidades;   // probabilidad de atacar cada zona (suman 1)
    double suavizadoMin;      // probabilidad minima de cualquier zona

    void normalizar();

public:
    Razonamiento(int numZonas, double suavizadoMin);
    ~Razonamiento();

    void calcularProbabilidades(const Aprendizaje& apr);
    void suavizar();
    int decidir() const;
};

#endif // RAZONAMIENTO_H
