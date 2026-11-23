#include "nivel.h"
#include "datos.h"
#include <stdexcept>

Nivel::Nivel(ModoDificultad modo):dificultad(modo),rei(nullptr),tiempoTranscurrido(0),tiempoLimite(0){
    cargarParametrosComunes();
}

Nivel::~Nivel(){
    delete rei;
    for(auto e:entidades){
        delete e;
    }
    for(auto o:obstaculos){
        delete o;
    }
    for(auto i:items){
        delete i;
    }
}

void Nivel::cargarParametrosComunes(){
    if(dificultad==ModoDificultad::NORMAL){
        comunes.anguloSalto=ParametrosComunes::ANGULO_SALTO;
        comunes.danioBase=ParametrosComunes::DANIO_BASE;
        comunes.gravedad=ParametrosComunes::GRAVEDAD;
        comunes.invulnerabilidad=ParametrosComunes::INVULNERABILIDAD;
        comunes.velocidadMecha=ParametrosComunes::VELOCIDAD_MECHA;
        comunes.velocidadSalto=ParametrosComunes::VELOCIDAD_SALTO;
        comunes.velocidadSoldado=ParametrosComunes::VELOCIDAD_SOLDADO;
        comunes.vidasRei=ParametrosComunes::VIDAS_REI;
    }
    else{
        comunes.anguloSalto=comunesDificil::ANGULO_SALTO;
        comunes.danioBase=comunesDificil::DANIO_BASE;
        comunes.gravedad=comunesDificil::GRAVEDAD;
        comunes.invulnerabilidad=comunesDificil::INVULNERABILIDAD;
        comunes.velocidadMecha=comunesDificil::VELOCIDAD_MECHA;
        comunes.velocidadSalto=comunesDificil::VELOCIDAD_SALTO;
        comunes.velocidadSoldado=comunesDificil::VELOCIDAD_SOLDADO;
        comunes.vidasRei=comunesDificil::VIDAS_REI;
    }
    validarParametros();
}

void Nivel::validarParametros(){
    if(comunes.velocidadMecha<=0||comunes.velocidadSoldado<=0||comunes.velocidadSalto<=0){
        throw invalid_argument("Velocidades incorrectas");
    }
    if(comunes.vidasRei<=0||comunes.danioBase<=0||comunes.invulnerabilidad<=0){
        throw invalid_argument("Vidas, danio o invulnerabilidad incorrectos");
    }
    if(comunes.gravedad<=0||comunes.anguloSalto<=0||comunes.anguloSalto>=90){
        throw invalid_argument("Gravedad o angulo de salto incorrectos");
    }
}

void Nivel::agregarEntidad(Entidad* e){
    if(e!=nullptr){
        entidades.push_back(e);
    }
}

void Nivel::agregarObstaculo(Obstaculo* o){
    if(o!=nullptr){
        obstaculos.push_back(o);
    }
}

void Nivel::agregarItem(Item* i){
    if(i!=nullptr){
        items.push_back(i);
    }
}


void Nivel::detectarColisiones(){
    for(auto i: entidades){
        if(i->estaViva() && i->colisionaCon(*rei)){
            int vidaAntes=rei->getVidas();
            i->alTocarRei(*rei,comunes.danioBase);
            if(rei->getVidas()<vidaAntes){
                RegistrarEvento(TipoEvento::REI_DANIO);
            }
        }
    }
    for(auto i: obstaculos){
        if(i->estaViva() && i->colisionaCon(*rei)){
            int vidaAntes=rei->getVidas();
            i->alTocarRei(*rei,comunes.danioBase);
            if(rei->getVidas()<vidaAntes){
                RegistrarEvento(TipoEvento::REI_DANIO);
            }
        }
    }
    for(auto i: items){
        if(i->estaViva() && i->colisionaCon(*rei)){
            i->alTocarRei(*rei,comunes.danioBase);
            if(!i->estaViva()){
                RegistrarEvento(TipoEvento::ITEM);
            }
        }
    }
}

void Nivel::eliminarInactivos(){
    for(size_t k=0;k<entidades.size();){
        if(!entidades[k]->estaViva()){
            delete entidades[k];
            entidades.erase(entidades.begin()+k);
        }
        else{
            k++;
        }
    }
    for(size_t k=0;k<items.size();){
        if(!items[k]->estaViva()){
            delete items[k];
            items.erase(items.begin()+k);
        }
        else{
            k++;
        }
    }
}

void Nivel::actualizarTemporizador(double dt){
    this->tiempoTranscurrido+=dt;
}

double Nivel::TiempoRestante(){
    if(this->tiempoLimite==0){
        return -1;
    }
    double restante=this->tiempoLimite-this->tiempoTranscurrido;
    if(restante<0){
        return 0;
    }
    return restante;
}

void Nivel::RegistrarEvento(TipoEvento tipo){
    eventos.push_back(tipo);
}

vector<TipoEvento> Nivel::tomarEventos(){
    vector<TipoEvento> newEvent=eventos;
    eventos.clear();
    return newEvent;
}

vector<Entidad*> Nivel::obtenerEntidadesDibujables() {
    vector<Entidad*> dibujas;
    dibujas.reserve(entidades.size() + obstaculos.size() + items.size() + 1);
    if (rei != nullptr) {
        dibujas.push_back(rei);
    }
    dibujas.insert(dibujas.end(), entidades.begin(), entidades.end());
    dibujas.insert(dibujas.end(), obstaculos.begin(), obstaculos.end());
    dibujas.insert(dibujas.end(), items.begin(), items.end());

    return dibujas;
}

double Nivel::getCamaraX(){
    return 0;
}

double Nivel::getVidaJefeRelativa(){
    return -1;
}

Rei* Nivel::getRei(){
    return this->rei;
}

ModoDificultad Nivel::getDificultad(){
    return this->dificultad;
}

const ParamsComunes& Nivel::getComunes(){
    return comunes;
}