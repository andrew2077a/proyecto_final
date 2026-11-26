#include "nivel2.h"
#include "datos.h"
#include <QFile>
#include <QTextStream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
Nivel2::Nivel2(ModoDificultad modo):Nivel(modo){
    bombas.reserve(50);

    cargarParametros();
    this->tiempoLimite=params.tiempoLimite;
    this->camaraX=0;
    this->anchoMundo=Nivel2Mundo::ANCHO_MUNDO;
    this->ySuelo=Nivel2Mundo::Y_SUELO;
    // Los temporizadores cuentan hacia atras: la primera bomba y la primera bala salen tras un intervalo completo
    this->temporizadorBomba=params.intervaloBomba;
    this->temporizadorBala=params.intervaloBalaLateral;
    srand(time(NULL));
}

// Copia las constantes de Nivel2Normal o Nivel2Dificil (datos.h)
void Nivel2::cargarParametros(){
    if(this->dificultad==ModoDificultad::NORMAL){
        params.tiempoLimite=Nivel2Normal::TIEMPO_LIMITE;
        params.intervaloBomba=Nivel2Normal::INTERVALO_BOMBA;
        params.numBombas=Nivel2Normal::NUM_BOMBAS;
        params.rangoCaidaBomba=Nivel2Normal::RANGO_CAIDA_BOMBA;
        params.radioDanioBomba=Nivel2Normal::RADIO_DANIO_BOMBA;
        params.radioEmpujeBomba=Nivel2Normal::RADIO_EMPUJE_BOMBA;
        params.fuerzaEmpuje=Nivel2Normal::FUERZA_EMPUJE;
        params.amortiguamientoEmpuje=Nivel2Normal::AMORTIGUAMIENTO_EMPUJE;
        params.intervaloBalaLateral=Nivel2Normal::INTERVALO_BALA_LATERAL;
        params.numBalasLaterales=Nivel2Normal::NUM_BALAS_LATERALES;
        params.velocidadBala=Nivel2Normal::VELOCIDAD_BALA;
        params.velocidadCamionPatrulla=Nivel2Normal::VELOCIDAD_CAMION_PATRULLA;
        params.duracionGaseosa=Nivel2Normal::DURACION_GASEOSA;
        params.multVelocidadGaseosa=Nivel2Normal::MULT_VELOCIDAD_GASEOSA;
        params.esperaPuas=Nivel2Normal::ESPERA_PUAS;
    }
    else{
        params.tiempoLimite=Nivel2Dificil::TIEMPO_LIMITE;
        params.intervaloBomba=Nivel2Dificil::INTERVALO_BOMBA;
        params.numBombas=Nivel2Dificil::NUM_BOMBAS;
        params.rangoCaidaBomba=Nivel2Dificil::RANGO_CAIDA_BOMBA;
        params.radioDanioBomba=Nivel2Dificil::RADIO_DANIO_BOMBA;
        params.radioEmpujeBomba=Nivel2Dificil::RADIO_EMPUJE_BOMBA;
        params.fuerzaEmpuje=Nivel2Dificil::FUERZA_EMPUJE;
        params.amortiguamientoEmpuje=Nivel2Dificil::AMORTIGUAMIENTO_EMPUJE;
        params.intervaloBalaLateral=Nivel2Dificil::INTERVALO_BALA_LATERAL;
        params.numBalasLaterales=Nivel2Dificil::NUM_BALAS_LATERALES;
        params.velocidadBala=Nivel2Dificil::VELOCIDAD_BALA;
        params.velocidadCamionPatrulla=Nivel2Dificil::VELOCIDAD_CAMION_PATRULLA;
        params.duracionGaseosa=Nivel2Dificil::DURACION_GASEOSA;
        params.multVelocidadGaseosa=Nivel2Dificil::MULT_VELOCIDAD_GASEOSA;
        params.esperaPuas=Nivel2Dificil::ESPERA_PUAS;
    }

}

