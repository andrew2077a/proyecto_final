#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <vector>
#include "entidades/entidad.h"
#include "fisicas/ondaEmpuje.h"

// Trayectoria o comportamiento del proyectil
enum class TipoProyectil { RECTO, PARABOLICO, ONDULATORIO, ESTALLIDO, BOMBA };

// Quien lo disparo (define a quien daña)
enum class Propietario { REI, ENEMIGO };


class Proyectil : public Entidad
{
private:
    TipoProyectil tipo;
    Propietario propietario;
    int danio;

    // Datos del agente inteligente
    int zonaObjetivo;    // zona a la que apuntaba (-1 si no es del agente)
    bool impacto;        // dio en Rei
    bool reportado;      // el resultado ya se le informo al agente

    // Estallido (bomba de puas del monstruo)
    double explotaEnX;   // posicion x donde se fragmenta
    int numPuas;

    // Movimiento sin fisica (recto y estallido): velocidad constante
    double vx;
    double vy;

    // Bomba del nivel 2
    double ySuelo;
    double radioDanio;       // dentro de este radio hace daño directo
    double radioEmpuje;      // dentro de este radio empuja a Rei
    double fuerzaEmpuje;
    double amortiguamiento;
    bool explotada;
    bool animandoExplosion;
    double tiempoExplosion;  // tiempo transcurrido de la animacion

    void aplicarAspecto();

public:
    Proyectil(double x, double y, TipoProyectil tipo, Propietario prop, int danio);

    void actualizar(double dt = 0) override;
    void setVelocidad(double vx, double vy);
    bool fueraDePantalla(double xMin, double xMax, double yMin, double yMax) const;
    void alImpactar(Entidad& objetivo);
    void alTocarRei(Rei& rei, int danio) override;

    // Estallido: se fragmenta en puas al llegar a explotaEnX
    void configurarEstallido(double xExplosion, int numPuas);
    bool debeFragmentar() const;
    std::vector<Proyectil*> fragmentar();   // el llamador es dueño de las puass

    // Bomba del nivel 2
    void configurarBomba(double ySuelo, double radioDanio, double radioEmpuje, double F, double k);
    void explotar();
    bool haExplotado() const;
    void iniciarExplosion();
    bool estaExplotando() const;
    bool explosionTerminada() const;
    EstadoVisual getEstadoVisual() const override;

    bool esImpactoDirecto(const Rei& rei) const;
    OndaEmpuje* calcularEmpuje(const Rei& rei) const;   // nullptr si Rei esta fuera del radio

    // Seguimiento para el agente inteligente
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
