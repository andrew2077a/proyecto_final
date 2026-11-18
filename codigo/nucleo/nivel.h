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
enum class TipoEvento {DISPARO_REI, BOMBA_CAE, MONSTRUO_LANZA, REI_DANIO, EXPLOSION, ITEM};
enum class ResultadoNivel {EN_CURSO, GANADO, PERDIDO};
struct ParamsComunes{
    int vidasRei;
    int danioBase;
    double invulnerabilidad;
    double gravedad;
    double velocidadMecha;
    double velocidadSoldado;
    double velocidadSalto;
    double anguloSalto;

};
class Nivel{
protected:
    ModoDificultad dificultad;
    ParamsComunes comunes;
    Rei* rei;
    vector<Entidad*> entidades;
    vector<Obstaculo*> obstaculos;
    vector<Item*> items;
    vector<TipoEvento> eventos;
    double tiempoTranscurrido;
    double tiempoLimite;

    void cargarParametrosComunes();
    void validarParametros();
    virtual void colocarItems()=0;
    void agregarEntidad(Entidad* e);
    void agregarObstaculo(Obstaculo* o);
    void agregarItem(Item* i);
    CamionNorcoreano* crearCamion(double x, double y, ModoCamion modo, double velocidad);
    void detectarColisiones();
    virtual void eliminarInactivos();
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
    virtual double getCamaraX();
    virtual double getVidaJefeRelativa();
    Rei* getRei();
    ModoDificultad getDificultad();
    const ParamsComunes& getComunes();
};

#endif // NIVEL_H