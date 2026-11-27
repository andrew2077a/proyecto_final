#include "nivel1.h"
#include "entidades/Monstruo.h"
#include "datos.h"
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Nivel1::Nivel1(ModoDificultad modo)
    : Nivel(modo), monstruo(nullptr), temporizadorCamiones(0.0),
    intervaloCamiones(0.0), velocidadCamiones(0.0), temporizadorBateria(0.0)
{
    srand(time(0));
    cargarParametros();
    intervaloCamiones = params.intervaloCamionInicial;
    velocidadCamiones = params.velocidadCamionInicial;
}
Nivel1::~Nivel1(){
    delete monstruo;
}

// Copia las constantes de Nivel1Normal o Nivel1Dificil (datos.h) y las valida
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
        params.danioDisparo = Nivel1Normal::DANIO_DISPARO;
        params.anguloDisparo = Nivel1Normal::ANGULO_DISPARO;
        params.velocidadMaxDisparo = Nivel1Normal::VELOCIDAD_MAX_DISPARO;
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
        params.danioDisparo = Nivel1Dificil::DANIO_DISPARO;
        params.anguloDisparo = Nivel1Dificil::ANGULO_DISPARO;
        params.velocidadMaxDisparo = Nivel1Dificil::VELOCIDAD_MAX_DISPARO;
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

    // Rangos invertidos o valores que provocarian divisiones por cero
    if (params.vidaMonstruo <= 0 || params.numZonas <= 0 ||
        params.ataquesCombinadosMin > params.ataquesCombinadosMax ||
        params.puasMin > params.puasMax ||
        params.umbralFase2 >= params.umbralFase1 ||
        params.factorOlvido <= 0.0 || params.factorOlvido > 1.0) {
        throw std::invalid_argument("Parametros de Nivel1 incoherentes");
    }
}
void Nivel1::iniciar(){
    rei = new Rei(ModoRei::MECHA, comunes, &params, Nivel1Init::REI_INICIO_X, Nivel1Init::REI_INICIO_Y);
    monstruo = new Monstruo(Nivel1Init::MONSTRUO_INICIO_X, Nivel1Init::MONSTRUO_INICIO_Y,
                            params, comunes.gravedad);
}

void Nivel1::actualizar(double dt){
    monstruo->actualizar(dt);
    rei->actualizar(dt);

    // Suelo y limites horizontales de Rei
    if(rei->getY() + rei->getAlto() >= Nivel1Init::Y_SUELO){
        rei->apoyarEn(Nivel1Init::Y_SUELO);
    }
    if(rei->getX() < Nivel1Init::LIMITE_REI_IZQ){
        rei->setPosicion(Nivel1Init::LIMITE_REI_IZQ, rei->getY());
    }
    else if(rei->getX() > Nivel1Init::LIMITE_REI_DER){
        rei->setPosicion(Nivel1Init::LIMITE_REI_DER, rei->getY());
    }

    actualizarTemporizador(dt);

    // El tiempo para la siguiente bateria solo corre si no hay una en pantalla
    if(items.empty()){
        temporizadorBateria += dt;
    }

    actualizarGeneracionCamiones(dt);
    for(auto i:entidades){
        i->actualizar(dt);
    }
    detectarColisiones();
    procesarDisparoMonstruo();
    procesarDisparoRei();
    procesarImpactosAlMonstruo();
    colocarItems();
    fragmentarEstallidos();
    notificarAgente();
    eliminarInactivos();
}

ResultadoNivel Nivel1:: verificarFin(){
    if(!rei->estaViva()){
        return ResultadoNivel::PERDIDO;
    }
    if (!monstruo->estaViva()){
        return ResultadoNivel::GANADO;
    }
    return ResultadoNivel::EN_CURSO;
}

// Una bateria a la vez, en una posicion horizontal aleatoria dentro del recorrido de Rei
void Nivel1::colocarItems(){
    if(!items.empty()){
        return;
    }

    if(temporizadorBateria >= params.intervaloBateria){
        temporizadorBateria = 0.0;

        const int rango =static_cast<int>(Nivel1Init::BATERIA_X_MAX) -static_cast<int>(Nivel1Init::BATERIA_X_MIN) + 1;

        const double ale =static_cast<double>(rand() % rango) + Nivel1Init::BATERIA_X_MIN;

        Item *i = new Item(TipoItem::BATERIA,ale,Nivel1Init::BATERIA_Y,Tamanio::BATERIA_ANCHO,Tamanio::BATERIA_ALTO,
            params.duracionBateria,params.multVelocidadBateria,params.multCadenciaBateria);
        agregarItem(i);
    }
}

// Los proyectiles se agregan a "entidades" (dueño) y a "proyectiles" (acceso rapido)
void Nivel1::procesarDisparoMonstruo(){
    if(monstruo->consumirInicioCarga()){
        RegistrarEvento(TipoEvento::MONSTRUO_LANZA);
    }
    vector<Proyectil*> nuevos =monstruo->atacar(*rei);
    for(Proyectil* p: nuevos){
        agregarEntidad(p);
        proyectiles.push_back(p);
    }
}

