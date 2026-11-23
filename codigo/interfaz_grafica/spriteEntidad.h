#ifndef SPRITEENTIDAD_H
#define SPRITEENTIDAD_H
#include <QGraphicsPixmapItem>
#include <QTransform>
#include <QHash>
#include <QVector>
#include <QPixmap>
#include <QSizeF>
#include "entidades/entidad.h"


class SpriteEntidad : public QGraphicsPixmapItem{
private:
    TipoSprite tipoSprite;
    Entidad* clave;
    EstadoVisual estadoActual;     // estado que se muestra ahora
    int fotogramaActual;           // indice del fotograma mostrado
    double tiempoFotograma;        // tiempo acumulado en el fotograma actual
    bool mirandoDerecha;           // sentido pedido por la logica
    int llaveMostrada;
    int indiceMostrado;
    QPixmap repetido;
    QSizeF tamanioRepetido;

    // Lista de fotogramas por tipo, estado y sentido (0 = como viene dibujado, 1 = espejado)
    static QHash<int, QVector<QPixmap>> cache;

    void mostrarFotograma();
    void prepararRepetido(double ancho, double alto);
    bool usarImagenNormal() const;
    double alturaReferencia() const;

public:
    SpriteEntidad(TipoSprite tipo, Entidad* clave);
    ~SpriteEntidad();
    static void cargarImagen(TipoSprite tipo, EstadoVisual estado);
    static void precargarTodo();
    void actualizarAnimacion(double dt, EstadoVisual estado, bool miraDerecha);

    void actualizarPosicion(double x, double y, double ancho, double alto, double camaraX, double rotacion = 0.0);
    const Entidad* getClave()const;
    TipoSprite getTipo() const;
};

#endif // SPRITEENTIDAD_H
