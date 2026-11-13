#include "entidad.h"
#include "entidad.h"
#include "entidades/rei.h"

Entidad::Entidad(double x, double y, double ancho, double alto, TipoSprite tipo)
    : x(x), y(y), ancho(ancho), alto(alto), activa(true), fisica(nullptr), tipoSprite(tipo) {}

Entidad::~Entidad(){
    delete fisica;
}

bool Entidad::recibirDanio(int n){
    return false;
}

void Entidad::matar(){
    this->activa=false;
}

bool Entidad::estaViva() const{
    return this->activa;
}

void Entidad::alTocarRei(Rei& rei, int danio){
}

bool Entidad::colisionaCon(const Entidad& otra) const{
    if (this->x < otra.x + otra.ancho && this->x + this->ancho > otra.x && this->y < otra.y + otra.alto && this->y + this->alto > otra.y){
        return true;
    }
    return false;
}

void Entidad::setFisica(Fisica* f){
    delete fisica;
    this->fisica=f;
}

void Entidad::desactivar(){
    this->activa=false;
}

void Entidad::setPosicion(double x,double y){
    this->x=x;
    this->y=y;
}

double Entidad::getX() const{
    return this->x;
}
double Entidad::getY() const{
    return this->y;
}
double Entidad::getAncho() const{
    return this->ancho;
}
double Entidad::getAlto() const{
    return this->alto;
}
TipoSprite Entidad::getTipoSprite() const{
    return this->tipoSprite;
}
Fisica* Entidad::getFisica() const{
    return this->fisica;
}
