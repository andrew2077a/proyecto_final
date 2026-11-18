#ifndef REI_H
#define REI_H
#include "entidades/entidad.h"
#include "fisicas/movParabolico.h"
#include "fisicas/ondaEmpuje.h"
#include "entidades/proyectil.h"
#include "datos.h"

struct ParamsComunes;
struct ParamsNivel1;

enum class Tecla { IZQUIERDA, DERECHA, SALTAR, DISPARAR };
enum class ModoRei{ MECHA , SOLDADO };
class Rei : public Entidad{
private:
    int danioBase;

    ModoRei modo;
    int vida;
    double velocidadBase;
    int vidaMax;
    int direccion;
    bool izquierdaPresionada;
    bool derechaPresionada;
    bool disparando;
    bool saltoSolicitado;
    bool enSuelo;
    MovParabolico* salto;
    OndaEmpuje* empuje;
    double calor;
    double calorMax;
    double calorPorDisparo;
    bool bloqueadaPorCalor;
    double tiempoBloqueoCalor;
    double tiempoBloqueoGolpe;
    double tiempoInvulnerable;
    double cadenciaBase;
    double cooldownDisparo;
    bool bateriaActiva;
    double tiempoBateria;
    double multCadenciaBateria;
    double multVelocidadBateria;
    bool escudoActivo;
    double tiempoEscudo;
    double multVelocidadEscudo;
    double velocidadSalto;
    double anguloSalto;
    double gravedad;
    double duracionInvulnerabilidad;
    double tiempoRecargaCalor;
public:
    Rei(ModoRei modo, const ParamsComunes& comunes, const ParamsNivel1* p1, double x, double y);
    ~Rei() override;
    void actualizar(double dt) override;
    void procesarEntrada(Tecla tecla, bool presionada);
    void mover(int dir, double dt);
    void saltar();
    void actualizarSalto(double dt);
    void apoyarEn(double ySuperficie);
    void iniciarCaida();
    Proyectil* disparar();
    bool puedeDisparar() const;
    void actualizarCalor(double dt);
    void activarBateria(double duracion, double multCadencia, double multVelocidad);
    void activarEscudo(double duracion, double multVelocidad);
    void actualizarEfectos(double dt);
    double calcularVelocidadEfectiva() const;
    void aplicarEmpuje(OndaEmpuje* onda);
    bool recibirDanio(int n) override;
    void matar() override;
    bool estaViva() const override;

    int getVidas() const;
    double getCalorRelativo() const;
    bool estaBloqueadaPorCalor() const;
    bool tieneEscudo() const;
    bool tieneBateria() const;
    bool estaDisparando() const;
};

#endif // REI_H
