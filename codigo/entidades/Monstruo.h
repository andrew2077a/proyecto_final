#ifndef MONSTRUO_H
#define MONSTRUO_H

#include <vector>
#include "entidades/entidad.h"
#include "nucleo/nivel1.h"

class Percepcion;
class Razonamiento;
class Accion;
class Aprendizaje;
class Proyectil;
class Rei;

// Enemigo principal del nivel 1 y, a la vez, el agente inteligente: es el
// cuerpo que ejecuta lo que su cerebro decide. Coordina las cuatro tareas
// (percibir, aprender, razonar y actuar), cada una en su propia clase.
class Monstruo : public Entidad
{
private:
    Percepcion* percepcion;      // Tarea 1: percibir (Monstruo es su dueño)
    Razonamiento* razonamiento;  // Tarea 2: decidir
    Accion* accion;              // Tarea 3: disparar
    Aprendizaje* aprendizaje;    // Tarea 4: recordar y mejorar
    int ultimaZona;              // Ultima zona donde se percibio a Rei
    int vida;                    // Vida actual (Entidad no tiene vida)
    const ParamsNivel1* params;
    int fase;
    int vidaMax;
    double temporizadorAtaque;
    double temporizadorOndulatorio;
    double temporizadorEstallido;
    bool cargando;
    double tiempoCarga;
    bool inicioCargaPendiente;
    int ataquesCombinadosPendientes;
    bool escudoActivo;
    double tiempoEscudo;
    int escudosUsados;

    // Las cuatro tareas del agente y su ciclo
    Proyectil* ejecutarAgente(const Rei& rei);
    int percibir(const Rei& rei);
    void aprender(int zona);
    int razonar();
    Proyectil* actuar(int zona);

    // Ataques que no apunta el agente (agregan sus proyectiles a "salida")
    void lanzarOndulatorio(std::vector<Proyectil*>& salida);
    void lanzarEstallido(std::vector<Proyectil*>& salida);

    void actualizarFase();
    void activarEscudo(double duracion);
    void liberarAgente();

public:
    Monstruo(double x, double y, const ParamsNivel1& p1, double gravedad);
    ~Monstruo() override;

    // Las cuatro partes son propias: copiar el objeto las liberaria dos veces
    Monstruo(const Monstruo&) = delete;
    Monstruo& operator=(const Monstruo&) = delete;

    void actualizar(double dt = 0) override;
    bool consumirInicioCarga();
    std::vector<Proyectil*> atacar(const Rei& rei);

    bool recibirDanio(int n) override;
    bool estaViva() const override;

    void notificarResultado(int zona, bool acierto);
    void reiniciarAgente();

    double getVidaRelativa() const;
    int getVida() const;
    int getFase() const;
    bool tieneEscudo() const;
    EstadoVisual getEstadoVisual() const override;
};

#endif // MONSTRUO_H