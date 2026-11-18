#ifndef SPRITEENTIDAD_H
#define SPRITEENTIDAD_H
#include <QGraphicsPixmapItem>
#include <QTransform>
#include "entidades/entidad.h"


class SpriteEntidad : public QGraphicsPixmapItem{
private:
    TipoSprite	tipoSprite;
    Entidad* clave;
    static QHash<TipoSprite, QPixmap> cache;
public:
    SpriteEntidad(TipoSprite tipo, Entidad* clave);
    ~SpriteEntidad();
    void cargarImagen(TipoSprite tipo);
    void actualizarPosicion(double x, double y, double ancho, double alto, double camaraX);
    const Entidad* getClave()const;
};

#endif // SPRITEENTIDAD_H