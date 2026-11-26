#ifndef CAMIONNORCOREANO_H
#define CAMIONNORCOREANO_H
#include "entidades/entidad.h"

// AVANCE: cruza la pantalla de derecha a izquierda y desaparece (nivel 1)
// PATRULLA: va y viene entre dos limites (nivel 2)
enum class ModoCamion { AVANCE, PATRULLA };

class CamionNorcoreano : public Entidad {
private:
    ModoCamion modo;
    double velocidad;
    double limiteIzq;   // solo se usan en modo PATRULLA
    double limiteDer;
    int direccion;      // -1 izquierda, 1 derecha

public:
    CamionNorcoreano(double x, double y, ModoCamion modo, double velocidad);
    ~CamionNorcoreano() override;
    void configurarPatrulla(double izq, double der);
    void actualizar(double dt) override;
    void alTocarRei(Rei& rei, int danio) override;
    ModoCamion getModo() const;
    bool miraDerecha() const override;
};

#endif // CAMIONNORCOREANO_H
