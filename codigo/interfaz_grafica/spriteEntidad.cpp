#include "spriteEntidad.h"
#include "datos.h"
#include <stdexcept>
#include <QFile>     // CAMBIO: para comprobar si una tira existe
#include <QImage>    // CAMBIO: para espejar los fotogramas

QHash<int, QVector<QPixmap>> SpriteEntidad::cache;

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

// CAMBIO: sufijo del archivo según el estado (QUIETO no lleva sufijo)
QString nombreEstadoDe(EstadoVisual estado) {
    switch (estado) {
    case EstadoVisual::QUIETO:     return "";
    case EstadoVisual::CORRIENDO:  return "_corriendo";
    case EstadoVisual::SALTANDO:   return "_saltando";
    case EstadoVisual::DISPARANDO: return "_disparando";
    case EstadoVisual::DANIADO:    return "_danio";      // sin ñ
    case EstadoVisual::LANZANDO:   return "_lanzando";
    case EstadoVisual::MURIENDO:   return "_muriendo";
    }
    return "";
}

// CAMBIO: una fila por animación (datos de presentación)
struct EntradaAnimacion {
    TipoSprite tipo;
    EstadoVisual estado;
    int fotogramas;            // cuántos fotogramas tiene la tira
    double duracionFotograma;  // segundos por fotograma
    bool repite;               // true: vuelve al primero; false: se queda en el último
};

// PRUEBA: mientras una tira no sea real, pon 1 en "fotogramas" (si no, la validación del ancho falla).
// Cuando tengas la tira, pon el número real (4 o 6).
const EntradaAnimacion TABLA[] = {
    { TipoSprite::REI_MECHA,   EstadoVisual::QUIETO,     1, 0.25,  true  },
    { TipoSprite::REI_MECHA,   EstadoVisual::CORRIENDO,  6, 0.08,  true  },
    { TipoSprite::REI_MECHA,   EstadoVisual::SALTANDO,   3, 0.12,  false },
    { TipoSprite::REI_MECHA,   EstadoVisual::DISPARANDO, 4, 0.06,  false },
    { TipoSprite::REI_MECHA,   EstadoVisual::DANIADO,    6, 0.07,  false },   // tu hoja de referencia: 6 fotogramas
    { TipoSprite::REI_SOLDADO, EstadoVisual::QUIETO,     1, 0.25,  true  },
    { TipoSprite::REI_SOLDADO, EstadoVisual::CORRIENDO,  6, 0.08,  true  },
    { TipoSprite::REI_SOLDADO, EstadoVisual::SALTANDO,   3, 0.12,  false },
    { TipoSprite::REI_SOLDADO, EstadoVisual::DANIADO,    4, 0.105, false },
    { TipoSprite::MONSTRUO,    EstadoVisual::QUIETO,     1, 0.25,  true  },
    { TipoSprite::MONSTRUO,    EstadoVisual::LANZANDO,   6, 0.10,  false },
    { TipoSprite::MONSTRUO,    EstadoVisual::MURIENDO,   5, 0.25,  false },
    { TipoSprite::CAMION,      EstadoVisual::QUIETO,     1, 0.10,  true  }
};
const int NUM_ENTRADAS = static_cast<int>(sizeof(TABLA) / sizeof(TABLA[0]));

// Busca la fila de la tabla. Si no hay: 1 fotograma y no repite.
EntradaAnimacion infoDe(TipoSprite tipo, EstadoVisual estado) {
    for (int i = 0; i < NUM_ENTRADAS; ++i) {
        if (TABLA[i].tipo == tipo && TABLA[i].estado == estado) {
            return TABLA[i];
        }
    }
    return { tipo, estado, 1, 0.0, false };
}

// Llave de la caché: tipo*1000 + estado*10 + sentido (0 = derecha, 1 = izquierda)
int llaveDe(TipoSprite tipo, EstadoVisual estado, bool derecha) {
    return static_cast<int>(tipo) * 1000 + static_cast<int>(estado) * 10 + (derecha ? 0 : 1);
}
}

SpriteEntidad::SpriteEntidad(TipoSprite tipo, Entidad* clave)
    :tipoSprite(tipo),clave(clave),
    estadoActual(EstadoVisual::QUIETO), fotogramaActual(0),   // CAMBIO
    tiempoFotograma(0.0), mirandoDerecha(true) {              // CAMBIO
    cargarImagen(tipo, EstadoVisual::QUIETO);
    mostrarFotograma();
    setTransformationMode(Qt::SmoothTransformation);
}
SpriteEntidad::~SpriteEntidad(){
}

