#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <vector>
#include "entidades/entidad.h"
#include "fisicas/ondaEmpuje.h"

// Que trayectoria o comportamiento lleva el proyectil.
enum class TipoProyectil { RECTO, PARABOLICO, ONDULATORIO, ESTALLIDO, BOMBA };

// Quien lo disparo (define a quien daña).
enum class Propietario { REI, ENEMIGO };


class Proyectil : public Entidad
{
private:
    TipoProyectil tipo;
    Propietario propietario;
    int danio;
    int zonaObjetivo;
    double explotaEnX;
    int numPuas;
    bool impacto;
    bool reportado;
    double vx;
    double vy;
    double ySuelo;
    double radioDanio;
    double radioEmpuje;
    double fuerzaEmpuje;
    double amortiguamiento;
    bool explotada;
    bool animandoExplosion;
    double tiempoExplosion;

    void aplicarAspecto();

public:
    Proyectil(double x, double y, TipoProyectil tipo, Propietario prop, int danio);

    void actualizar(double dt = 0) override;
    void setVelocidad(double vx, double vy);
    bool fueraDePantalla(double xMin, double xMax, double yMin, double yMax) const;
    void alImpactar(Entidad& objetivo);
    void alTocarRei(Rei& rei, int danio) override;

    // Bomba de puas del monstruo
    void configurarEstallido(double xExplosion, int numPuas);
    bool debeFragmentar() const;
    std::vector<Proyectil*> fragmentar();

    // Bomba del nivel 2
    void configurarBomba(double ySuelo, double radioDanio, double radioEmpuje, double F, double k);
    void explotar();
    bool haExplotado() const;
    void iniciarExplosion();
    bool estaExplotando() const;
    bool explosionTerminada() const;
    EstadoVisual getEstadoVisual() const override;

    bool esImpactoDirecto(const Rei& rei) const;
    OndaEmpuje* calcularEmpuje(const Rei& rei) const;

    // Agente inteligente
    void setZonaObjetivo(int zona);
    int getZonaObjetivo() const;
    bool haImpactado() const;
    void marcarReportado();
    bool fueReportado() const;


    bool miraDerecha() const override;
    double getRotacion() const override;

    Propietario getPropietario() const;
    TipoProyectil getTipo() const;
};

#endif
