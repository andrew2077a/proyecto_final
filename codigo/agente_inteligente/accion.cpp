#include "accion.h"
#include "entidades/proyectil.h"
#include "fisicas/movParabolico.h"
#include <cmath>
#include <stdexcept>
#include "datos.h"
namespace {
const double PI = std::acos(-1.0);
}

Accion::Accion(const ParamsNivel1& p1, double gravedad)
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
    // Punto de impacto: el centro de Rei dentro de la zona
    const double objetivoX = centroX + Tamanio::REI_MECHA_ANCHO / 2.0 - Tamanio::PARABOLICO_ANCHO / 2.0;
    const double objetivoY = Nivel1Init::Y_SUELO - Tamanio::REI_MECHA_ALTO / 2.0 - Tamanio::PARABOLICO_ALTO / 2.0;
    const double alcance = std::fabs(objetivoX - origenX);
    const double rad = anguloDisparo * PI / 180.0;
    const double dy = objetivoY - origenY;   // positivo: el objetivo esta mas abajo
    // v0 que hace pasar la parabola por el objetivo
    const double den = 2.0 * std::cos(rad) * std::cos(rad) * (alcance * std::tan(rad) + dy);
    double v0 = (den > 0.0) ? std::sqrt(gravedad * alcance * alcance / den) : velocidadMax;
    if (v0 > velocidadMax) v0 = velocidadMax;
    const int dirX = (objetivoX >= origenX) ? 1 : -1;

    Proyectil* p = new Proyectil(origenX, origenY, TipoProyectil::PARABOLICO,Propietario::ENEMIGO, danio);
    p->setZonaObjetivo(zona);   // para reportar luego si acerto

    MovParabolico* fis = new MovParabolico(origenX, origenY);
    fis->setParametros(v0, anguloDisparo, gravedad, dirX);
    p->setFisica(fis);   // el proyectil es dueño de la fisica
    return p;
}
