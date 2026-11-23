#include "Monstruo.h"
#include "agente_inteligente/percepcion.h"
#include "agente_inteligente/razonamiento.h"
#include "agente_inteligente/accion.h"
#include "agente_inteligente/aprendizaje.h"
#include "entidades/proyectil.h"
#include "entidades/rei.h"
#include "fisicas/movOndulatorio.h"
#include "datos.h"
#include <cmath>
#include <random>
#include <stdexcept>

namespace {
const double PI = std::acos(-1.0);


const double EPS = 1e-6;

// Valores del ataque ondulatorio y del estallido
const double VX_ONDULATORIO = 500.0;     // px/s hacia la izquierda
const double AMPLITUD_ONDULATORIO = 120.0;
const double FRECUENCIA_ONDULATORIO = 2.0 * PI / 1.5;   // una oscilacion cada 1,5 s
const double VX_ESTALLIDO = 350.0;       // px/s hacia la izquierda
const double X_EXPLOSION = Tamanio::ANCHO_PANTALLA / 2.0;   // a mitad de pantalla

std::mt19937& generador() {
    static std::mt19937 gen{std::random_device{}()};
    return gen;
}

// Entero aleatorio en [a, b]
int aleatorio(int a, int b) {
    if (b <= a) return a;
    return std::uniform_int_distribution<int>(a, b)(generador());
}

// Ya se cumplio el intervalo (con margen por los decimales acumulados)
bool cumplido(double transcurrido, double intervalo) {
    return transcurrido + EPS >= intervalo;
}
}

Monstruo::Monstruo(double x, double y, const ParamsNivel1& p1, double gravedad)
    : Entidad(x, y, Tamanio::MONSTRUO_ANCHO, Tamanio::MONSTRUO_ALTO, TipoSprite::MONSTRUO),
    percepcion(nullptr), razonamiento(nullptr), accion(nullptr), aprendizaje(nullptr),
    ultimaZona(-1), vida(p1.vidaMonstruo), params(&p1), fase(1), vidaMax(p1.vidaMonstruo),
    temporizadorAtaque(0.0), temporizadorOndulatorio(0.0), temporizadorEstallido(0.0),
    cargando(false), tiempoCarga(0.0), inicioCargaPendiente(false),
    ataquesCombinadosPendientes(0), escudoActivo(false), tiempoEscudo(0.0), escudosUsados(0)
{
    if (p1.vidaMonstruo <= 0) {
        throw std::invalid_argument("Monstruo: la vida debe ser mayor que 0");
    }
    // Si algun new o constructor falla, se libera lo ya creado antes de relanzar
    try {
        // Las zonas cubren el recorrido de Rei: desde el borde izquierdo hasta el monstruo
        percepcion = new Percepcion(p1.numZonas, 0.0, x);
        aprendizaje = new Aprendizaje(p1.numZonas, p1.factorOlvido);
        razonamiento = new Razonamiento(p1.numZonas, p1.suavizadoMin);
        accion = new Accion(p1, gravedad);
    } catch (...) {
        liberarAgente();
        throw;
    }
    ataquesCombinadosPendientes = aleatorio(p1.ataquesCombinadosMin, p1.ataquesCombinadosMax);
}

Monstruo::~Monstruo() {
    liberarAgente();
}

void Monstruo::liberarAgente() {
    delete percepcion;    percepcion = nullptr;
    delete razonamiento;  razonamiento = nullptr;
    delete accion;        accion = nullptr;
    delete aprendizaje;   aprendizaje = nullptr;
}

// Ciclo del monstruo

void Monstruo::actualizar(double dt) {
    if (!estaViva()) return;

    temporizadorAtaque += dt;
    temporizadorOndulatorio += dt;
    if (params->estallidoActivo) temporizadorEstallido += dt;


    if (cargando) {
        tiempoCarga -= dt;
        if (tiempoCarga <= 0.0) {
            tiempoCarga = 0.0;
            cargando = false;
        }
    } else {
        const double previo = params->tiempoCarga;
        const bool llega = cumplido(temporizadorAtaque, params->intervaloParabolico - previo)
                           || cumplido(temporizadorOndulatorio, params->intervaloOndulatorio - previo)
                           || (params->estallidoActivo
                               && cumplido(temporizadorEstallido, params->intervaloEstallido - previo));
        if (llega) {
            cargando = true;
            tiempoCarga = previo;
            inicioCargaPendiente = true;
        }
    }

    if (escudoActivo) {
        tiempoEscudo -= dt;
        if (tiempoEscudo <= 0.0) {
            tiempoEscudo = 0.0;
            escudoActivo = false;
        }
    }

    actualizarFase();
}


bool Monstruo::consumirInicioCarga() {
    if (inicioCargaPendiente) {
        inicioCargaPendiente = false;
        return true;
    }
    return false;
}

