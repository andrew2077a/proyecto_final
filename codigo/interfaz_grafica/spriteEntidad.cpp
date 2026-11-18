#include "spriteEntidad.h"
#include "datos.h"
#include <stdexcept>

QHash<TipoSprite, QPixmap> SpriteEntidad::cache;

namespace {
QString nombreDe(TipoSprite tipo) {
    switch (tipo) {
    case TipoSprite::REI_MECHA:          return "rei_mecha";
    case TipoSprite::REI_SOLDADO:        return "rei_soldado";
    case TipoSprite::MONSTRUO:           return "monstruo";
    case TipoSprite::CAMION:             return "camion";
    case TipoSprite::PROYECTIL_REI:      return "proyectil_rei";
    case TipoSprite::PROYECTIL_MONSTRUO: return "proyectil_monstruo";
    case TipoSprite::ONDULATORIO:        return "ondulatorio";
    case TipoSprite::PUA:                return "pua";
    case TipoSprite::BOMBA:              return "bomba";
    case TipoSprite::BLOQUE:             return "bloque";
    case TipoSprite::FOSA:               return "fosa";
    case TipoSprite::PUAS:               return "puas";
    case TipoSprite::BATERIA:            return "bateria";
    case TipoSprite::GASEOSA:            return "gaseosa";
    case TipoSprite::META:               return "meta";
    }
    return "";
}
}
SpriteEntidad::SpriteEntidad(TipoSprite tipo, Entidad* clave)
    :tipoSprite(tipo),clave(clave) {
    cargarImagen(tipo);
    setTransformationMode(Qt::SmoothTransformation);
}
SpriteEntidad::~SpriteEntidad(){
}
void SpriteEntidad::cargarImagen(TipoSprite tipo){
    if(!cache.contains(tipo)){
        QString ruta = QString(Rutas::CARPETA_SPRITES) + nombreDe(tipo) + Rutas::EXTENSION_SPRITES;
        QPixmap imagen(ruta);
        if(imagen.isNull()){
            throw std::runtime_error("No se pudo cargar el sprite: " + ruta.toStdString());
        }
        cache.insert(tipo, imagen);
    }
    setPixmap(cache.value(tipo));
}

void SpriteEntidad::actualizarPosicion(double x, double y, double ancho, double alto, double camaraX) {
    const QPixmap imagen = cache.value(tipoSprite);
    setTransform(QTransform::fromScale(ancho / imagen.width(), alto / imagen.height()));
    setPos(x - camaraX, y);
}

const Entidad* SpriteEntidad::getClave() const {
    return clave;
}