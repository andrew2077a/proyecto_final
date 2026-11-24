#include "proyectil.h"
#include "entidades/rei.h"
#include "datos.h"
#include <cmath>

namespace {
const double PI = std::acos(-1.0);
const double VELOCIDAD_PUA_DEFECTO = 300.0;   // si el estallido no traia velocidad

// Distancia entre los centros de dos entidades.
double distanciaCentros(const Entidad& a, const Entidad& b) {
    const double ax = a.getX() + a.getAncho() / 2.0;
    const double ay = a.getY() + a.getAlto() / 2.0;
    const double bx = b.getX() + b.getAncho() / 2.0;
    const double by = b.getY() + b.getAlto() / 2.0;
    return std::hypot(ax - bx, ay - by);
}
}

Proyectil::Proyectil(double x, double y, TipoProyectil tipo, Propietario prop, int danio)
    : Entidad(x, y, 0.0, 0.0, TipoSprite::PROYECTIL_REI),
    tipo(tipo), propietario(prop), danio(danio), zonaObjetivo(-1),
    explotaEnX(0.0), numPuas(6), impacto(false), reportado(false),
    vx(0.0), vy(0.0), ySuelo(0.0), radioDanio(0.0), radioEmpuje(0.0),
    fuerzaEmpuje(0.0), amortiguamiento(0.0),
    explotada(false),animandoExplosion(false), tiempoExplosion(0.0)
{
    aplicarAspecto();
}

// Elige sprite y tamaño segun el tipo y quien lo disparo.
void Proyectil::aplicarAspecto() {
    switch (tipo) {
    case TipoProyectil::RECTO:
        if (propietario == Propietario::REI) {
            tipoSprite = TipoSprite::PROYECTIL_REI;
            ancho = Tamanio::BALA_REI_ANCHO;
            alto = Tamanio::BALA_REI_ALTO;
        } else {
            tipoSprite = TipoSprite::PROYECTIL_MONSTRUO;
            ancho = Tamanio::BALA_LATERAL_ANCHO;
            alto = Tamanio::BALA_LATERAL_ALTO;
        }
        break;
    case TipoProyectil::PARABOLICO:
    case TipoProyectil::ESTALLIDO:
        tipoSprite = TipoSprite::PROYECTIL_MONSTRUO;
        ancho = Tamanio::PARABOLICO_ANCHO;
        alto = Tamanio::PARABOLICO_ALTO;
        break;
    case TipoProyectil::ONDULATORIO:
        tipoSprite = TipoSprite::ONDULATORIO;
        ancho = Tamanio::ONDULATORIO_ANCHO;
        alto = Tamanio::ONDULATORIO_ALTO;
        break;
    case TipoProyectil::BOMBA:
        tipoSprite = TipoSprite::BOMBA;
        ancho = Tamanio::BOMBA_ANCHO;
        alto = Tamanio::BOMBA_ALTO;
        break;
    }
}


bool Proyectil::miraDerecha() const {
    if (tipoSprite == TipoSprite::PUA) return true;   // la pua no se espeja: se rota
    return vx >= 0.0;
}

// La pua sale en todas direcciones (estallido): el sprite apunta a la derecha y se gira segun su velocidad.
double Proyectil::getRotacion() const {
    if (tipoSprite == TipoSprite::PUA) return std::atan2(vy, vx) * 180.0 / PI;
    return 0.0;
}

void Proyectil::actualizar(double dt) {
    if (explotada) {                     // la bomba queda quieta: solo avanza su animacion
        if (animandoExplosion) tiempoExplosion += dt;
        return;
    }              // la bomba queda quieta hasta que el nivel la procese

    if (fisica) {
        // parabolico, ondulatorio y bomba: la fisica calcula la posicion
        fisica->actualizar(dt);
        x = fisica->getX();
        y = fisica->getY();
    } else {
        // recto y estallido: velocidad constante
        x += vx * dt;
        y += vy * dt;
    }

    // La bomba explota cuando su borde inferior llega al suelo
    if (tipo == TipoProyectil::BOMBA && y + alto >= ySuelo) {
        y = ySuelo - alto;
        explotar();
    }
}

void Proyectil::setVelocidad(double vx, double vy) {
    this->vx = vx;
    this->vy = vy;
}


bool Proyectil::fueraDePantalla(double xMin, double xMax, double yMin, double yMax) const {
    return (x + ancho < xMin) || (x > xMax) || (y + alto < yMin) || (y > yMax);
}

void Proyectil::alImpactar(Entidad& objetivo) {
    objetivo.recibirDanio(danio);
    impacto = true;
    desactivar();
}


void Proyectil::alTocarRei(Rei& rei, int danio) {
    if (propietario == Propietario::ENEMIGO && tipo != TipoProyectil::BOMBA) {
        alImpactar(rei);
    }
}



