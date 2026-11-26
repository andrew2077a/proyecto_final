#ifndef NIVEL_H
#define NIVEL_H
#include <vector>
#include "entidades/rei.h"
#include "entidades/entidad.h"
#include "entidades/camionNorcoreano.h"
#include "obstaculos_e_items/obstaculo.h"
#include "obstaculos_e_items/item.h"
using namespace std;
enum class ModoDificultad {NORMAL, DIFICIL};

// Sucesos que la vista convierte en sonido
enum class TipoEvento {DISPARO_REI, BOMBA_CAE, MONSTRUO_LANZA, REI_DANIO, EXPLOSION, ITEM};
enum class ResultadoNivel {EN_CURSO, GANADO, PERDIDO};

// Parametros que dependen de la dificultad y son iguales en los dos niveles
struct ParamsComunes{
    int vidasRei;
    int danioBase;
    double invulnerabilidad;   // segundos sin recibir daño tras un golpe
    double gravedad;
    double velocidadMecha;
    double velocidadSoldado;
    double velocidadSalto;
    double anguloSalto;        // grados

};

// Clase base de los niveles. Guarda lo que tienen en comun (Rei, listas de
// objetos, colisiones, tiempo, eventos de sonido) y deja a cada nivel su
// propia dinamica. Es dueño de Rei y de todo lo que hay en las tres listas.
class Nivel{
protected:
    ModoDificultad dificultad;
    ParamsComunes comunes;
    Rei* rei;
    vector<Entidad*> entidades;     // enemigos y proyectiles
    vector<Obstaculo*> obstaculos;
    vector<Item*> items;
    vector<TipoEvento> eventos;     // cola de sonidos pendientes
    double tiempoTranscurrido;
    double tiempoLimite;            // 0 = el nivel no tiene limite de tiempo

    void cargarParametrosComunes();
    void validarParametros();
    virtual void colocarItems()=0;
    void agregarEntidad(Entidad* e);
    void agregarObstaculo(Obstaculo* o);
    void agregarItem(Item* i);
    void detectarColisiones();
    virtual void eliminarInactivos();   // las clases hijas lo extienden y llaman a este al final
    void actualizarTemporizador(double dt);
    void RegistrarEvento(TipoEvento tipo);

public:
    Nivel(ModoDificultad modo);
    virtual ~Nivel();
    virtual void iniciar()=0;
    virtual void actualizar(double dt)=0;
    virtual ResultadoNivel verificarFin()=0;

    double TiempoRestante();
    vector<TipoEvento> tomarEventos();
    virtual vector<Entidad*> obtenerEntidadesDibujables();
    virtual double getCamaraX();            // 0 si el nivel no tiene scroll
    virtual double getVidaJefeRelativa();   // -1 si el nivel no tiene jefe
    Rei* getRei();
    ModoDificultad getDificultad();
    const ParamsComunes& getComunes();
};

#endif // NIVEL_H
