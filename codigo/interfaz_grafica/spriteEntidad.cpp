#include "spriteEntidad.h"
#include "datos.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <QFile>
#include <QImage>
#include <QPainter>

QHash<int, QVector<QPixmap>> SpriteEntidad::cache;

namespace {
// Nombre base del archivo de imagen de cada tipo
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

// Sufijo del archivo segun el estado (QUIETO no lleva sufijo)
QString nombreEstadoDe(EstadoVisual estado) {
    switch (estado) {
    case EstadoVisual::QUIETO:     return "";
    case EstadoVisual::CORRIENDO:  return "_corriendo";
    case EstadoVisual::SALTANDO:   return "_saltando";
    case EstadoVisual::DISPARANDO: return "_disparando";
    case EstadoVisual::DANIADO:    return "_danio";      // sin ñ en el nombre del archivo
    case EstadoVisual::LANZANDO:   return "_lanzando";
    case EstadoVisual::MURIENDO:   return "_muriendo";
    case EstadoVisual::AGACHADO:   return "_agachado";
    case EstadoVisual::EXPLOTANDO: return "_explotando";
    }
    return "";
}

// Cada animacion es una tira horizontal de fotogramas del mismo ancho
struct EntradaAnimacion {
    TipoSprite tipo;
    EstadoVisual estado;
    int fotogramas;            // cuantos fotogramas tiene la tira
    double duracionFotograma;  // segundos por fotograma
    bool repite;               // true: vuelve al primero; false: se queda en el ultimo
};

// Los comentarios de la derecha indican con que duracion de datos.h debe coincidir la animacion
const EntradaAnimacion TABLA[] = {
    { TipoSprite::REI_MECHA,   EstadoVisual::QUIETO,     1, 0.25,  true  },
    { TipoSprite::REI_MECHA,   EstadoVisual::CORRIENDO,  6, 0.08,  true  },
    { TipoSprite::REI_MECHA,   EstadoVisual::SALTANDO,   3, 0.12,  false },
    { TipoSprite::REI_MECHA,   EstadoVisual::DISPARANDO, 6, 0.04,  false },   // 6 * 0.04 = 0.24 s = DURACION_DISPARO_VISUAL
    { TipoSprite::REI_MECHA,   EstadoVisual::DANIADO,    6, 0.07,  false },   // 6 * 0.07 = 0.42 s = DURACION_DANIO_VISUAL
    { TipoSprite::REI_MECHA,   EstadoVisual::AGACHADO,   1, 0.25,  true  },
    { TipoSprite::REI_SOLDADO, EstadoVisual::AGACHADO,   1, 0.25,  true  },
    { TipoSprite::REI_SOLDADO, EstadoVisual::QUIETO,     4, 0.25,  true  },
    { TipoSprite::REI_SOLDADO, EstadoVisual::CORRIENDO,  6, 0.08,  true  },
    { TipoSprite::REI_SOLDADO, EstadoVisual::SALTANDO,   3, 0.12,  false },
    { TipoSprite::REI_SOLDADO, EstadoVisual::DANIADO,    6, 0.07,  false },
    { TipoSprite::MONSTRUO,    EstadoVisual::QUIETO,     4, 0.25,  true  },
    { TipoSprite::MONSTRUO,    EstadoVisual::LANZANDO,   5, 0.20,  false },   // 5 * 0.20 = 1.0 s = TIEMPO_CARGA (Normal)
    { TipoSprite::MONSTRUO,    EstadoVisual::MURIENDO,   5, 0.25,  false },   // 5 * 0.25 = 1.25 s (ESPERA_FIN vale 0.5 s)
    { TipoSprite::BOMBA,       EstadoVisual::EXPLOTANDO, 4, 0.09,  false },   // 4 * 0.09 = 0.36 s = DURACION_EXPLOSION_VISUAL
    { TipoSprite::CAMION,      EstadoVisual::QUIETO,     4, 0.08,  true  }
};
const int NUM_ENTRADAS = static_cast<int>(sizeof(TABLA) / sizeof(TABLA[0]));

// Si no hay fila: 1 fotograma y no repite
EntradaAnimacion infoDe(TipoSprite tipo, EstadoVisual estado) {
    for (int i = 0; i < NUM_ENTRADAS; ++i) {
        if (TABLA[i].tipo == tipo && TABLA[i].estado == estado) {
            return TABLA[i];
        }
    }
    return { tipo, estado, 1, 0.0, false };
}

// Llave de la cache: tipo*1000 + estado*10 + sentido (0 = normal, 1 = espejado)
int llaveDe(TipoSprite tipo, EstadoVisual estado, bool normal) {
    return static_cast<int>(tipo) * 1000 + static_cast<int>(estado) * 10 + (normal ? 0 : 1);
}

// Como se adapta la imagen a la caja de colision de la entidad:
//   PERSONAJE        escala segun el alto del sprite quieto, con los pies en el borde inferior de la caja
//   ESTIRAR          rellena la caja completa
//   CONTENER_PIES    cabe dentro de la caja sin deformarse, apoyada abajo
//   CONTENER_CENTRO  cabe dentro de la caja sin deformarse, centrada
//   REPETIR          una baldosa repetida a lo ancho de la caja
enum class Ajuste { PERSONAJE, ESTIRAR, CONTENER_PIES, CONTENER_CENTRO, REPETIR };

struct ConfigDibujo {
    TipoSprite tipo;
    Ajuste ajuste;
    bool nativoDerecha;   // el dibujo original mira a la derecha
    double z;             // capa: mayor se dibuja encima
    double escala;
};

const ConfigDibujo CONFIG[] = {
    { TipoSprite::FOSA,               Ajuste::ESTIRAR,         true,  1, 1.0 },
    { TipoSprite::BLOQUE,             Ajuste::ESTIRAR,         true,  2, 1.0 },
    { TipoSprite::PUAS,               Ajuste::REPETIR,         true,  3, 1.0 },
    { TipoSprite::META,               Ajuste::ESTIRAR,         true,  3, 1.0 },
    { TipoSprite::BATERIA,            Ajuste::CONTENER_PIES,   true,  4, 1.0 },
    { TipoSprite::GASEOSA,            Ajuste::CONTENER_PIES,   true,  4, 1.0 },
    { TipoSprite::CAMION,             Ajuste::CONTENER_PIES,   false, 5, 1.0 },
    { TipoSprite::MONSTRUO,           Ajuste::PERSONAJE,       true,  6, 1.0 },
    { TipoSprite::REI_MECHA,          Ajuste::PERSONAJE,       true,  7, 1.0 },
    { TipoSprite::REI_SOLDADO,        Ajuste::PERSONAJE,       true,  7, 1.0 },
    { TipoSprite::PROYECTIL_REI,      Ajuste::CONTENER_CENTRO, true,  8, 1.0 },
    { TipoSprite::PROYECTIL_MONSTRUO, Ajuste::CONTENER_CENTRO, true,  8, 1.0 },
    { TipoSprite::ONDULATORIO,        Ajuste::CONTENER_CENTRO, true,  8, 1.0 },
    { TipoSprite::PUA,                Ajuste::CONTENER_CENTRO, true,  8, 1.8 },
    { TipoSprite::BOMBA,              Ajuste::CONTENER_CENTRO, true,  8, 1.0 }
};
const int NUM_CONFIG = static_cast<int>(sizeof(CONFIG) / sizeof(CONFIG[0]));

ConfigDibujo configDe(TipoSprite tipo) {
    for (int i = 0; i < NUM_CONFIG; ++i) {
        if (CONFIG[i].tipo == tipo) {
            return CONFIG[i];
        }
    }
    return { tipo, Ajuste::ESTIRAR, true, 0, 1.0 };
}
}

