#include "rei.h"
#include "nucleo/nivel.h"
#include "nucleo/nivel1.h"



Rei::Rei(ModoRei modo, const ParamsComunes& comunes, const ParamsNivel1* p1, double x, double y)
    : Entidad(x, y, modo == ModoRei::MECHA ? Tamanio::REI_MECHA_ANCHO : Tamanio::REI_SOLDADO_ANCHO,
              modo == ModoRei::MECHA ? Tamanio::REI_MECHA_ALTO : Tamanio::REI_SOLDADO_ALTO,
              modo == ModoRei::MECHA ? TipoSprite::REI_MECHA : TipoSprite::REI_SOLDADO),
    modo(modo){
    if(modo==ModoRei::SOLDADO){
        this->velocidadBase=comunes.velocidadSoldado;
        this->velocidadSalto=comunes.velocidadSalto;
    }
    else{
        this->velocidadBase=comunes.velocidadMecha;
        this->velocidadSalto=comunes.velocidadSalto;
    }
    this->vida=comunes.vidasRei;
    this->vidaMax=comunes.vidasRei;
    this->danioBase=comunes.danioBase;
    this->anguloSalto=comunes.anguloSalto;
    this->gravedad=comunes.gravedad;
    this->duracionInvulnerabilidad=comunes.invulnerabilidad;
    this->tiempoInvulnerable=0;

    this->direccion=1;
    this->izquierdaPresionada=false;
    this->derechaPresionada=false;
    this->disparando=false;
    this->saltoSolicitado=false;
    this->enSuelo=true;
    this->salto=nullptr;
    this->empuje=nullptr;

    this->bloqueadaPorCalor=false;
    this->tiempoBloqueoCalor=0;
    this->cooldownDisparo=0;
    this->calor=0;

    this->bateriaActiva=false;
    this->tiempoBateria=0;
    this->multCadenciaBateria=1;
    this->multVelocidadBateria=1;
    this->escudoActivo=false;
    this->tiempoEscudo=0;
    this->multVelocidadEscudo=1;

    if(p1!=nullptr){
        this->calorMax=p1->calorMax;
        this->calorPorDisparo=p1->calorPorDisparo;
        this->tiempoRecargaCalor=p1->tiempoRecargaCalor;
        this->tiempoBloqueoGolpe=p1->tiempoBloqueoGolpe;
        this->cadenciaBase=p1->cadenciaBase;
    }
    else{
        this->calorMax=1;
        this->calorPorDisparo=0;
        this->tiempoRecargaCalor=0;
        this->tiempoBloqueoGolpe=0;
        this->cadenciaBase=0;
    }
}

Rei::~Rei(){
    delete salto;
    delete empuje;
}
void Rei::actualizar(double dt){

    int dir=0;
    if(derechaPresionada){
        dir++;
    }
    if(izquierdaPresionada){
        dir--;
    }
    if(dir!=0){
        this->direccion=dir;
    }
    this->mover(dir,dt);
    if(saltoSolicitado){
        this->saltar();
    }
    this->actualizarSalto(dt);
    if(empuje!=nullptr){
        this->aplicarEmpuje(empuje);
    }
    this->actualizarEfectos(dt);
}
void Rei:: procesarEntrada(Tecla tecla, bool presionada){
    if(tecla==Tecla::DERECHA){
        this->derechaPresionada=presionada;
    }
    else if(tecla==Tecla::IZQUIERDA){
        this->izquierdaPresionada=presionada;
    }
    else if(tecla==Tecla::SALTAR){
        this->saltoSolicitado=presionada;
    }
    else if(tecla==Tecla::DISPARAR){
        this->disparando=presionada;
    }
}

void Rei::mover(int dir, double dt){
    this->x+=dir*dt*calcularVelocidadEfectiva();
}

