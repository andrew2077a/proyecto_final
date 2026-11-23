#ifndef SPRITEENTIDAD_H
#define SPRITEENTIDAD_H
#include <QGraphicsPixmapItem>
#include <QTransform>
#include <QHash>       // CAMBIO
#include <QVector>     // CAMBIO
#include <QPixmap>     // CAMBIO
#include "entidades/entidad.h"


class SpriteEntidad : public QGraphicsPixmapItem{
private:
    // OJO: este orden es el de la lista de inicialización del constructor
    TipoSprite tipoSprite;
    Entidad* clave;
    EstadoVisual estadoActual;     // CAMBIO: estado que se muestra ahora
    int fotogramaActual;           // CAMBIO: índice del fotograma mostrado
    double tiempoFotograma;        // CAMBIO: tiempo acumulado en el fotograma actual
    bool mirandoDerecha;           // CAMBIO: sentido mostrado

    // CAMBIO: antes QHash<TipoSprite, QPixmap>. Ahora guarda la lista de fotogramas por tipo, estado y sentido
    static QHash<int, QVector<QPixmap>> cache;

    void mostrarFotograma();       // CAMBIO: pone en pantalla el fotograma y sentido actuales

public:
    SpriteEntidad(TipoSprite tipo, Entidad* clave);
    ~SpriteEntidad();
    static void cargarImagen(TipoSprite tipo, EstadoVisual estado);   // CAMBIO: ahora static y con estado
    static void precargarTodo();                                      // CAMBIO: nuevo
    void actualizarAnimacion(double dt, EstadoVisual estado, bool miraDerecha);   // CAMBIO: nuevo
    void actualizarPosicion(double x, double y, double ancho, double alto, double camaraX);
    const Entidad* getClave()const;
};

#endif // SPRITEENTIDAD_H