SpriteEntidad::SpriteEntidad(TipoSprite tipo, Entidad* clave)
    :tipoSprite(tipo),clave(clave),
    estadoActual(EstadoVisual::QUIETO), fotogramaActual(0),
    tiempoFotograma(0.0), mirandoDerecha(true),
    llaveMostrada(-1), indiceMostrado(-1), tamanioRepetido(0, 0) {
    cargarImagen(tipo, EstadoVisual::QUIETO);
    setTransformationMode(Qt::SmoothTransformation);
    setZValue(configDe(tipo).z);
    mostrarFotograma();
}
SpriteEntidad::~SpriteEntidad(){
}

// Carga una tira, la corta en fotogramas iguales y guarda la version normal y la espejada
void SpriteEntidad::cargarImagen(TipoSprite tipo, EstadoVisual estado){
    if(cache.contains(llaveDe(tipo, estado, true))){
        return;
    }

    QString ruta = QString(Rutas::CARPETA_SPRITES) + nombreDe(tipo)+ nombreEstadoDe(estado) + Rutas::EXTENSION_SPRITES;

    // Si la tira de este estado no existe se usa la de QUIETO, asi se pueden ir agregando animaciones de a una
    if(estado != EstadoVisual::QUIETO && !QFile::exists(ruta)){
        cargarImagen(tipo, EstadoVisual::QUIETO);

        cache.insert(
            llaveDe(tipo, estado, true),
            cache.value(llaveDe(tipo, EstadoVisual::QUIETO, true))
            );
        cache.insert(
            llaveDe(tipo, estado, false),
            cache.value(llaveDe(tipo, EstadoVisual::QUIETO, false))
            );
        return;
    }

    QPixmap tira(ruta);
    if(tira.isNull()){
        throw std::runtime_error(
            "No se pudo cargar el sprite: " + ruta.toStdString()
            );
    }

    const int n = infoDe(tipo, estado).fotogramas;
    const int anchoFotograma = tira.width() / n;

    QVector<QPixmap> normales;
    QVector<QPixmap> espejados;

    for(int i = 0; i < n; ++i){
        QPixmap fotograma = tira.copy(
            i * anchoFotograma,
            0,
            anchoFotograma,
            tira.height()
            );

        normales.push_back(fotograma);
        espejados.push_back(
            QPixmap::fromImage(fotograma.toImage().mirrored(true, false))
            );
    }

    cache.insert(llaveDe(tipo, estado, true), normales);
    cache.insert(llaveDe(tipo, estado, false), espejados);
}

