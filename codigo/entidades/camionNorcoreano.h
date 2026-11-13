#ifndef CAMIONNORCOREANO_H
#define CAMIONNORCOREANO_H
#include "entidades/entidad.h"

enum class ModoCamion { AVANCE, PATRULLA };

class CamionNorcoreano : public Entidad {
private:
    ModoCamion modo;
    double velocidad;
    double limiteIzq;
    double limiteDer;
    int direccion;

public:
    CamionNorcoreano(double x, double y, ModoCamion modo, double velocidad);
    ~CamionNorcoreano() override;
    void configurarPatrulla(double izq, double der);
    void actualizar(double dt) override;
    void alTocarRei(Rei& rei, int danio) override;
    ModoCamion getModo() const;
};

#endif // CAMIONNORCOREANO_H