void Rei::saltar(){
    if(!enSuelo){
        return;
    }
    this->enSuelo=false;
    if(salto!=nullptr){
        delete salto;
    }
    salto =new MovParabolico(x,y);
}
void Rei::actualizarSalto(double dt){
    if(enSuelo||salto==nullptr){
        return;
    }
    salto->actualizar(dt);
    this->y=salto->getY();
}
void Rei::apoyarEn(double ySuperficie){
    this->y=ySuperficie-alto;
    this->enSuelo=true;
    delete salto;
    salto=nullptr;
}
void Rei::iniciarCaida(){
    if(enSuelo){
        this->enSuelo=false;
        delete salto;
        salto=new MovParabolico(x,y);
    }
}
Proyectil* Rei::disparar() {
    if (!puedeDisparar()) {
        return nullptr;
    }

    calor += calorPorDisparo;
    if (calor >= calorMax) {
        calor = calorMax;
        bloqueadaPorCalor = true;
        tiempoBloqueoCalor = tiempoRecargaCalor;
    }
    cooldownDisparo = bateriaActiva ? (cadenciaBase / multCadenciaBateria) : cadenciaBase;

    double xBala = (direccion > 0) ? x + ancho : x;
    Proyectil* bala = new Proyectil(xBala, y + alto / 2.0, TipoProyectil::RECTO, Propietario::REI, danioBase);
    bala->setVelocidad(direccion * 800.0, 0.0);
    return bala;
}
bool Rei::puedeDisparar() const{
    if (calor >= calorMax||bloqueadaPorCalor||cooldownDisparo>0) {
        return false;
    }
    return true;
}

void Rei::actualizarCalor(double dt) {
    if (bloqueadaPorCalor) {
        tiempoBloqueoCalor -= dt;
        if (tiempoBloqueoCalor <= 0.0) {
            tiempoBloqueoCalor = 0.0;
            bloqueadaPorCalor = false;
            calor = 0.0;
        }
    } else if (calor > 0.0) {
        const double tasaEnfriamiento = 5.0;
        calor -= tasaEnfriamiento * dt;
        if (calor < 0.0) {
            calor = 0.0;
        }
    }
}
void Rei:: activarBateria(double duracion, double multCadencia, double multVelocidad){
    this->tiempoBateria=duracion;
    this->multCadenciaBateria=multCadencia;
    this->multVelocidadBateria=multVelocidad;
    this->bateriaActiva=true;

}
void Rei::activarEscudo(double duracion, double multVelocidad){
    this->escudoActivo=true;
    this->tiempoEscudo=duracion;
    this->multVelocidadEscudo=multVelocidad;
}

void Rei::actualizarEfectos(double dt){
    actualizarCalor(dt);
    if(cooldownDisparo>0){
        cooldownDisparo-=dt;
        if(cooldownDisparo<0){
            cooldownDisparo=0;
        }
    }
    if(bateriaActiva){
        tiempoBateria-=dt;
        if(tiempoBateria<=0){
            tiempoBateria=0;
            bateriaActiva=false;
            multCadenciaBateria=1;
            multVelocidadBateria=1;
        }
    }
    if(escudoActivo){
        tiempoEscudo-=dt;
        if(tiempoEscudo<=0){
            tiempoEscudo=0;
            escudoActivo=false;
            multVelocidadEscudo=1;
        }
    }
    if(tiempoInvulnerable>0){
        tiempoInvulnerable-=dt;
        if(tiempoInvulnerable<0){
            tiempoInvulnerable=0;
        }
    }
}
double Rei:: calcularVelocidadEfectiva() const{
    double v=velocidadBase;
    if(bateriaActiva){
        v*=multVelocidadBateria;
    }
    if(escudoActivo){
        v*=multVelocidadEscudo;
    }
    return v;
}

void Rei:: aplicarEmpuje(OndaEmpuje* onda){
    if(empuje!=onda){
        delete empuje;
        empuje=onda;
    }
}
bool Rei:: recibirDanio(int n){
    if(escudoActivo||tiempoInvulnerable>0){
        return false;
    }
    this->vida-=n;
    if(vida<0){
        vida=0;
    }
    tiempoInvulnerable=duracionInvulnerabilidad;
    return true;
}
void Rei::matar(){
    vida=0;
    desactivar();

}
bool Rei::estaViva()const{
    if(this->vida>0&&this->activa){
        return true;
    }
    return false;
}
int Rei::getVidas() const{
    return vida;
}

double Rei::getCalorRelativo() const{
    if(calorMax<=0){
        return 0;
    }
    return calor/calorMax;
}
bool Rei:: estaBloqueadaPorCalor() const{
    return bloqueadaPorCalor;
}
bool Rei::tieneEscudo() const{
    return escudoActivo;
}
bool  Rei:: tieneBateria() const{
    return bateriaActiva;
}
bool  Rei:: estaDisparando() const{
    return disparando;
}
