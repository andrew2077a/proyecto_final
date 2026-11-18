#include "juego.h"
#include "datos.h"
#include "nucleo/nivel1.h"
#include "nucleo/nivel2.h"
#include <stdexcept>

Juego::Juego():estado(EstadoJuego::MENU),nivelActual(nullptr),numNivel(0),dificultad(ModoDificultad::NORMAL),
    finPendiente(false),ganoPendiente(false),esperaFin(0) {}

Juego::~Juego(){
    delete nivelActual;
}

void Juego::configurar(ModoDificultad modo){
    this->dificultad=modo;
}

void Juego::iniciarNivel(int num){
    if(num!=1 && num!=2){
        throw invalid_argument("Nivel no encontrado");
    }
    if(nivelActual!=nullptr){
        delete nivelActual;
        nivelActual=nullptr;
    }
    this->finPendiente=false;
    this->ganoPendiente=false;
    this->esperaFin=0;
    switch (num) {
    case 1:
        nivelActual=new Nivel1(dificultad);
        this->numNivel=num;
        this->estado=EstadoJuego::JUGANDO;
        nivelActual->iniciar();
        break;
    case 2:
        nivelActual=new Nivel2(dificultad);
        this->numNivel=num;
        this->estado=EstadoJuego::JUGANDO;
        nivelActual->iniciar();
        break;
    default:
        throw invalid_argument("Nivel no encontrado");
        break;
    }
}

void Juego::actualizar(double dt){
    if(this->estado==EstadoJuego::JUGANDO && nivelActual!=nullptr){
        if(finPendiente){
            this->esperaFin-=dt;
            if(esperaFin<=0){
                terminarNivel(ganoPendiente);
            }
        }
        else{
            nivelActual->actualizar(dt);
            ResultadoNivel ju=nivelActual->verificarFin();
            if(ju!=ResultadoNivel::EN_CURSO){
                this->ganoPendiente=false;
                if(ju==ResultadoNivel::GANADO){
                    this->ganoPendiente=true;
                }
                this->finPendiente=true;
                this->esperaFin=Tamanio::ESPERA_FIN;
            }
        }
    }
}

void Juego::manejarTecla(Tecla tecla, bool presionada){
    if(nivelActual!=nullptr && nivelActual->getRei()!=nullptr){
        nivelActual->getRei()->procesarEntrada(tecla,presionada);
    }
}

void Juego::pausar(){
    if(this->estado==EstadoJuego::JUGANDO){
        this->estado=EstadoJuego::PAUSA;
    }
}

void Juego::reanudar(){
    if(this->estado==EstadoJuego::PAUSA){
        this->estado=EstadoJuego::JUGANDO;
    }
}

void Juego::terminarNivel(bool gano){
    delete nivelActual;
    nivelActual=nullptr;
    this->finPendiente=false;
    if(gano){
        this->estado=EstadoJuego::GANADO;
    }
    else{
        this->estado=EstadoJuego::PERDIDO;
    }
}

void Juego::reiniciar(){
    iniciarNivel(numNivel);
}

void Juego::volverAlMenu(){
    if(nivelActual!=nullptr){
        delete nivelActual;
        nivelActual=nullptr;
    }
    this->finPendiente=false;
    this->estado=EstadoJuego::MENU;
}

vector<TipoEvento> Juego::tomarEventos(){
    if(nivelActual==nullptr){
        return {};
    }
    return nivelActual->tomarEventos();
}

EstadoJuego Juego::getEstado()const{
    return this->estado;
}

Nivel* Juego::getNivel()const{
    return this->nivelActual;
}

int Juego::getNumNivel()const{
    return this->numNivel;
}

ModoDificultad Juego::getDificultad()const{
    return this->dificultad;
}