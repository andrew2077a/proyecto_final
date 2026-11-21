#include "nivel1.h"
#include <stdexcept>

Nivel1::Nivel1(ModoDificultad modo):Nivel(modo){
    temporizadorCamiones=0.0;
    temporizadorBateria=0.0;
    cargarParametros();
}

Nivel1::~Nivel1(){
    delete moustro;
}

void Nivel1::cargarParametros() {
    if (dificultad == ModoDificultad::NORMAL) {
        params.vidaMonstruo = Nivel1Normal::VIDA_MONSTRUO;
        params.umbralFase1 = Nivel1Normal::UMBRAL_FASE1;
        params.umbralFase2 = Nivel1Normal::UMBRAL_FASE2;
        params.intervaloParabolico = Nivel1Normal::INTERVALO_PARABOLICO;
        params.intervaloOndulatorio = Nivel1Normal::INTERVALO_ONDULATORIO;
        params.tiempoCarga = Nivel1Normal::TIEMPO_CARGA;
        params.ataquesCombinadosMin = Nivel1Normal::ATAQUES_COMBINADOS_MIN;
        params.ataquesCombinadosMax = Nivel1Normal::ATAQUES_COMBINADOS_MAX;
        params.ataquesDobles = Nivel1Normal::ATAQUES_DOBLES;
        params.escudoMonstruo = Nivel1Normal::ESCUDO_MONSTRUO;
        params.umbralesEscudo[0] = Nivel1Normal::UMBRAL_ESCUDO_1;
        params.umbralesEscudo[1] = Nivel1Normal::UMBRAL_ESCUDO_2;
        params.umbralesEscudo[2] = Nivel1Normal::UMBRAL_ESCUDO_3;
        params.duracionEscudo = Nivel1Normal::DURACION_ESCUDO;
        params.estallidoActivo = Nivel1Normal::ESTALLIDO_ACTIVO;
        params.puasMin = Nivel1Normal::PUAS_MIN;
        params.puasMax = Nivel1Normal::PUAS_MAX;
        params.intervaloEstallido = Nivel1Normal::INTERVALO_ESTALLIDO;
        params.numZonas = Nivel1Normal::NUM_ZONAS;
        params.factorOlvido = Nivel1Normal::FACTOR_OLVIDO;
        params.suavizadoMin = Nivel1Normal::SUAVIZADO_MIN;
        params.intervaloCamionInicial = Nivel1Normal::INTERVALO_CAMION_INICIAL;
        params.velocidadCamionInicial = Nivel1Normal::VELOCIDAD_CAMION_INICIAL;
        params.rampaCamiones = Nivel1Normal::RAMPA_CAMIONES;
        params.calorMax = Nivel1Normal::CALOR_MAX;
        params.calorPorDisparo = Nivel1Normal::CALOR_POR_DISPARO;
        params.tiempoRecargaCalor = Nivel1Normal::TIEMPO_RECARGA_CALOR;
        params.tiempoBloqueoGolpe = Nivel1Normal::TIEMPO_BLOQUEO_GOLPE;
        params.cadenciaBase = Nivel1Normal::CADENCIA_BASE;
        params.duracionBateria = Nivel1Normal::DURACION_BATERIA;
        params.multCadenciaBateria = Nivel1Normal::MULT_CADENCIA_BATERIA;
        params.multVelocidadBateria = Nivel1Normal::MULT_VELOCIDAD_BATERIA;
        params.intervaloBateria = Nivel1Normal::INTERVALO_BATERIA;
    } else {
        params.vidaMonstruo = Nivel1Dificil::VIDA_MONSTRUO;
        params.umbralFase1 = Nivel1Dificil::UMBRAL_FASE1;
        params.umbralFase2 = Nivel1Dificil::UMBRAL_FASE2;
        params.intervaloParabolico = Nivel1Dificil::INTERVALO_PARABOLICO;
        params.intervaloOndulatorio = Nivel1Dificil::INTERVALO_ONDULATORIO;
        params.tiempoCarga = Nivel1Dificil::TIEMPO_CARGA;
        params.ataquesCombinadosMin = Nivel1Dificil::ATAQUES_COMBINADOS_MIN;
        params.ataquesCombinadosMax = Nivel1Dificil::ATAQUES_COMBINADOS_MAX;
        params.ataquesDobles = Nivel1Dificil::ATAQUES_DOBLES;
        params.escudoMonstruo = Nivel1Dificil::ESCUDO_MONSTRUO;
        params.umbralesEscudo[0] = Nivel1Dificil::UMBRAL_ESCUDO_1;
        params.umbralesEscudo[1] = Nivel1Dificil::UMBRAL_ESCUDO_2;
        params.umbralesEscudo[2] = Nivel1Dificil::UMBRAL_ESCUDO_3;
        params.duracionEscudo = Nivel1Dificil::DURACION_ESCUDO;
        params.estallidoActivo = Nivel1Dificil::ESTALLIDO_ACTIVO;
        params.puasMin = Nivel1Dificil::PUAS_MIN;
        params.puasMax = Nivel1Dificil::PUAS_MAX;
        params.intervaloEstallido = Nivel1Dificil::INTERVALO_ESTALLIDO;
        params.numZonas = Nivel1Dificil::NUM_ZONAS;
        params.factorOlvido = Nivel1Dificil::FACTOR_OLVIDO;
        params.suavizadoMin = Nivel1Dificil::SUAVIZADO_MIN;
        params.intervaloCamionInicial = Nivel1Dificil::INTERVALO_CAMION_INICIAL;
        params.velocidadCamionInicial = Nivel1Dificil::VELOCIDAD_CAMION_INICIAL;
        params.rampaCamiones = Nivel1Dificil::RAMPA_CAMIONES;
        params.calorMax = Nivel1Dificil::CALOR_MAX;
        params.calorPorDisparo = Nivel1Dificil::CALOR_POR_DISPARO;
        params.tiempoRecargaCalor = Nivel1Dificil::TIEMPO_RECARGA_CALOR;
        params.tiempoBloqueoGolpe = Nivel1Dificil::TIEMPO_BLOQUEO_GOLPE;
        params.cadenciaBase = Nivel1Dificil::CADENCIA_BASE;
        params.duracionBateria = Nivel1Dificil::DURACION_BATERIA;
        params.multCadenciaBateria = Nivel1Dificil::MULT_CADENCIA_BATERIA;
        params.multVelocidadBateria = Nivel1Dificil::MULT_VELOCIDAD_BATERIA;
        params.intervaloBateria = Nivel1Dificil::INTERVALO_BATERIA;
    }

    if (params.vidaMonstruo <= 0 || params.numZonas <= 0 ||
        params.ataquesCombinadosMin > params.ataquesCombinadosMax ||
        params.puasMin > params.puasMax ||
        params.umbralFase2 >= params.umbralFase1 ||
        params.factorOlvido <= 0.0 || params.factorOlvido > 1.0) {
        throw std::invalid_argument("Parametros de Nivel1 incoherentes");
    }
}





void Nivel1::iniciar(){
    rei= new Rei(ModoRei::MECHA,comunes,params,Nivel1Init::REI_INICIO_X,Nivel1Init::Y_SUELO);
    moustro= new Moustro(Nivel1Init::MOUSTRO_INICIO_X,Nivel1Init::MOUSTRO_INICIO_Y,params,comunes.gravedad);
}

void Nivel2::actualizar(double dt){

}


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
    Moustro* moustro;
    ParamsNivel1 params;
    double temporizadorCamiones;
    double intervaloCamiones;
    double velocidadCamiones;
    double temporizadorBateria;
    vector<Proyectil*> proyectiles;

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
    void actualizar(double dt) override;
    ResultadoNivel verificarFin() override;
    vector<Entidad*> obtenerEntidadesDibujables() override;
    double getVidaJefeRelativa() override;
};
