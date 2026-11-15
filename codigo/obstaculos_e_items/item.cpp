#include "item.h"
#include "entidades/rei.h"

namespace {
TipoSprite spriteDe(TipoItem tipo) {
    switch (tipo) {
    case TipoItem::BATERIA: return TipoSprite::BATERIA;
    case TipoItem::GASEOSA: return TipoSprite::GASEOSA;
    }
    return TipoSprite::BATERIA;
}
}

Item::Item(TipoItem tipo, double x, double y, double ancho, double alto,
           double duracion, double multVelocidad, double multCadencia)
    : Entidad(x, y, ancho, alto, spriteDe(tipo)),
    tipo(tipo), duracion(duracion),
    multCadencia(multCadencia), multVelocidad(multVelocidad),
    recogido(false) {}

Item::~Item() {}
void Item::alTocarRei(Rei& rei, int danio) {
    (void)danio;
    if (recogido) return;

    switch (tipo) {
    case TipoItem::BATERIA:
        rei.activarBateria(duracion, multCadencia, multVelocidad);
        break;
    case TipoItem::GASEOSA:
        rei.activarEscudo(duracion, multVelocidad);
        break;
    }
    recoger();
}

void Item::actualizar(double dt) {
    (void)dt;
}

void Item::recoger() {
    recogido = true;
    desactivar();
}

bool Item::estaRecogido() const {
    return recogido;
}

double Item::getDuracion() const {
    return duracion;
}

TipoItem Item::getTipo() const {
    return tipo;
}
