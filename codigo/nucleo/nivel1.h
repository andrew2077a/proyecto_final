#ifndef NIVEL1_H
#define NIVEL1_H
#include "nucleo/nivel.h"
#include "entidades/proyectil.h"

class Monstruo;
struct ParamsNivel1{
    int vidaMonstruo;
    double umbralFase1 ;
    double umbralFase2 ;
    double intervaloParabolico ;
    double intervaloOndulatorio ;
    double tiempoCarga ;
    int ataquesCombinadosMin ;
    int ataquesCombinadosMax ;
    bool ataquesDobles = false;
    bool escudoMonstruo = false;
    double umbralesEscudo[3] = {0,0,0};
    double duracionEscudo ;
    bool estallidoActivo = false;
    int puasMin ;
    int puasMax ;
    double intervaloEstallido ;
    int danioDisparo ;
    double anguloDisparo ;
    double velocidadMaxDisparo ;
    int numZonas ;
    double factorOlvido ;
    double suavizadoMin ;
    double intervaloCamionInicial ;
    double velocidadCamionInicial ;
    double rampaCamiones ;
    double calorMax ;
    double calorPorDisparo ;
    double tiempoRecargaCalor ;
    double tiempoBloqueoGolpe ;
    double cadenciaBase ;
    double duracionBateria ;
    double multCadenciaBateria ;
    double multVelocidadBateria ;
    double intervaloBateria ;
};

class Nivel1: public Nivel{
private:
    Monstruo* monstruo;
    ParamsNivel1 params;
    double temporizadorCamiones;
    double intervaloCamiones;
    double velocidadCamiones;
    double temporizadorBateria;
    vector<Proyectil*> proyectiles;

    void cargarParametros();
    void procesarDisparoMonstruo();
    void procesarDisparoRei();
    void procesarImpactosAlMonstruo();
    void fragmentarEstallidos();
    void notificarAgente();
    void actualizarGeneracionCamiones(double dt);

protected:
    void colocarItems() override;
    void eliminarInactivos() override;

public:
    Nivel1(ModoDificultad modo);
    ~Nivel1() override;
    void iniciar() override;
    void actualizar(double dt) override;
    ResultadoNivel verificarFin() override;
    vector<Entidad*> obtenerEntidadesDibujables() override;
    double getVidaJefeRelativa() override;
};

#endif // NIVEL1_H