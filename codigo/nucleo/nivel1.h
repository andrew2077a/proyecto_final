#ifndef NIVEL1_H
#define NIVEL1_H
#include "nucleo/nivel.h"
#include "entidades/proyectil.h"

class Monstruo;

// Parametros del nivel 1. Se llenan segun la dificultad en Nivel1::cargarParametros().
// Los tiempos estan en segundos.
struct ParamsNivel1{
    // Monstruo
    int vidaMonstruo;
    double umbralFase1 ;          // vida relativa a la que pasa a la fase 2
    double umbralFase2 ;          // y a la fase 3
    double intervaloParabolico ;
    double intervaloOndulatorio ;
    double tiempoCarga ;          // animacion previa al ataque
    int ataquesCombinadosMin ;
    int ataquesCombinadosMax ;
    bool ataquesDobles = false;
    bool escudoMonstruo = false;
    double umbralesEscudo[3] = {0,0,0};   // vida relativa a la que se activa cada escudo
    double duracionEscudo ;
    bool estallidoActivo = false;
    int puasMin ;
    int puasMax ;
    double intervaloEstallido ;

    // Disparo apuntado del agente
    int danioDisparo ;
    double anguloDisparo ;
    double velocidadMaxDisparo ;

    // Agente inteligente
    int numZonas ;
    double factorOlvido ;
    double suavizadoMin ;

    // Camiones que cruzan la pantalla: cada vez salen mas seguido y mas rapido
    double intervaloCamionInicial ;
    double velocidadCamionInicial ;
    double rampaCamiones ;

    // Arma de Rei
    double calorMax ;
    double calorPorDisparo ;
    double tiempoRecargaCalor ;
    double tiempoBloqueoGolpe ;
    double cadenciaBase ;

    // Bateria
    double duracionBateria ;
    double multCadenciaBateria ;
    double multVelocidadBateria ;
    double intervaloBateria ;
};

// Nivel 1: Rei (mecha) contra el monstruo en una sola pantalla fija
class Nivel1: public Nivel{
private:
    Monstruo* monstruo;   // no esta en "entidades": Nivel1 es su dueño
    ParamsNivel1 params;
    double temporizadorCamiones;
    double intervaloCamiones;     // se acorta con cada camion
    double velocidadCamiones;     // aumenta con cada camion
    double temporizadorBateria;
    vector<Proyectil*> proyectiles;   // los mismos proyectiles de "entidades" (que es su dueño)

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
