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

// Enemigo del nivel 1 y agente inteligente: coordina las cuatro tareas
// (percibir, aprender, razonar y actuar), cada una en su propia clase.
class Monstruo : public Entidad{
private:
    Percepcion* percepcion;
    Razonamiento* razonamiento;
    Accion* accion;
    Aprendizaje* aprendizaje;
    int ultimaZona;

    int vida;
    int vidaMax;
    const ParamsNivel1* params;  // pertenece a Nivel1; el monstruo solo lo lee
    int fase;                    // 1 a 3

    double temporizadorAtaque;   // ataque parabolico (el del agente)
    double temporizadorOndulatorio;
    double temporizadorEstallido;

    // Animacion de carga previa al ataque
    bool cargando;
    double tiempoCarga;
    bool inicioCargaPendiente;   // el nivel lo consulta para reproducir el sonido

    int ataquesCombinadosPendientes;   // ondulatorios que acompañan al parabolico

    bool escudoActivo;
    double tiempoEscudo;
    int escudosUsados;

    // Ciclo del agente: percibir -> aprender -> razonar -> actuar
    Proyectil* ejecutarAgente(const Rei& rei);
    int percibir(const Rei& rei);
    void aprender(int zona);
    int razonar();
    Proyectil* actuar(int zona);

    void lanzarOndulatorio(std::vector<Proyectil*>& salida);
    void lanzarEstallido(std::vector<Proyectil*>& salida);

    void actualizarFase();
    void activarEscudo(double duracion);
    void liberarAgente();

public:
    Monstruo(double x, double y, const ParamsNivel1& p1, double gravedad);
    ~Monstruo() override;

    // Se prohibe copiar: copiar duplicaria los punteros a las cuatro partes
    // del agente y el destructor haria delete dos veces sobre la misma memoria.
    Monstruo(const Monstruo&) = delete;
    Monstruo& operator=(const Monstruo&) = delete;

    void actualizar(double dt = 0) override;
    bool consumirInicioCarga();
    std::vector<Proyectil*> atacar(const Rei& rei);   // quien lo llama se encarga de liberar los proyectiles

    bool recibirDanio(int n) override;
    bool estaViva() const override;

    void notificarResultado(int zona, bool acierto);   // informa al agente si su disparo acerto
    void reiniciarAgente();

    double getVidaRelativa() const;
    int getVida() const;
    int getFase() const;
    bool tieneEscudo() const;
    EstadoVisual getEstadoVisual() const override;
    bool miraDerecha() const override;
};

#endif // MONSTRUO_H
