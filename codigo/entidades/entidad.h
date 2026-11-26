#ifndef ENTIDAD_H
#define ENTIDAD_H
#include "fisicas/fisica.h"

class Rei;

// Estado de la animacion: decide que tira de sprite se muestra
enum class EstadoVisual { QUIETO, CORRIENDO, SALTANDO, DISPARANDO, DANIADO, LANZANDO, MURIENDO, EXPLOTANDO,AGACHADO };

// Cada valor corresponde al archivo de imagen del mismo nombre
enum class TipoSprite { REI_MECHA, REI_SOLDADO, MONSTRUO, CAMION, PROYECTIL_REI,
                        PROYECTIL_MONSTRUO, ONDULATORIO, PUA, BOMBA, BLOQUE,
                        FOSA, PUAS, BATERIA, GASEOSA, META };

// Clase base de todo lo que existe en el nivel (logica, sin nada de Qt)
class Entidad{
protected:
    double x;       // esquina superior izquierda de la caja de colision
    double y;
    double ancho;
    double alto;
    bool activa;    // false = el nivel la elimina
    Fisica* fisica; // movimiento opcional; la entidad es su dueña
    TipoSprite tipoSprite;

public:
    Entidad(double x, double y, double ancho, double alto, TipoSprite tipo);
    virtual ~Entidad();
    virtual void actualizar(double dt=0)=0;
    virtual bool recibirDanio(int n);   // devuelve true si el daño se aplico
    virtual void matar();
    virtual bool estaViva() const;
    bool colisionaCon(const Entidad& otra) const;
    void setFisica(Fisica* f);
    void desactivar();
    virtual void alTocarRei(Rei& rei, int danio);   // reaccion al chocar con Rei
    void setPosicion(double x,double y);
    double getX() const;
    double getY() const;
    double getAncho() const;
    double getAlto() const;
    TipoSprite getTipoSprite() const;
    Fisica* getFisica() const;

    // Datos para dibujar; las clases hijas los sobrescriben si les hace falta
    virtual EstadoVisual getEstadoVisual() const { return EstadoVisual::QUIETO; }
    virtual bool miraDerecha() const { return true; }
    virtual double getRotacion() const { return 0.0; }   // grados
    virtual double getAltoVisual() const { return alto; }
};

#endif // ENTIDAD_H
