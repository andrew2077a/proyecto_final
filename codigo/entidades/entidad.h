#ifndef ENTIDAD_H
#define ENTIDAD_H
#include "fisicas/fisica.h"

class Rei;

enum class TipoSprite { REI_MECHA, REI_SOLDADO, MONSTRUO, CAMION, PROYECTIL_REI,
                        PROYECTIL_MONSTRUO, ONDULATORIO, PUA, BOMBA, BLOQUE,
                        FOSA, PUAS, BATERIA, GASEOSA, META };

class Entidad{
protected:
    double x;
    double y;
    double ancho;
    double alto;
    bool activa;
    Fisica* fisica;
    TipoSprite tipoSprite;

public:
    Entidad(double x, double y, double ancho, double alto, TipoSprite tipo);
    virtual ~Entidad();
    virtual void actualizar(double dt=0)=0;
    virtual bool recibirDanio(int n);
    virtual void matar();
    virtual bool estaViva() const;
    bool colisionaCon(const Entidad& otra) const;
    void setFisica(Fisica* f);
    void desactivar();
    virtual void alTocarRei(Rei& rei, int danio);
    void setPosicion(double x,double y);
    double getX() const;
    double getY() const;
    double getAncho() const;
    double getAlto() const;
    TipoSprite getTipoSprite() const;
    Fisica* getFisica() const;
};

#endif // ENTIDAD_H
