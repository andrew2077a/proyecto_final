#ifndef FISICA_H
#define FISICA_H

// Clase base de los modelos de movimiento. Cada modelo calcula la posicion
// (x, y) en funcion del tiempo transcurrido t, partiendo de (x0, y0).
class Fisica{

protected:
    double x0;   // posicion inicial
    double y0;
    double t;    // tiempo acumulado desde el inicio, en segundos
    double x;    // posicion actual
    double y;

public:
    Fisica(double x0, double y0);
    virtual ~Fisica();

    // Avanza dt segundos y recalcula x e y
    virtual void actualizar(double dt) = 0;

    double getX() const;
    double getY() const;

    void setPosicionInicial(double x0, double y0);   // tambien reinicia el movimiento
    virtual void reiniciar();
    virtual bool haTerminado() const;   // por defecto el movimiento no termina
};

#endif