void Nivel1::procesarDisparoRei(){
    if(rei->estaDisparando()&&rei->puedeDisparar()){
        Proyectil* bala = rei->disparar();
        if(bala != nullptr){
            RegistrarEvento(TipoEvento::DISPARO_REI);
            agregarEntidad(bala);
            proyectiles.push_back(bala);
        }
    }
}

// Las balas de Rei no pasan por detectarColisiones (esa funcion solo comprueba contra Rei)
void Nivel1::procesarImpactosAlMonstruo(){
    for(Proyectil* p:proyectiles){
        if(p->getPropietario()==Propietario::REI){
            const double margen = (monstruo->getAncho() - 280.0) / 2.0;
            const double izquierda = monstruo->getX() + margen;
            const double derecha = monstruo->getX() + monstruo->getAncho() - margen;

            if (p->colisionaCon(*monstruo) &&
                p->getX() + p->getAncho() > izquierda &&
                p->getX() < derecha) {
                p->alImpactar(*monstruo);
                p->desactivar();
            }
        }
    }
}

// Cuando un estallido llega a su punto, se reemplaza por sus puas.
// Se recorre por indice porque las puas nuevas se agregan al mismo vector durante el bucle.
void Nivel1::fragmentarEstallidos(){
    for(size_t k = 0; k < proyectiles.size(); ++k){
        Proyectil* p = proyectiles[k];
        if(!p->estaViva() || !p->debeFragmentar()){
            continue;
        }
        vector<Proyectil*> puas = p->fragmentar();
        for(Proyectil* pua : puas){
            agregarEntidad(pua);
            proyectiles.push_back(pua);
        }
        RegistrarEvento(TipoEvento::EXPLOSION);
    }
}

// Aprendizaje del agente: cuando un disparo apuntado termina (impacta o cae
// al suelo o sale de la pantalla) se le informa una sola vez si acerto.
void Nivel1::notificarAgente(){
    for (Proyectil* p : proyectiles){
        if (p->fueReportado() || p->getZonaObjetivo() < 0){   // zona -1 = no es un disparo del agente
            continue;
        }
        bool impacto = p->haImpactado();
        bool fuera = p->getX() + p->getAncho() < 0 || p->getY() + p->getAlto() >= Nivel1Init::Y_SUELO;
        if (!impacto && !fuera){
            continue;
        }
        monstruo->notificarResultado(p->getZonaObjetivo(), impacto);
        p->marcarReportado();
    }
}

// Cada camion sale un poco antes y mas rapido que el anterior (el intervalo nunca baja de 0.5 s)
void Nivel1::actualizarGeneracionCamiones(double dt){
    temporizadorCamiones += dt;
    if(temporizadorCamiones >= intervaloCamiones){
        temporizadorCamiones -= intervaloCamiones;
        CamionNorcoreano* camion = new CamionNorcoreano(Nivel1Init::CAMION_INICIO_X,Nivel1Init::CAMION_INICIO_Y,ModoCamion::AVANCE,velocidadCamiones);
        agregarEntidad(camion);
        intervaloCamiones = std::max(0.5, intervaloCamiones * (1.0 - params.rampaCamiones));
        velocidadCamiones *= (1.0 + params.rampaCamiones);
    }
}

// Primero se descartan los proyectiles que cayeron al suelo o se alejaron y se quitan
// de "proyectiles"; despues Nivel::eliminarInactivos los borra de "entidades".
// Ese orden evita dejar punteros colgando en "proyectiles".
void Nivel1:: eliminarInactivos(){
    for(Proyectil* p : proyectiles){
        if(p->getTipo() == TipoProyectil::PARABOLICO && p->getY() + p->getAlto() >= Nivel1Init::Y_SUELO){
            p->desactivar();
        }
        if(p->fueraDePantalla(-500, Tamanio::ANCHO_PANTALLA + 500,
                               -1000, Tamanio::ALTO_PANTALLA + 500)){
            p->desactivar();
        }
    }
    for(size_t i=0;i<proyectiles.size();){
        if(!proyectiles[i]->estaViva()){
            proyectiles.erase(proyectiles.begin()+i);
        }
        else{
            i++;
        }
    }
    Nivel::eliminarInactivos();
}

// El monstruo no esta en "entidades", asi que se agrega aparte
vector<Entidad*> Nivel1::obtenerEntidadesDibujables(){
    vector<Entidad*> dibujas;
    dibujas.reserve(entidades.size() + obstaculos.size() + items.size() + 2);
    if (rei != nullptr) {
        dibujas.push_back(rei);
    }
    if(monstruo != nullptr){
        dibujas.push_back(monstruo);
    }
    dibujas.insert(dibujas.end(), entidades.begin(), entidades.end());
    dibujas.insert(dibujas.end(), obstaculos.begin(), obstaculos.end());
    dibujas.insert(dibujas.end(), items.begin(), items.end());
    return dibujas;
}

double Nivel1::getVidaJefeRelativa(){
    if(monstruo == nullptr){
        return 0.0;
    }
    return monstruo->getVidaRelativa();
}