// Se llama al iniciar el nivel para que un archivo faltante falle ahi y no a mitad de partida
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

// Si el dibujo original mira a la izquierda (camion), la imagen "normal" es la que mira a la izquierda
bool SpriteEntidad::usarImagenNormal() const{
    return mirandoDerecha == configDe(tipoSprite).nativoDerecha;
}

// Alto del sprite quieto: sirve de referencia para que todas las animaciones del personaje tengan la misma escala
double SpriteEntidad::alturaReferencia() const{
    const QVector<QPixmap> quieto = cache.value(llaveDe(tipoSprite, EstadoVisual::QUIETO, true));
    return quieto.isEmpty() ? 0.0 : quieto[0].height();
}

void SpriteEntidad::actualizarAnimacion(double dt, EstadoVisual estado, bool miraDerecha){
    if(estado != estadoActual){   // al cambiar de estado la animacion arranca de cero
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
        // while por si un frame largo salta varios fotogramas
        while(tiempoFotograma >= info.duracionFotograma){
            tiempoFotograma -= info.duracionFotograma;
            if(fotogramaActual < n - 1){
                fotogramaActual++;
            }
            else if(info.repite){
                fotogramaActual = 0;
            }
            else{
                tiempoFotograma = 0.0;   // se queda en el ultimo fotograma
                break;
            }
        }
    }
    mostrarFotograma();
}

void SpriteEntidad::mostrarFotograma(){
    if(configDe(tipoSprite).ajuste == Ajuste::REPETIR && !repetido.isNull()){
        return;   // las puas ya tienen su imagen compuesta
    }
    const int llave = llaveDe(tipoSprite, estadoActual, usarImagenNormal());
    const QVector<QPixmap> fotogramas = cache.value(llave);
    if(fotogramas.isEmpty()){
        return;
    }
    int i = fotogramaActual;
    if(i >= fotogramas.size()){
        i = fotogramas.size() - 1;
    }
    if(llave == llaveMostrada && i == indiceMostrado){
        return;   // ya es la imagen que se esta mostrando
    }
    setPixmap(fotogramas[i]);
    llaveMostrada = llave;
    indiceMostrado = i;
}

