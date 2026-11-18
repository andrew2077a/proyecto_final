#ifndef JUEGO_H
#define JUEGO_H
#include "nucleo/nivel.h"

enum class EstadoJuego {MENU , JUGANDO, PAUSA, GANADO , PERDIDO};
class Juego{
private:
    EstadoJuego estado;
    Nivel* nivelActual;
    int numNivel;
    ModoDificultad dificultad;
    void terminarNivel(bool gano);
    bool finPendiente;
    bool ganoPendiente;
    double esperaFin;
public:
    Juego();
    ~Juego();
    void configurar(ModoDificultad modo);
    void iniciarNivel(int num);
    void actualizar(double dt);
    void manejarTecla( Tecla tecla, bool presionada);
    void pausar();
    void reanudar();
    void reiniciar();
    void volverAlMenu();
    vector<TipoEvento> tomarEventos();
    EstadoJuego getEstado()const;
    Nivel* getNivel()const;
    int getNumNivel()const;
    ModoDificultad getDificultad()const;
};

#endif // JUEGO_H