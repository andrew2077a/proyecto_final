#include "obstaculo.h"
#include "entidades/rei.h"

Obstaculo::Obstaculo(TipoSprite tipo, double x, double y, double ancho, double alto, double espera)
    :Entidad(x,y,ancho,alto,tipo),espera(espera),esperaActual(0){}

Obstaculo::~Obstaculo(){}
void Obstaculo::alTocarRei(Rei& rei,int danio){
    if(esperaActual==0){
        switch (tipoSprite) {
        case TipoSprite::PUAS:
            rei.recibirDanio(danio);
            this->esperaActual=espera;
            break;
        case TipoSprite::FOSA:
            rei.matar();
            break;
        default:
            break;
        }
    }
}
void Obstaculo::actualizar(double dt){
    if(tipoSprite==TipoSprite::PUAS&&esperaActual>0){
        this->esperaActual-=dt;
        if(esperaActual<0){
            this->esperaActual=0;
        }
    }
}

bool Obstaculo::esSolido()const{
    return tipoSprite==TipoSprite::BLOQUE;
}


