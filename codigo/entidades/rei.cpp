#include "rei.h"
#include "nucleo/nivel.h"
#include "nucleo/nivel1.h"

// p1 es nullptr en el nivel 2, donde Rei no dispara ni usa calor
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
    this->tiempoDisparoVisual=0;
    this->tiempoDanioVisual=0;

    this->direccion=1;
    this->izquierdaPresionada=false;
    this->derechaPresionada=false;
    this->disparando=false;
    this->saltoSolicitado=false;
    this->agachadaPresionada=false;
    this->agachada=false;
    this->altoDePie=this->alto;
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
        // Sin parametros de nivel 1: calorMax=1 evita dividir por cero
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

// Orden por frame: direccion, agachado, movimiento, salto, empuje y efectos
void Rei::actualizar(double dt){

    int dir=0;
    if(derechaPresionada){
        dir++;
    }
    if(izquierdaPresionada){
        dir--;
    }
    if(dir!=0){
        this->direccion=dir;   // conserva la ultima direccion al soltar las teclas
    }
    this->actualizarAgachado();
    this->mover(dir,dt);
    if(saltoSolicitado){
        this->saltar();
    }
    this->actualizarSalto(dt);

    // La onda de una bomba desplaza a Rei un poco cada frame hasta extinguirse
    if(empuje!=nullptr){
        empuje->actualizar(dt);
        this->x += empuje->getDelta();
        if(empuje->haTerminado()){
            delete empuje;
            empuje = nullptr;
        }
    }
    this->actualizarEfectos(dt);
}

// Solo guarda el estado de las teclas; la accion ocurre en actualizar()
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
    else if(tecla==Tecla::AGACHARSE){
        this->agachadaPresionada=presionada;
    }
}

// Solo se agacha en el suelo. Al cambiar el alto se ajusta y para que los pies no se muevan.
void Rei::actualizarAgachado(){
    const bool quiere = agachadaPresionada && enSuelo && !saltoSolicitado;
    if(quiere && !agachada){
        const double nuevoAlto = altoDePie * Agachado::FACTOR_ALTO;
        this->y += this->alto - nuevoAlto;
        this->alto = nuevoAlto;
        this->agachada = true;
    }
    else if(!quiere && agachada){
        this->y -= altoDePie - this->alto;
        this->alto = altoDePie;
        this->agachada = false;
    }
}
bool Rei::estaAgachada() const{
    return agachada;
}

// Alto del sprite: siempre el de pie, aunque la caja de colision este agachada
double Rei::getAltoVisual() const{
    return altoDePie;
}

void Rei::mover(int dir, double dt){
    this->x+=dir*dt*calcularVelocidadEfectiva();
}

// Salto: tiro parabolico con la velocidad y angulo de los parametros
void Rei::saltar(){
    if(!enSuelo){
        return;
    }
    this->enSuelo=false;
    if(salto!=nullptr){
        delete salto;
    }
    salto =new MovParabolico(x,y);
    salto->setParametros(velocidadSalto, anguloSalto, gravedad, 1);
}
void Rei::actualizarSalto(double dt){
    if(enSuelo||salto==nullptr){
        return;
    }
    salto->actualizar(dt);
    this->y=salto->getY();   // solo toma la y: el movimiento horizontal lo controla el jugador
}

// El nivel la llama al detectar una superficie bajo los pies
void Rei::apoyarEn(double ySuperficie){
    this->y=ySuperficie-alto;
    this->enSuelo=true;
    delete salto;
    salto=nullptr;
}

// Caida libre (sin velocidad inicial) al perder el suelo
void Rei::iniciarCaida(){
    if(enSuelo){
        this->enSuelo=false;
        delete salto;
        salto=new MovParabolico(x,y);

        salto->setParametros(0, 0, gravedad, 1);
    }
}

// Crea la bala si puede disparar. Cada disparo suma calor; al llenarse, el arma se bloquea un tiempo.
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
    // La bateria acorta el tiempo entre disparos
    cooldownDisparo = bateriaActiva ? (cadenciaBase / multCadenciaBateria) : cadenciaBase;

    double xBala = (direccion > 0) ? x + ancho : x;
    Proyectil* bala = new Proyectil(xBala, y + alto / 4.0, TipoProyectil::RECTO, Propietario::REI, danioBase);
    bala->setVelocidad(direccion * 800.0, 0.0);
    tiempoDisparoVisual = Animacion::DURACION_DISPARO_VISUAL;
    return bala;
}
bool Rei::puedeDisparar() const{
    if (calor >= calorMax||bloqueadaPorCalor||cooldownDisparo>0) {
        return false;
    }
    return true;
}

