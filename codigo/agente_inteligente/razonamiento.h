#ifndef RAZONAMIENTO_H
#define RAZONAMIENTO_H

class Aprendizaje;

// Tarea 2 del agente: decide a que zona atacar a partir de lo aprendido.
class Razonamiento
{
private:
    int numZonas;
    double* probabilidades;
    double suavizadoMin;

    void normalizar();

public:
    Razonamiento(int numZonas, double suavizadoMin);
    ~Razonamiento();


    Razonamiento(const Razonamiento&) = delete;
    Razonamiento& operator=(const Razonamiento&) = delete;

    void calcularProbabilidades(const Aprendizaje& apr);
    void suavizar();
    int decidir() const;
};

#endif // RAZONAMIENTO_H
