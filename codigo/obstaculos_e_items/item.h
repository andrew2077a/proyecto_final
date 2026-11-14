#ifndef ITEM_H
#define ITEM_H

#include "entidades/entidad.h"
enum class TipoItem { BATERIA, GASEOSA };

class Item : public Entidad
{
private:
    TipoItem tipo;
    double duracion;
    double multCadencia;
    double multVelocidad;
    bool recogido;

public:
    Item(TipoItem tipo, double x, double y, double ancho, double alto,
         double duracion, double multVelocidad, double multCadencia);
    ~Item() override;

    void alTocarRei(Rei& rei, int danio) override;
    void actualizar(double dt = 0) override;

    void recoger();
    bool estaRecogido() const;
    double getDuracion() const;
    TipoItem getTipo() const;
};

#endif