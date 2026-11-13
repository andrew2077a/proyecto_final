#ifndef FISICA_H
#define FISICA_H
class Fisica
{
protected:
    double x0;
    double y0;
    double t;
    double x;
    double y;

public:
    Fisica(double x0, double y0);
    virtual ~Fisica();

    virtual void actualizar(double dt) = 0;

    double getX() const;
    double getY() const;

    void setPosicionInicial(double x0, double y0);
    virtual void reiniciar();
    virtual bool haTerminado() const;
};

#endif