// CAMBIO: carga una tira, la corta en fotogramas y guarda la versión normal y la espejada
void SpriteEntidad::cargarImagen(TipoSprite tipo, EstadoVisual estado){
    if(cache.contains(llaveDe(tipo, estado, true))){
        return;                                   // ya está en la caché
    }
    QString ruta = QString(Rutas::CARPETA_SPRITES) + nombreDe(tipo)
                   + nombreEstadoDe(estado) + Rutas::EXTENSION_SPRITES;

    // CAMBIO: si la tira de un ESTADO todavía no existe, se usa la del estado QUIETO.
    // Así puedes ir agregando animaciones de a una sin que el juego falle. La tira QUIETO sí es obligatoria.
    if(estado != EstadoVisual::QUIETO && !QFile::exists(ruta)){
        cargarImagen(tipo, EstadoVisual::QUIETO);
        cache.insert(llaveDe(tipo, estado, true),  cache.value(llaveDe(tipo, EstadoVisual::QUIETO, true)));
        cache.insert(llaveDe(tipo, estado, false), cache.value(llaveDe(tipo, EstadoVisual::QUIETO, false)));
        return;
    }

    QPixmap tira(ruta);
    if(tira.isNull()){
        throw std::runtime_error("No se pudo cargar el sprite: " + ruta.toStdString());
    }
    const int n = infoDe(tipo, estado).fotogramas;
    const int anchoFotograma = tira.width() / n;   // si no es exacto, sobra un poco a la derecha

    QVector<QPixmap> normales;
    QVector<QPixmap> espejados;
    for(int i = 0; i < n; ++i){
        QPixmap fotograma = tira.copy(i * anchoFotograma, 0, anchoFotograma, tira.height());
        normales.push_back(fotograma);
        espejados.push_back(QPixmap::fromImage(fotograma.toImage().mirrored(true, false)));
    }
    cache.insert(llaveDe(tipo, estado, true),  normales);
    cache.insert(llaveDe(tipo, estado, false), espejados);
}

// CAMBIO: carga todo al iniciar el nivel, para que un archivo faltante falle ahí y no a mitad de partida
void SpriteEntidad::precargarTodo(){
    const TipoSprite tipos[] = {
        TipoSprite::REI_MECHA, TipoSprite::REI_SOLDADO, TipoSprite::MONSTRUO, TipoSprite::CAMION,
        TipoSprite::PROYECTIL_REI, TipoSprite::PROYECTIL_MONSTRUO, TipoSprite::ONDULATORIO,
        TipoSprite::PUA, TipoSprite::BOMBA, TipoSprite::BLOQUE, TipoSprite::FOSA, TipoSprite::PUAS,
        TipoSprite::BATERIA, TipoSprite::GASEOSA
    };
    for(TipoSprite t : tipos){
        cargarImagen(t, EstadoVisual::QUIETO);
    }
    for(int i = 0; i < NUM_ENTRADAS; ++i){
        cargarImagen(TABLA[i].tipo, TABLA[i].estado);
    }
}

// CAMBIO: avanza la animación con el dt del ciclo
void SpriteEntidad::actualizarAnimacion(double dt, EstadoVisual estado, bool miraDerecha){
    if(estado != estadoActual){                   // cambió el estado: se reinicia la animación
        estadoActual = estado;
        fotogramaActual = 0;
        tiempoFotograma = 0.0;
        cargarImagen(tipoSprite, estado);
    }
    mirandoDerecha = miraDerecha;

    const int n = cache.value(llaveDe(tipoSprite, estadoActual, true)).size();
    const EntradaAnimacion info = infoDe(tipoSprite, estadoActual);
    if(n > 1 && info.duracionFotograma > 0.0){
        tiempoFotograma += dt;
        while(tiempoFotograma >= info.duracionFotograma){
            tiempoFotograma -= info.duracionFotograma;
            if(fotogramaActual < n - 1){
                fotogramaActual++;
            }
            else if(info.repite){
                fotogramaActual = 0;
            }
            else{
                tiempoFotograma = 0.0;            // se queda en el último fotograma
                break;
            }
        }
    }
    mostrarFotograma();
}

void SpriteEntidad::mostrarFotograma(){
    const QVector<QPixmap> fotogramas = cache.value(llaveDe(tipoSprite, estadoActual, mirandoDerecha));
    if(fotogramas.isEmpty()){
        return;
    }
    int i = fotogramaActual;
    if(i >= fotogramas.size()){
        i = fotogramas.size() - 1;
    }
    setPixmap(fotogramas[i]);
}

void SpriteEntidad::actualizarPosicion(double x, double y, double ancho, double alto, double camaraX) {
    // CAMBIO: antes leía cache.value(tipoSprite); ahora escala con el fotograma que se muestra
    const QPixmap imagen = pixmap();
    if(imagen.isNull()){
        return;
    }
    setTransform(QTransform::fromScale(ancho / imagen.width(), alto / imagen.height()));
    setPos(x - camaraX, y);
}

const Entidad* SpriteEntidad::getClave() const {
    return clave;
}