std::vector<Proyectil*> Monstruo::atacar(const Rei& rei) {
    std::vector<Proyectil*> lanzados;
    if (!estaViva()) return lanzados;


    if (cumplido(temporizadorAtaque, params->intervaloParabolico)) {
        const int cantidad = fase + (params->ataquesDobles ? 1 : 0);
        for (int i = 0; i < cantidad; ++i) {
            lanzados.push_back(ejecutarAgente(rei));
        }

        if (ataquesCombinadosPendientes > 0) {
            lanzarOndulatorio(lanzados);
            --ataquesCombinadosPendientes;
        }
        temporizadorAtaque = 0.0;
    }


    if (cumplido(temporizadorOndulatorio, params->intervaloOndulatorio)) {
        lanzarOndulatorio(lanzados);
        temporizadorOndulatorio = 0.0;
    }


    if (params->estallidoActivo && cumplido(temporizadorEstallido, params->intervaloEstallido)) {
        lanzarEstallido(lanzados);
        temporizadorEstallido = 0.0;
    }
    return lanzados;
}

// Las cuatro tareas del agente
// Orden fijo: percibir -> aprender -> razonar -> actuar.
Proyectil* Monstruo::ejecutarAgente(const Rei& rei) {
    const int zona = percibir(rei);
    aprender(zona);
    const int decision = razonar();
    return actuar(decision);
}

int Monstruo::percibir(const Rei& rei) {
    ultimaZona = percepcion->percibir(rei);
    return ultimaZona;
}

// Primero el olvido (lo viejo pesa menos) y luego la nueva visita.
void Monstruo::aprender(int zona) {
    aprendizaje->aplicarOlvido();
    aprendizaje->registrar(zona);   // lanza  si la zona no existe
}

int Monstruo::razonar() {
    razonamiento->calcularProbabilidades(*aprendizaje);
    razonamiento->suavizar();       // ninguna zona queda con probabilidad cero
    return razonamiento->decidir();
}

Proyectil* Monstruo::actuar(int zona) {
    const double centroX = percepcion->centroDeZona(zona);
    // Los proyectiles salen por el costado izquierdo, a un tercio de la altura
    return accion->ejecutar(zona, centroX, x, y + alto / 3.0);
}



void Monstruo::lanzarOndulatorio(std::vector<Proyectil*>& salida) {
    const int cantidad = params->ataquesDobles ? 2 : 1;
    const double origenX = x;
    const double origenY = y + alto / 3.0;
    for (int i = 0; i < cantidad; ++i) {
        Proyectil* p = new Proyectil(origenX, origenY, TipoProyectil::ONDULATORIO,
                                     Propietario::ENEMIGO, params->danioDisparo);
        MovOndulatorio* onda = new MovOndulatorio(origenX, origenY);
        // El segundo (ataque doble) va en contrafase: las dos ondas se cruzan
        onda->setParametros(-VX_ONDULATORIO, AMPLITUD_ONDULATORIO,
                            FRECUENCIA_ONDULATORIO, (i == 0) ? 0.0 : PI);
        p->setFisica(onda);
        salida.push_back(p);
    }
}

void Monstruo::lanzarEstallido(std::vector<Proyectil*>& salida) {
    Proyectil* p = new Proyectil(x, y + alto / 3.0, TipoProyectil::ESTALLIDO,
                                 Propietario::ENEMIGO, params->danioDisparo);
    p->setVelocidad(-VX_ESTALLIDO, 0.0);
    p->configurarEstallido(X_EXPLOSION, aleatorio(params->puasMin, params->puasMax));
    salida.push_back(p);
}



// Sube la fase al cruzar los umbrales de vida (66 % y 33 %). Nunca baja.
void Monstruo::actualizarFase() {
    const double relativa = getVidaRelativa();
    int nueva = 1;
    if (relativa <= params->umbralFase1) nueva = 2;
    if (relativa <= params->umbralFase2) nueva = 3;

    if (nueva > fase) {
        fase = nueva;
        // Cada fase trae su propia tanda de ataques combinados
        ataquesCombinadosPendientes = aleatorio(params->ataquesCombinadosMin,
                                                params->ataquesCombinadosMax);
    }
}

void Monstruo::activarEscudo(double duracion) {
    escudoActivo = true;
    tiempoEscudo = duracion;
}

bool Monstruo::recibirDanio(int n) {
    if (!estaViva() || escudoActivo || n <= 0) return false;

    vida -= n;
    if (vida <= 0) {
        vida = 0;
        matar();
        return true;
    }


    if (params->escudoMonstruo) {
        const double relativa = getVidaRelativa();
        bool cruzo = false;
        while (escudosUsados < 3 && relativa <= params->umbralesEscudo[escudosUsados]) {
            ++escudosUsados;
            cruzo = true;
        }
        if (cruzo) activarEscudo(params->duracionEscudo);
    }
    return true;
}

bool Monstruo::estaViva() const {
    return Entidad::estaViva() && vida > 0;
}

// Aprendizaje y consultas

void Monstruo::notificarResultado(int zona, bool acierto) {
    aprendizaje->registrarResultado(zona, acierto);
}

// Cada partida empieza de cero.
void Monstruo::reiniciarAgente() {
    aprendizaje->reiniciar();
}

double Monstruo::getVidaRelativa() const {
    return static_cast<double>(vida) / vidaMax;
}

int Monstruo::getVida() const {
    return vida;
}

int Monstruo::getFase() const {
    return fase;
}

bool Monstruo::tieneEscudo() const {
    return escudoActivo;
}

EstadoVisual Monstruo::getEstadoVisual() const {
    if (vida <= 0) {
        return EstadoVisual::MURIENDO;
    }
    if (cargando) {
        return EstadoVisual::LANZANDO;
    }
    return EstadoVisual::QUIETO;
}
