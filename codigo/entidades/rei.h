#ifndef REI_H
#define REI_H
#include "entidades/entidad.h"
#include "fisicas/movParabolico.h"
#include "fisicas/ondaEmpuje.h"
#include "entidades/proyectil.h"
#include "datos.h"

struct ParamsComunes;
struct ParamsNivel1;

enum class Tecla { IZQUIERDA, DERECHA, SALTAR, DISPARAR, AGACHARSE };
// MECHA: nivel 1 (dispara). SOLDADO: nivel 2 (no dispara).
enum class ModoRei{ MECHA , SOLDADO };

// Personaje del jugador
class Rei : public Entidad{
private:
    int danioBase;
    ModoRei modo;

    // Vida
    int vida;
    int vidaMax;
    double tiempoInvulnerable;        // restante tras recibir daño
    double duracionInvulnerabilidad;

    // Movimiento y entrada
    double velocidadBase;
    int direccion;                    // -1 izquierda, 1 derecha (ultima direccion usada)
    bool izquierdaPresionada;
    bool derechaPresionada;
    bool disparando;
    bool saltoSolicitado;

    // Agacharse: la caja baja de alto pero los pies siguen en el suelo
    bool agachadaPresionada;
    bool agachada;
    double altoDePie;                 // alto original, para volver a levantarse

    // Salto y caida
    bool enSuelo;
    MovParabolico* salto;             // fisica activa mientras esta en el aire
    double velocidadSalto;
    double anguloSalto;
    double gravedad;

    // Empuje de las bombas del nivel 2
    OndaEmpuje* empuje;               // Rei es su dueña y la libera al terminar

    // Disparo y calor (nivel 1)
    double calor;
    double calorMax;
    double calorPorDisparo;
    bool bloqueadaPorCalor;
    double tiempoBloqueoCalor;
    double tiempoRecargaCalor;
    double tiempoBloqueoGolpe;        // tiempo sin poder disparar tras un golpe
    double cadenciaBase;              // segundos entre disparos
    double cooldownDisparo;

    // Items
    bool bateriaActiva;
    double tiempoBateria;
    double multCadenciaBateria;
    double multVelocidadBateria;
    bool escudoActivo;
    double tiempoEscudo;
    double multVelocidadEscudo;

    // Temporizadores de las animaciones de disparo y daño
    double tiempoDisparoVisual;
    double tiempoDanioVisual;
public:
    Rei(ModoRei modo, const ParamsComunes& comunes, const ParamsNivel1* p1, double x, double y);
    ~Rei() override;
    void actualizar(double dt) override;
    void procesarEntrada(Tecla tecla, bool presionada);
    void mover(int dir, double dt);
    void saltar();
    void actualizarSalto(double dt);
    void apoyarEn(double ySuperficie);
    void actualizarAgachado();
    bool estaAgachada() const;
    double getAltoVisual() const override;
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
    EstadoVisual getEstadoVisual() const override;
    bool miraDerecha() const override;
};

#endif // REI_H
