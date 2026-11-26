#ifndef SPRITEENTIDAD_H
#define SPRITEENTIDAD_H
#include <QGraphicsPixmapItem>
#include <QTransform>
#include <QHash>
#include <QVector>
#include <QPixmap>
#include <QSizeF>
#include "entidades/entidad.h"

// Imagen animada de una entidad. No guarda logica del juego: solo muestra
// el fotograma que corresponde al estado visual que le pide la entidad.
class SpriteEntidad : public QGraphicsPixmapItem{
private:
    TipoSprite tipoSprite;
    Entidad* clave;                // entidad a la que representa
    EstadoVisual estadoActual;
    int fotogramaActual;
    double tiempoFotograma;        // tiempo acumulado en el fotograma actual
    bool mirandoDerecha;
    int llaveMostrada;             // ultima imagen puesta, para no repetir setPixmap
    int indiceMostrado;
    QPixmap repetido;              // imagen ya compuesta (solo para las puas)
    QSizeF tamanioRepetido;

    // Fotogramas de cada tipo, estado y sentido (normal o espejado); compartida entre todos los sprites
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