void Nivel2:: iniciar(){
    if(rei!=nullptr){
        delete rei;
    }
    rei=new Rei(ModoRei::SOLDADO,comunes,nullptr,Nivel2Mundo::REI_INICIO_X,
                  Nivel2Mundo::Y_SUELO-Tamanio::REI_SOLDADO_ALTO);
    construirMundo();
}

// En este nivel los items se colocan al construir el mundo (ver construirMundo)
void Nivel2::colocarItems(){}

// Lee mundo.txt. Cada linea empieza con el tipo de objeto seguido de sus numeros:
//   BLOQUE x y ancho alto     FOSA x y ancho alto     PUAS x y
//   CAMION x y limiteIzq limiteDer     GASEOSA x y
// Las lineas que empiezan con otra cosa (como los comentarios con #) se ignoran.
void Nivel2::construirMundo(){
    obstaculos.reserve(60);
    entidades.reserve(60);
    items.reserve(20);
    QFile archivo(Rutas::ARCHIVO_NIVEL2);
    if(!archivo.open(QIODevice::ReadOnly | QIODevice::Text)){
        throw runtime_error("No se pudo abrir el archivo del mundo");
    }
    QTextStream texto(&archivo);
    while(!texto.atEnd()){
        QString linea=texto.readLine();
        QTextStream flujoLinea(&linea);
        QString tipo;
        double x=0,y=0,ancho=0,alto=0,izq=0,der=0;
        flujoLinea>>tipo;
        if(tipo=="BLOQUE"){
            flujoLinea>>x>>y>>ancho>>alto;
            agregarObstaculo(new Obstaculo(TipoSprite::BLOQUE,x,y,ancho,alto,0));
        }
        else if(tipo=="FOSA"){
            flujoLinea>>x>>y>>ancho>>alto;
            agregarObstaculo(new Obstaculo(TipoSprite::FOSA,x,y,ancho,alto,0));
        }
        else if(tipo=="PUAS"){
            flujoLinea>>x>>y;
            agregarObstaculo(new Obstaculo(TipoSprite::PUAS,x,y,Tamanio::PUAS_ANCHO,
                                           Tamanio::PUAS_ALTO,params.esperaPuas));
        }
        else if(tipo=="CAMION"){
            flujoLinea>>x>>y>>izq>>der;
            CamionNorcoreano* camion = new CamionNorcoreano(x, y, ModoCamion::PATRULLA,params.velocidadCamionPatrulla);
            camion->configurarPatrulla(izq, der);
            agregarEntidad(camion);
        }
        else if(tipo=="GASEOSA"){
            flujoLinea>>x>>y;
            agregarItem(new Item(TipoItem::GASEOSA,x,y,Tamanio::GASEOSA_ANCHO,Tamanio::GASEOSA_ALTO,
                                 params.duracionGaseosa,params.multVelocidadGaseosa,1));
        }
    }
}