// Arma una sola vez la imagen de las puas: la baldosa se repite a lo ancho de la caja de colision
void SpriteEntidad::prepararRepetido(double ancho, double alto){
    if(!repetido.isNull() && tamanioRepetido == QSizeF(ancho, alto)){
        return;
    }
    const QVector<QPixmap> base = cache.value(llaveDe(tipoSprite, EstadoVisual::QUIETO, true));
    if(base.isEmpty() || ancho <= 0 || alto <= 0){
        return;
    }
    const QPixmap& baldosa = base[0];
    const double relacion = static_cast<double>(baldosa.width()) / baldosa.height();
    int n = static_cast<int>(std::lround(ancho / (alto * relacion)));   // cuantas baldosas caben sin deformarse
    if(n < 1){
        n = 1;
    }
    const int pw = static_cast<int>(ancho * 2);   // se arma al doble de tamaño para que no se vea borrosa
    const int ph = static_cast<int>(alto * 2);
    QPixmap compuesto(pw, ph);
    compuesto.fill(Qt::transparent);
    QPainter pintor(&compuesto);
    pintor.setRenderHint(QPainter::SmoothPixmapTransform);
    const double celda = static_cast<double>(pw) / n;
    for(int i = 0; i < n; ++i){
        pintor.drawPixmap(QRectF(i * celda, 0, celda, ph), baldosa, QRectF(baldosa.rect()));
    }
    pintor.end();
    repetido = compuesto;
    tamanioRepetido = QSizeF(ancho, alto);
    setPixmap(repetido);
}

// Coloca y escala el sprite sobre la caja de colision de la entidad (x, y, ancho, alto en coordenadas del mundo)
void SpriteEntidad::actualizarPosicion(double x, double y, double ancho, double alto, double camaraX, double rotacion) {
    const ConfigDibujo cfg = configDe(tipoSprite);
    if(cfg.ajuste == Ajuste::REPETIR){
        prepararRepetido(ancho, alto);
    }
    const QPixmap imagen = pixmap();
    if(imagen.isNull() || ancho <= 0 || alto <= 0){
        return;
    }
    const double w = imagen.width();
    const double h = imagen.height();

    // Caja en coordenadas de pantalla: se resta la camara
    const double cajaX = x - camaraX;
    const double cajaY = y;

    double dw = ancho;   // tamaño final con que se dibuja
    double dh = alto;
    double cx = cajaX + ancho / 2.0;   // centro final
    double cy = cajaY + alto / 2.0;

    switch(cfg.ajuste){
    case Ajuste::PERSONAJE: {
        const double ref = alturaReferencia();
        const double s = (ref > 0.0) ? alto / ref : alto / h;
        dw = w * s;
        dh = h * s;
        cy = cajaY + alto - dh / 2.0;
        break;
    }
    case Ajuste::CONTENER_PIES:
    case Ajuste::CONTENER_CENTRO: {
        const double s = std::min(ancho / w, alto / h) * cfg.escala;
        dw = w * s;
        dh = h * s;
        if(cfg.ajuste == Ajuste::CONTENER_PIES){
            cy = cajaY + alto - dh / 2.0;
        }
        break;
    }
    case Ajuste::ESTIRAR:
    case Ajuste::REPETIR:
        break;   // rellena la caja completa
    }

    // El orden importa: la imagen se centra en su origen, se escala y se rota alrededor de su centro
    QTransform t;
    t.rotate(rotacion);
    t.scale(dw / w, dh / h);
    t.translate(-w / 2.0, -h / 2.0);
    setTransform(t);
    setPos(cx, cy);
}

const Entidad* SpriteEntidad::getClave() const {
    return clave;
}

TipoSprite SpriteEntidad::getTipo() const {
    return tipoSprite;
}
