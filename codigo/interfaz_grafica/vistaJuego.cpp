#include "vistaJuego.h"
#include "datos.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <QFile>
#include <QPainter>

namespace {
QPixmap cargarFondoEscalado(const char* ruta){
    QPixmap imagen(ruta);
    if(imagen.isNull()){
        throw std::runtime_error(std::string("No se pudo cargar el fondo: ") + ruta);
    }
    return imagen.scaled(static_cast<int>(Tamanio::ANCHO_PANTALLA),
                         static_cast<int>(Tamanio::ALTO_PANTALLA),
                         Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

int indiceFondoNivel2(int tramo){
    const int tramos = static_cast<int>(std::ceil(Nivel2Mundo::ANCHO_MUNDO / Tamanio::ANCHO_PANTALLA));
    if(tramo < 0){
        tramo = 0;
    }
    if(tramo > tramos - 1){
        tramo = tramos - 1;
    }
    return tramo * Rutas::NUM_FONDOS_NIVEL2 / tramos;
}
}

VistaJuego::VistaJuego(Juego* juego, QWidget* parent)
    : QGraphicsView(parent),
    escena(new QGraphicsScene(this)),
    juego(juego),
    musica(new QMediaPlayer(this)),
    salidaAudio(new QAudioOutput(this)),
    pistaActual(IdMusica::MENU),
    volumen(0.5),
    panel(new PanelEstado(escena)),
    fondo(nullptr),
    fondo2(nullptr),
    tramoFondo(-1),
    meta(nullptr)
{
    setScene(escena);
    escena->setSceneRect(0, 0, Tamanio::ANCHO_PANTALLA, Tamanio::ALTO_PANTALLA);
    musica->setAudioOutput(salidaAudio);
    salidaAudio->setVolume(volumen);
    setFocusPolicy(Qt::StrongFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setBackgroundBrush(Qt::black);
    setAlignment(Qt::AlignCenter);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);   // el fondo se mueve: evita restos al hacer scroll

    conectarSenales();

    try {
        cargarRecursosSonido();
    } catch (...) {
        delete panel;
        throw;
    }
}
VistaJuego::~VistaJuego(){
    delete panel;
}
void VistaJuego::iniciar(){
    SpriteEntidad::precargarTodo();
    limpiarSprites();
    panel->mostrar();

    setFocus();
    reloj.start();
    temporizador.start(16);
}
void VistaJuego:: detener(){
    temporizador.stop();
}
void VistaJuego::ciclo(){
    // 1. Tiempo
    double dt = reloj.restart() / 1000.0;
    if(dt > 0.05){
        dt = 0.05;
    }

    // 2. Lógica
    juego->actualizar(dt);

    // 3. Dibujo
    sincronizarSprites(dt);
    panel->actualizar(*juego);

    // 4. Sonido
    reproducirEventos(juego->tomarEventos());

    // 5. Fin del nivel
    EstadoJuego estado = juego->getEstado();
    if(estado == EstadoJuego::GANADO || estado == EstadoJuego::PERDIDO){
        detener();
        emit nivelTerminado(estado == EstadoJuego::GANADO);
    }
}
void VistaJuego::keyPressEvent(QKeyEvent* evento){
    if(evento->isAutoRepeat()){
        return;
    }
    switch (evento->key()) {
    case Qt::Key_A:
        juego->manejarTecla(Tecla::IZQUIERDA,true);
        break;
    case Qt::Key_D:
        juego->manejarTecla(Tecla::DERECHA,true);
        break;
    case Qt::Key_W:
        juego->manejarTecla(Tecla::SALTAR,true);
        break;
    case Qt::Key_Space:
        juego->manejarTecla(Tecla::DISPARAR,true);
        break;
    case Qt::Key_S:
        juego->manejarTecla(Tecla::AGACHARSE,true);
        break;
    case Qt::Key_Escape:
        emit pausaSolicitada();
        break;
    case Qt::Key_F11:
        emit pantallaCompletaSolicitada();
        break;
    default:
        break;
    }
}
void VistaJuego::keyReleaseEvent(QKeyEvent* evento){
    if(evento->isAutoRepeat()){
        return;
    }
    switch (evento->key()) {
    case Qt::Key_A:
        juego->manejarTecla(Tecla::IZQUIERDA,false);
        break;
    case Qt::Key_D:
        juego->manejarTecla(Tecla::DERECHA,false);
        break;
    case Qt::Key_W:
        juego->manejarTecla(Tecla::SALTAR,false);
        break;
    case Qt::Key_Space:
        juego->manejarTecla(Tecla::DISPARAR,false);
        break;
    case Qt::Key_S:
        juego->manejarTecla(Tecla::AGACHARSE,false);
        break;
    case Qt::Key_Escape:
        break;
    case Qt::Key_F11:
        break;
    default:
        break;
    }
}

// Quita todos los sprites de la escena (no hay nivel o se reinicia).
void VistaJuego::limpiarSprites(){
    for(auto it = sprites.begin(); it != sprites.end(); ++it){
        escena->removeItem(it.value());
        delete it.value();
    }
    sprites.clear();
}

void VistaJuego::sincronizarSprites(double dt){
    Nivel* nive = juego->getNivel();


    if(nive == nullptr){
        limpiarSprites();
        return;
    }

    QSet<Entidad*> vivos;
    for(Entidad* e : nive->obtenerEntidadesDibujables()){
        // el monstruo muriendo ya no esta "vivo", pero se sigue dibujando para ver su animacion de muerte
        if(e->estaViva() || e->getEstadoVisual() == EstadoVisual::MURIENDO){
            vivos.insert(e);
        }
    }


    for(auto it = sprites.begin(); it != sprites.end(); ){
        Entidad* dueno = it.key();
        // tambien se descarta si otro objeto nuevo ocupo la misma direccion de memoria (tipo distinto)
        const bool sigue = vivos.contains(dueno) && it.value()->getTipo() == dueno->getTipoSprite();
        if(!sigue){
            escena->removeItem(it.value());
            delete it.value();
            it = sprites.erase(it);
        }
        else{
            ++it;
        }
    }

    const double camaraX = nive->getCamaraX();
    for(Entidad* e : vivos){
        if(!sprites.contains(e)){
            SpriteEntidad* nuevo = new SpriteEntidad(e->getTipoSprite(), e);
            escena->addItem(nuevo);
            sprites.insert(e, nuevo);
        }
        // primero la animacion y luego la posicion (la escala depende del fotograma)
        sprites[e]->actualizarAnimacion(dt, e->getEstadoVisual(), e->miraDerecha());
        // el sprite se escala con el alto "de pie" y se apoya en los pies de la caja (importa con Rei agachada)
        const double altoVisual = e->getAltoVisual();
        const double yVisual = e->getY() + e->getAlto() - altoVisual;
        sprites[e]->actualizarPosicion(e->getX(), yVisual, e->getAncho(), altoVisual, camaraX, e->getRotacion());
    }

    actualizarFondo(camaraX);
    actualizarMeta(camaraX);
}

void VistaJuego::actualizarFondo(double camaraX){
    if(fondo == nullptr || fondo2 == nullptr){
        return;
    }
    const double W = Tamanio::ANCHO_PANTALLA;
    const int tramo = static_cast<int>(camaraX / W);

    // Solo se cambia el pixmap al entrar a otra pantalla, no en cada ciclo
    if(!fondosNivel2.isEmpty() && tramo != tramoFondo){
        tramoFondo = tramo;
        fondo->setPixmap(fondosNivel2[indiceFondoNivel2(tramo)]);
        fondo2->setPixmap(fondosNivel2[indiceFondoNivel2(tramo + 1)]);
    }

    const double desplazamiento = camaraX - tramo * W;
    fondo->setPos(-desplazamiento, 0);
    fondo2->setPos(W - desplazamiento, 0);
}

// La meta del nivel 2 no tenia ninguna imagen: el jugador no sabia a donde ir.
void VistaJuego::actualizarMeta(double camaraX){
    if(meta != nullptr && meta->isVisible()){
        meta->setPos(Nivel2Mundo::META_X - camaraX, Nivel2Mundo::Y_SUELO - Tamanio::META_ALTO);
    }
}

void VistaJuego::cargarFondo(int numNivel){
    fondosNivel2.clear();
    QPixmap primero;
    switch(numNivel){
    case 1:
        primero = cargarFondoEscalado(Rutas::ARCHIVO_FONDO_NIVEL1);
        break;
    case 2:
        for(int i = 0; i < Rutas::NUM_FONDOS_NIVEL2; ++i){
            fondosNivel2.push_back(cargarFondoEscalado(Rutas::FONDOS_NIVEL2[i]));
        }
        primero = fondosNivel2[0];
        break;
    default:
        throw std::invalid_argument("cargarFondo: nivel no valido");
    }

    tramoFondo = -1;    // fuerza a actualizarFondo a poner las imagenes
    if(fondo == nullptr){
        fondo = escena->addPixmap(primero);
        fondo->setZValue(-10);
        fondo->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
        fondo2 = escena->addPixmap(primero);
        fondo2->setZValue(-10);
        fondo2->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    }
    else{
        fondo->setPixmap(primero);
        fondo2->setPixmap(primero);
    }
    fondo2->setVisible(numNivel == 2);
    actualizarFondo(0.0);

    // Bandera de meta (solo nivel 2): esta parte queda igual que la tenias
    if(numNivel == 2){
        if(meta == nullptr){
            const QString ruta = QString(Rutas::CARPETA_SPRITES) + "meta" + Rutas::EXTENSION_SPRITES;
            QPixmap imagenMeta(ruta);
            if(imagenMeta.isNull()){
                throw runtime_error(std::string("No se pudo cargar la meta: ") + ruta.toStdString());
            }
            imagenMeta = imagenMeta.scaled(static_cast<int>(Tamanio::META_ANCHO), static_cast<int>(Tamanio::META_ALTO),
                                           Qt::IgnoreAspectRatio, Qt::FastTransformation);
            meta = escena->addPixmap(imagenMeta);
            meta->setZValue(3);
        }
        meta->setVisible(true);
    }
    else if(meta != nullptr){
        meta->setVisible(false);
    }
}

void VistaJuego::cargarRecursosSonido(){
    //  MUSICAS
    const char* musicas[] = {
        Rutas::MUSICA_MENU,
        Rutas::MUSICA_PAUSA,
        Rutas::MUSICA_NIVEL1,
        Rutas::MUSICA_NIVEL2
    };
    for(const char* ruta : musicas){
        if(!QFile::exists(QString(ruta))){
            throw runtime_error(std::string("No se encontro la musica: ") + ruta);
        }
    }

    // EFECTOS
    const TipoEvento tipos[] = {
        TipoEvento::DISPARO_REI,
        TipoEvento::BOMBA_CAE,
        TipoEvento::MONSTRUO_LANZA,
        TipoEvento::REI_DANIO,
        TipoEvento::EXPLOSION,
        TipoEvento::ITEM
    };
    const char* rutas[] = {
        Rutas::SONIDO_DISPARO_REI,
        Rutas::SONIDO_BOMBA_CAE,
        Rutas::SONIDO_MONSTRUO_LANZA,
        Rutas::SONIDO_REI_DANIO,
        Rutas::SONIDO_EXPLOSION,
        Rutas::SONIDO_ITEM
    };

    for(int i = 0; i < 6; ++i){
        if(!QFile::exists(QString(rutas[i]))){
            throw runtime_error(std::string("No se encontro el sonido: ") + rutas[i]);
        }
        QSoundEffect* efecto = new QSoundEffect(this);   // Qt lo libera junto con la vista
        efecto->setSource(QUrl(QString("qrc") + rutas[i]));
        efecto->setVolume(volumen);
        efectos.insert(tipos[i], efecto);
    }
}
void VistaJuego::reproducirMusica(IdMusica id){

    if(id == pistaActual && musica->playbackState() == QMediaPlayer::PlayingState){
        return;
    }
    switch (id) {
    case IdMusica::MENU:
        musica->setSource(QUrl(QString("qrc") + Rutas::MUSICA_MENU));
        musica->setLoops(QMediaPlayer::Infinite);
        musica->play();
        break;
    case IdMusica::PAUSA:
        musica->setSource(QUrl(QString("qrc") + Rutas::MUSICA_PAUSA));
        musica->setLoops(QMediaPlayer::Infinite);
        musica->play();
        break;
    case IdMusica::NIVEL1:
        musica->setSource(QUrl(QString("qrc") + Rutas::MUSICA_NIVEL1));
        musica->setLoops(QMediaPlayer::Infinite);
        musica->play();
        break;
    case IdMusica::NIVEL2:
        musica->setSource(QUrl(QString("qrc") + Rutas::MUSICA_NIVEL2));
        musica->setLoops(QMediaPlayer::Infinite);
        musica->play();
        break;
    default:
        throw runtime_error(string("No se encontro el sonido: "));
        break;
    }
    pistaActual = id;
}
void VistaJuego::pausarMusica(){
    musica->pause();
}
void VistaJuego:: reanudarMusica(){
    musica->play();
}
void VistaJuego:: detenerMusica(){
    musica->stop();
}

void VistaJuego::reproducirEventos(const vector<TipoEvento>& eventos){
    for(auto i:eventos){
        switch (i) {
        case TipoEvento::BOMBA_CAE:
            efectos[TipoEvento::BOMBA_CAE]->play();
            break;
        case TipoEvento::DISPARO_REI:
            efectos[TipoEvento::DISPARO_REI]->play();
            break;
        case TipoEvento::EXPLOSION:
            efectos[TipoEvento::EXPLOSION]->play();
            break;
        case TipoEvento::ITEM:
            efectos[TipoEvento::ITEM]->play();
            break;
        case TipoEvento::MONSTRUO_LANZA:
            efectos[TipoEvento::MONSTRUO_LANZA]->play();
            break;
        case TipoEvento::REI_DANIO:
            efectos[TipoEvento::REI_DANIO]->play();
            break;
        default:
            throw runtime_error(string("No se encontro el sonido: "));
            break;
        }
    }
}
void VistaJuego::setVolumen(double v) {
    // 1. Asegura que v este estrictamente entre 0.0 y 1.0
    if (v < 0.0){
        v = 0.0;
    }
    if (v > 1.0){
        v = 1.0;
    }
    volumen = v;
    // 2. Ajusta la musica a traves de su salida de audio
    if (salidaAudio != nullptr) {
        salidaAudio->setVolume(static_cast<float>(volumen));
    }
    // 3. Ajusta cada efecto de sonido cargado
    for (auto* efecto : as_const(efectos)) {
        if (efecto != nullptr) {
            efecto->setVolume(volumen);
        }
    }
}
void VistaJuego::ajustarEscala() {
    fitInView(0, 0, Tamanio::ANCHO_PANTALLA, Tamanio::ALTO_PANTALLA, Qt::KeepAspectRatio);
}
void VistaJuego::conectarSenales(){
    connect(&temporizador, &QTimer::timeout, this, &VistaJuego::ciclo);
}