void Nivel2:: actualizar(double dt){
    rei->actualizar(dt);

    // Choque lateral con bloques: si los pies estan mas de 60 px por debajo del borde
    // superior del bloque no es un aterrizaje, asi que se empuja a Rei hacia afuera
    for(auto o : obstaculos){
        if(o->esSolido() && rei->colisionaCon(*o) &&
            rei->getY() + rei->getAlto() > o->getY() + 60){
            double cRei = rei->getX() + rei->getAncho() / 2.0;
            double cBloque = o->getX() + o->getAncho() / 2.0;
            if(cRei < cBloque){
                rei->setPosicion(o->getX() - rei->getAncho(), rei->getY());
            } else {
                rei->setPosicion(o->getX() + o->getAncho(), rei->getY());
            }
        }
    }
    // Limites del mundo
    if(rei->getX() < 0){
        rei->setPosicion(0, rei->getY());
    }
    if(rei->getX() + rei->getAncho() > anchoMundo){
        rei->setPosicion(anchoMundo - rei->getAncho(), rei->getY());
    }

    // Suelo: Rei aterriza sobre la superficie que tenga debajo; si no hay
    // ninguna (fosa) o se va por el borde de un bloque, empieza a caer.
    // Los pies quedan 20 px por debajo del borde superior de la superficie.
    double ySup;
    if(superficieBajo(*rei,ySup)){
        double pies=rei->getY()+rei->getAlto();
        if(pies>=ySup+20){
            rei->apoyarEn(ySup+20);
        }
        else if(pies<ySup-1){
            rei->iniciarCaida();
        }
    }
    else{
        rei->iniciarCaida();
    }
    for(auto i:obstaculos){
        i->actualizar(dt);
    }
    for(auto e:entidades){
        e->actualizar(dt);
    }
    for(auto e:items){
        e->actualizar(dt);
    }
    actualizarTemporizador(dt);
    detectarColisiones();

    // Balas y bombas salen cuando su temporizador llega a cero
    this->temporizadorBomba-=dt;
    this->temporizadorBala-=dt;
    if (temporizadorBala<=0){
        lanzarBalaLateral();
        temporizadorBala=params.intervaloBalaLateral;
    }
    if( temporizadorBomba<=0){
        soltarBomba();
        temporizadorBomba=params.intervaloBomba;
    }
    // La bomba tambien explota si choca con un bloque antes de llegar al suelo
    for(auto b : bombas){
        if(!b->haExplotado()){
            for(auto o : obstaculos){
                if(o->esSolido() && b->colisionaCon(*o)){
                    b->explotar();
                    break;
                }
            }
        }
    }
    procesarExplosiones();
    actualizarScroll();
    eliminarInactivos();
}

// Pierde si Rei muere o se acaba el tiempo; gana al llegar a la meta
ResultadoNivel Nivel2:: verificarFin(){
    if(!rei->estaViva()||TiempoRestante()==0){
        return ResultadoNivel::PERDIDO;
    }
    else if(rei->getX()+rei->getAncho()>=Nivel2Mundo::META_X){
        return ResultadoNivel::GANADO;
    }
    return ResultadoNivel::EN_CURSO;
}

// La camara sigue a Rei manteniendola en el centro de la pantalla,
// sin salirse de los bordes del mundo
void Nivel2::actualizarScroll() {
    if (!rei) return;
    double scroll = rei->getX() - (Tamanio::ANCHO_PANTALLA / 2.0);
    if (scroll < 0.0) {
        this->camaraX = 0.0;
        return;
    }
    if (scroll + Tamanio::ANCHO_PANTALLA >= this->anchoMundo) {
        this->camaraX = this->anchoMundo - Tamanio::ANCHO_PANTALLA;
        return;
    }
    this->camaraX = scroll;
}

// Las balas aparecen fuera de la pantalla por la izquierda, a la altura de Rei,
// y avanzan hacia la derecha. Si hay varias, se apilan hacia arriba cada 60 px.
void Nivel2::lanzarBalaLateral(){

    for(int i=0;i<params.numBalasLaterales;i++){
        double y=rei->getY() + rei->getAlto()/2 - Tamanio::BALA_LATERAL_ALTO/2 - i*60;
        Proyectil* proyet =new Proyectil(camaraX - Tamanio::BALA_LATERAL_ANCHO -
                                              Nivel2Mundo::MARGEN_APARICION_BALA,y,TipoProyectil::RECTO, Propietario::ENEMIGO, comunes.danioBase);
        proyet->setVelocidad(params.velocidadBala,0);
        agregarEntidad(proyet);
    }
}

