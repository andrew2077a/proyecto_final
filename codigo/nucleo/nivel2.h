#ifndef NIVEL2_H
#define NIVEL2_H
#include <vector>
#include "nucleo/nivel.h"
#include "entidades/proyectil.h"

// Parametros del nivel 2. Se llenan segun la dificultad en Nivel2::cargarParametros().
struct ParamsNivel2{
    double tiempoLimite;
    // Bombas que caen del cielo cerca de Rei
    double intervaloBomba;
    int numBombas;               // bombas por tanda
    double rangoCaidaBomba;      // dispersion horizontal alrededor de Rei
    double radioDanioBomba;      // dentro de este radio hace daño directo
    double radioEmpujeBomba;     // dentro de este radio empuja a Rei
    double fuerzaEmpuje;
    double amortiguamientoEmpuje;
    // Balas que entran desde la izquierda
    double intervaloBalaLateral;
    int numBalasLaterales;
    double velocidadBala;
    // Mundo
    double velocidadCamionPatrulla;
    double duracionGaseosa;
    double multVelocidadGaseosa;
    double esperaPuas;           // tiempo entre daños consecutivos de las puas
};

// Nivel 2: Rei (soldado) recorre un mundo con scroll horizontal, contra reloj,
// esquivando obstaculos, camiones, bombas y balas hasta llegar a la meta.
class Nivel2: public Nivel{
private:
    double camaraX;        // desplazamiento horizontal de la camara
    double anchoMundo;
    double ySuelo;
    vector<Proyectil*> bombas;   // bombas vivas; los objetos pertenecen a "entidades"
    ParamsNivel2 params;
    double temporizadorBomba;
    double temporizadorBala;
    void cargarParametros();
    void construirMundo();
    void actualizarScroll();
    void lanzarBalaLateral();
    void soltarBomba();
    void procesarExplosiones();
    bool superficieBajo(const Rei& rei, double& y) const;

protected:
    void colocarItems() override;
    void eliminarInactivos() override;

public:
    Nivel2(ModoDificultad modo);
    void iniciar() override;
    void actualizar(double dt) override;
    ResultadoNivel verificarFin() override;
    double getCamaraX() override;
};

#endif // NIVEL2_H
