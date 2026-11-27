#ifndef JUEGO_H
#define JUEGO_H
#include "nucleo/nivel.h"

enum class EstadoJuego {MENU , JUGANDO, PAUSA, GANADO , PERDIDO};

// Controla la partida: crea y destruye el nivel, lo avanza en cada ciclo y
// recibe el teclado.
class Juego{
private:
    EstadoJuego estado;
    Nivel* nivelActual;   // Juego es su dueño; nullptr si no hay partida en curso
    int numNivel;
    ModoDificultad dificultad;
    void terminarNivel(bool gano);

    // Al terminar el nivel se espera esperaFin segundos (para ver la animacion
    // final) antes de cambiar de estado
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