// Bloqueada: espera y se vacia el calor de golpe. Si no, se enfria poco a poco.
void Rei::actualizarCalor(double dt) {
    if (bloqueadaPorCalor) {
        tiempoBloqueoCalor -= dt;
        if (tiempoBloqueoCalor <= 0.0) {
            tiempoBloqueoCalor = 0.0;
            bloqueadaPorCalor = false;
            calor = calorMax-1;
        }
    } else if (calor > 0.0) {
        const double tasaEnfriamiento = 5.0;   // unidades de calor por segundo
        calor -= tasaEnfriamiento * dt;
        if (calor < 0.0) {
            calor = 0.0;
        }
    }
}

// Bateria: dispara mas rapido pero camina mas lento
void Rei:: activarBateria(double duracion, double multCadencia, double multVelocidad){
    this->tiempoBateria=duracion;
    this->multCadenciaBateria=multCadencia;
    this->multVelocidadBateria=multVelocidad;
    this->bateriaActiva=true;

}
// Gaseosa: escudo contra daño pero camina mas lento
void Rei::activarEscudo(double duracion, double multVelocidad){
    this->escudoActivo=true;
    this->tiempoEscudo=duracion;
    this->multVelocidadEscudo=multVelocidad;
}

// Hace avanzar todos los temporizadores; cada uno se detiene en 0
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

    if(tiempoDisparoVisual>0){
        tiempoDisparoVisual-=dt;
        if(tiempoDisparoVisual<0){
            tiempoDisparoVisual=0;
        }
    }
    if(tiempoDanioVisual>0){
        tiempoDanioVisual-=dt;
        if(tiempoDanioVisual<0){
            tiempoDanioVisual=0;
        }
    }
}

// Los multiplicadores de agacharse, bateria y escudo se acumulan
double Rei:: calcularVelocidadEfectiva() const{
    double v=velocidadBase;
    if(agachada){
        v*=Agachado::FACTOR_VELOCIDAD;
    }
    if(bateriaActiva){
        v*=multVelocidadBateria;
    }
    if(escudoActivo){
        v*=multVelocidadEscudo;
    }
    return v;
}

// Rei se queda con la onda y la libera al terminar (ver actualizar)
void Rei:: aplicarEmpuje(OndaEmpuje* onda){
    if(empuje!=onda){
        delete empuje;
        empuje=onda;
    }
}

// Devuelve false si no hubo daño (escudo o invulnerabilidad). Un golpe tambien bloquea el disparo un rato.
bool Rei:: recibirDanio(int n){
    if(escudoActivo||tiempoInvulnerable>0){
        return false;
    }
    this->vida-=n;
    if(vida<0){
        vida=0;
    }
    tiempoInvulnerable=duracionInvulnerabilidad;
    tiempoDanioVisual=Animacion::DURACION_DANIO_VISUAL;

    if(tiempoBloqueoGolpe > cooldownDisparo){
        cooldownDisparo = tiempoBloqueoGolpe;
    }
    return true;
}

// Muerte instantanea (fosas)
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

// Calor entre 0 y 1, para la barra del HUD
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

// Prioridad de animaciones: daño, aire, agachada, disparo, correr, quieta
EstadoVisual Rei::getEstadoVisual() const{
    if(tiempoDanioVisual>0){
        return EstadoVisual::DANIADO;
    }
    if(!enSuelo){
        return EstadoVisual::SALTANDO;
    }
    if(agachada){                                        // agachada se ve agachada aunque dispare
        return EstadoVisual::AGACHADO;
    }
    if(tiempoDisparoVisual>0 && modo==ModoRei::MECHA){
        return EstadoVisual::DISPARANDO;
    }
    if(izquierdaPresionada!=derechaPresionada){          // exactamente una tecla de direccion
        return EstadoVisual::CORRIENDO;
    }
    return EstadoVisual::QUIETO;
}

bool Rei::miraDerecha() const{
    return direccion==1;
}