void Proyectil::configurarEstallido(double xExplosion, int numPuas) {
    tipo = TipoProyectil::ESTALLIDO;
    explotaEnX = xExplosion;
    if (numPuas < 6) numPuas = 6;
    if (numPuas > 8) numPuas = 8;
    this->numPuas = numPuas;
    aplicarAspecto();
}

// Llego a la posicion de explosion segun hacia donde se mueve.
bool Proyectil::debeFragmentar() const {
    if (tipo != TipoProyectil::ESTALLIDO) return false;
    return (vx < 0.0) ? (x <= explotaEnX) : (x >= explotaEnX);
}

// Puas rectilineas repartidas en circulo desde el centro de la bomba.
std::vector<Proyectil*> Proyectil::fragmentar() {
    std::vector<Proyectil*> puas;

    double rapidez = std::hypot(vx, vy);
    if (rapidez <= 0.0) rapidez = VELOCIDAD_PUA_DEFECTO;

    const double cx = x + ancho / 2.0;
    const double cy = y + alto / 2.0;

    for (int i = 0; i < numPuas; ++i) {
        const double ang = 2.0 * PI * i / numPuas;
        Proyectil* pua = new Proyectil(cx, cy, TipoProyectil::RECTO, propietario, danio);
        pua->tipoSprite = TipoSprite::PUA;
        pua->ancho = Tamanio::PUA_ANCHO;
        pua->alto = Tamanio::PUA_ALTO;
        pua->x = cx - pua->ancho / 2.0;
        pua->y = cy - pua->alto / 2.0;
        pua->setVelocidad(rapidez * std::cos(ang), rapidez * std::sin(ang));
        puas.push_back(pua);
    }

    desactivar();
    return puas;
}

// Bomba del nivel 2

void Proyectil::configurarBomba(double ySuelo, double radioDanio, double radioEmpuje,
                                double F, double k) {
    tipo = TipoProyectil::BOMBA;
    this->ySuelo = ySuelo;
    this->radioDanio = radioDanio;
    this->radioEmpuje = radioEmpuje;
    this->fuerzaEmpuje = F;
    this->amortiguamiento = k;
    aplicarAspecto();
}

void Proyectil::explotar() {
    explotada = true;
}

bool Proyectil::haExplotado() const {
    return explotada;
}

// Agranda la caja al tamaño de la explosion, centrada en la bomba y con el mismo borde inferior.
void Proyectil::iniciarExplosion() {
    const double cx = x + ancho / 2.0;
    const double pies = y + alto;
    ancho = 2.0 * radioDanio;
    alto = 2.0 * radioDanio;
    x = cx - ancho / 2.0;
    y = pies - alto;
    animandoExplosion = true;
    tiempoExplosion = 0.0;
}

bool Proyectil::estaExplotando() const {
    return animandoExplosion;
}

bool Proyectil::explosionTerminada() const {
    return animandoExplosion && tiempoExplosion >= Animacion::DURACION_EXPLOSION_VISUAL;
}

EstadoVisual Proyectil::getEstadoVisual() const {
    if (tipo == TipoProyectil::BOMBA && animandoExplosion) {
        return EstadoVisual::EXPLOTANDO;
    }
    return EstadoVisual::QUIETO;
}
bool Proyectil::esImpactoDirecto(const Rei& rei) const {
    return distanciaCentros(*this, rei) <= radioDanio;
}

// Onda de empuje segun la distancia a Rei; nullptr si esta fuera del radio.
OndaEmpuje* Proyectil::calcularEmpuje(const Rei& rei) const {
    if (tipo != TipoProyectil::BOMBA) return nullptr;

    const double d = distanciaCentros(*this, rei);
    if (d > radioEmpuje) return nullptr;


    const double centroBombaX = x + ancho / 2.0;
    const double centroReiX = rei.getX() + rei.getAncho() / 2.0;
    const int dir = (centroReiX >= centroBombaX) ? 1 : -1;

    OndaEmpuje* onda = new OndaEmpuje(rei.getX(), rei.getY());
    onda->setParametros(fuerzaEmpuje, radioEmpuje, amortiguamiento, dir);
    onda->setDistancia(d);
    return onda;
}

// Agente inteligente

void Proyectil::setZonaObjetivo(int zona) {
    zonaObjetivo = zona;
}

int Proyectil::getZonaObjetivo() const {
    return zonaObjetivo;
}

bool Proyectil::haImpactado() const {
    return impacto;
}

void Proyectil::marcarReportado() {
    reportado = true;
}

bool Proyectil::fueReportado() const {
    return reportado;
}

Propietario Proyectil::getPropietario() const {
    return propietario;
}

TipoProyectil Proyectil::getTipo() const {
    return tipo;
}
