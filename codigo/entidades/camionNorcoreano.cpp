#include "camionNorcoreano.h"
#include "datos.h"
#include "entidades/rei.h"

// AVANCE empieza yendo a la izquierda; PATRULLA, a la derecha
CamionNorcoreano::CamionNorcoreano(double x,double y,ModoCamion modo,double velocidad)
    : Entidad(x, y, Tamanio::CAMION_ANCHO, Tamanio::CAMION_ALTO, TipoSprite::CAMION),
    modo(modo), velocidad(velocidad), limiteIzq(0), limiteDer(0),
    direccion(modo == ModoCamion::AVANCE ? -1 : 1) {}
void CamionNorcoreano:: configurarPatrulla	(double izq, double der){
    this->limiteIzq=izq;
    this->limiteDer=der;
}
void CamionNorcoreano::actualizar(double dt){
    this->x+=velocidad*dt*direccion;
    if(this->modo==ModoCamion::PATRULLA){
        // Al llegar a un limite se queda dentro y da la vuelta
        if(x+ancho>limiteDer){
            x=limiteDer-ancho;
            this->direccion=-1;
        }
        if(x<limiteIzq){
            x=limiteIzq+1;
            this->direccion=1;
        }
    }
    else{
        // Al salir de la pantalla se desactiva para que el nivel lo elimine
        if(x>Tamanio::ANCHO_PANTALLA){
            x=Tamanio::ANCHO_PANTALLA;
            desactivar();
        }
        if(x+ancho<0){
            x=0;
            desactivar();
        }
    }

}

// El camion hace daño y desaparece al chocar
void CamionNorcoreano:: alTocarRei	(Rei& rei, int danio){
    rei.recibirDanio(danio);
    desactivar();
}
ModoCamion CamionNorcoreano:: getModo()const{
    return modo;
}
CamionNorcoreano::~CamionNorcoreano(){}

bool CamionNorcoreano::miraDerecha() const{
    return direccion==1;
}
