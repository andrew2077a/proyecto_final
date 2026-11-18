#ifndef NIVEL2_H
#define NIVEL2_H
#include <vector>
#include "nucleo/nivel.h"
#include "entidades/proyectil.h"

struct ParamsNivel2{
    double tiempoLimite;
    double intervaloBomba;
    int numBombas;
    double rangoCaidaBomba;
    double radioDanioBomba;
    double radioEmpujeBomba;
    double fuerzaEmpuje;
    double amortiguamientoEmpuje;
    double intervaloBalaLateral;
    int numBalasLaterales;
    double velocidadBala;
    double velocidadCamionPatrulla;
    double duracionGaseosa;
    double multVelocidadGaseosa;
    double esperaPuas;
};

class Nivel2: public Nivel{
private:
    double camaraX;
    double anchoMundo;
    double ySuelo;
    vector<Proyectil*> bombas;
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