// Las bombas caen en caida libre (velocidad inicial 0 y solo gravedad) desde
// arriba de la pantalla, en una x aleatoria cerca de Rei.
// Sin llamar a setParametros la gravedad quedaria en 0 y no caerian.
void Nivel2:: soltarBomba(){
    for(int i=0;i<params.numBombas;i++){
        int rango = static_cast<int>(params.rangoCaidaBomba);
        double X = rei->getX() + (rand() % (2 * rango + 1)) - rango;
        double Y= Nivel2Mundo::ALTURA_APARICION_BOMBA;
        Proyectil* bomba=new Proyectil(X,Y,TipoProyectil::BOMBA,Propietario::ENEMIGO,comunes.danioBase);
        bomba->configurarBomba(ySuelo,params.radioDanioBomba,params.radioEmpujeBomba,params.fuerzaEmpuje,params.amortiguamientoEmpuje);
        MovParabolico* caida = new MovParabolico(X, Y);
        caida->setParametros(0, 0, comunes.gravedad, 1);
        bomba->setFisica(caida);
        agregarEntidad(bomba);
        bombas.push_back(bomba);
        this->RegistrarEvento(TipoEvento::BOMBA_CAE);
    }
}

// Cuando una bomba explota, el daño y el empuje se aplican una sola vez
// (iniciarExplosion marca estaExplotando). Despues se espera a que termine
// la animacion y la bomba se desactiva.
void Nivel2::procesarExplosiones(){
    for(size_t k=0;k<bombas.size();){
        Proyectil* i=bombas[k];
        if(!i->haExplotado()){
            k++;
            continue;
        }
        if(!i->estaExplotando()){
            if(i->esImpactoDirecto(*rei)){
                if(rei->recibirDanio(comunes.danioBase)){
                    this->RegistrarEvento(TipoEvento::REI_DANIO);
                }
            }
            OndaEmpuje* onda=i->calcularEmpuje(*rei);
            if(onda!=nullptr){
                rei->aplicarEmpuje(onda);
            }
            this->RegistrarEvento(TipoEvento::EXPLOSION);
            i->iniciarExplosion();
        }
        if(i->explosionTerminada()){
            i->desactivar();
            bombas.erase(bombas.begin()+k);
        }
        else{
            k++;
        }
    }
}

// Busca la y de la superficie sobre la que esta Rei. Devuelve false si esta sobre una fosa.
// Un bloque cuenta como superficie si Rei esta encima de el y su borde superior
// no esta a mas de MARGEN px por encima de los pies (asi puede subir escalones
// pequeños). Si hay varios, se elige el mas alto.
bool Nivel2::superficieBajo(const Rei& rei, double& y) const{
    const double pies=rei.getY()+rei.getAlto();
    const double centro=rei.getX()+rei.getAncho()/2.0;
    const double MARGEN=50;
    bool hayBloque=false;
    double techo=ySuelo;
    for(auto i:obstaculos){
        if(i->esSolido()){
            bool encima=rei.getX()<i->getX()+i->getAncho() && rei.getX()+rei.getAncho()>i->getX();
            if(encima && i->getY()>=pies-MARGEN && i->getY()<techo){
                techo=i->getY();
                hayBloque=true;
            }
        }
    }
    if(hayBloque){
        y=techo;
        return true;
    }
    // Sin bloque debajo: solo hay suelo si el centro de Rei no esta sobre una fosa
    for(auto o:obstaculos){
        if(o->getTipoSprite()==TipoSprite::FOSA && centro>o->getX() && centro<o->getX()+o->getAncho()){
            return false;
        }
    }
    y=ySuelo;
    return true;
}

// Solo las balas laterales (RECTO) se descartan al alejarse de la camara.
// Despues se quitan las bombas muertas de "bombas" antes de que Nivel::eliminarInactivos
void Nivel2::eliminarInactivos(){
    for(auto e:entidades){
        Proyectil* p=dynamic_cast<Proyectil*>(e);
        if(p!=nullptr && p->getTipo()==TipoProyectil::RECTO &&
            p->fueraDePantalla(camaraX-500,camaraX+Tamanio::ANCHO_PANTALLA+500,
                               -1000,Tamanio::ALTO_PANTALLA+500)){
            p->desactivar();
        }
    }
    for(size_t k=0;k<bombas.size();){
        if(!bombas[k]->estaViva()){
            bombas.erase(bombas.begin()+k);
        }
        else{
            k++;
        }
    }
    Nivel::eliminarInactivos();
}

double Nivel2::getCamaraX(){
    return camaraX;
}
