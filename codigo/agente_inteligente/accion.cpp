#include "accion.h"
#include "entidades/proyectil.h"
#include "fisicas/movParabolico.h"
#include <cmath>
#include <stdexcept>

namespace {
const double PI = std::acos(-1.0);
}

Accion::Accion(const Nivel1::ParamsNivel1& p1, double gravedad)
    : gravedad(gravedad),
    anguloDisparo(p1.anguloDisparo),
    velocidadMax(p1.velocidadMaxDisparo),
    danio(p1.danioDisparo)
{
    if (gravedad <= 0.0) {
        throw std::invalid_argument("Accion: la gravedad debe ser mayor que 0");
    }
    if (anguloDisparo <= 0.0 || anguloDisparo >= 90.0) {
        throw std::invalid_argument("Accion: el angulo de disparo debe estar entre 0 y 90 grados");
    }
    if (velocidadMax <= 0.0) {
        throw std::invalid_argument("Accion: velocidadMax debe ser mayor que 0");
    }
}

Proyectil* Accion::ejecutar(int zona, double centroX, double origenX, double origenY) {
    // Alcance R = distancia horizontal al objetivo; de R = v0^2 * sen(2*theta) / g
    // se despeja v0 = sqrt(R * g / sen(2*theta)).
    const double alcance = std::fabs(centroX - origenX);
    const double sen2t = std::sin(2.0 * anguloDisparo * PI / 180.0);
    double v0 = std::sqrt(alcance * gravedad / sen2t);
    if (v0 > velocidadMax) v0 = velocidadMax;       // el tope de velocidad manda

    const int dirX = (centroX >= origenX) ? 1 : -1; // hacia donde esta el objetivo

    Proyectil* p = new Proyectil(origenX, origenY, TipoProyectil::PARABOLICO,
                                 Propietario::ENEMIGO, danio);
    p->setZonaObjetivo(zona);

    MovParabolico* fis = new MovParabolico(origenX, origenY);
    fis->setParametros(v0, anguloDisparo, gravedad, dirX);
    p->setFisica(fis);                              // el proyectil es dueño de la fisica
    return p;
}