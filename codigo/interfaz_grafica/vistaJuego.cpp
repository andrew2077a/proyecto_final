#include "vistaJuego.h"
#include "datos.h"
#include <algorithm>
#include <QFile>

VistaJuego::VistaJuego(Juego* juego, QWidget* parent)
    : QGraphicsView(parent),
    escena(new QGraphicsScene(this)),
    juego(juego),
    musica(new QMediaPlayer(this)),
    salidaAudio(new QAudioOutput(this)),
    pistaActual(IdMusica::MENU),
    volumen(0.5),
    panel(new PanelEstado(escena)),
    fondo(nullptr)
{
    setScene(escena);
    escena->setSceneRect(0, 0, Tamanio::ANCHO_PANTALLA, Tamanio::ALTO_PANTALLA);
    musica->setAudioOutput(salidaAudio);
    salidaAudio->setVolume(volumen);
    setFocusPolicy(Qt::StrongFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
    for(auto it = sprites.begin(); it != sprites.end(); ++it){
        escena->removeItem(it.value());
        delete it.value();
    }
    sprites.clear();

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
    case Qt::Key_Escape:
        break;
    case Qt::Key_F11:
        break;
    default:
        break;
    }
}

void VistaJuego::sincronizarSprites(double dt){   // CAMBIO: recibe dt
    Nivel* nive = juego->getNivel();

    // (el bloque "if(nive == nullptr){ ... }" queda exactamente igual)

    QSet<Entidad*> vivos;
    for(Entidad* e : nive->obtenerEntidadesDibujables()){
        // CAMBIO: el monstruo muriendo ya no está "vivo", pero se sigue dibujando
        // para que se vea su animación de muerte durante ESPERA_FIN
        if(e->estaViva() || e->getEstadoVisual() == EstadoVisual::MURIENDO){
            vivos.insert(e);
        }
    }

    for(Entidad* e : vivos){
        if(!sprites.contains(e)){
            SpriteEntidad* nuevo = new SpriteEntidad(e->getTipoSprite(), e);
            escena->addItem(nuevo);
            sprites.insert(e, nuevo);
        }
        // CAMBIO: primero la animación y luego la posición (la escala depende del fotograma)
        sprites[e]->actualizarAnimacion(dt, e->getEstadoVisual(), e->miraDerecha());
        sprites[e]->actualizarPosicion(e->getX(), e->getY(), e->getAncho(), e->getAlto(), nive->getCamaraX());
    }

    // (el paso 3, quitar sprites de entidades que ya no están, queda igual)
}

void VistaJuego::cargarFondo(int numNivel){
    const char* ruta = nullptr;
    switch(numNivel){
    case 1:
        ruta = Rutas::ARCHIVO_FONDO_NIVEL1;
        break;
    case 2:
        ruta = Rutas::ARCHIVO_FONDO_NIVEL2;
        break;
    default:
        throw std::invalid_argument("cargarFondo: nivel no valido");
    }

    // Cargar la imagen; si el archivo no esta en el .qrc, la imagen queda nula
    QPixmap imagen(ruta);
    if(imagen.isNull()){
        throw runtime_error(std::string("No se pudo cargar el fondo: ") + ruta);
    }

    // Escalar UNA sola vez a la resolucion logica (no en cada ciclo)
    imagen = imagen.scaled(static_cast<int>(Tamanio::ANCHO_PANTALLA),static_cast<int>(Tamanio::ALTO_PANTALLA),Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if(fondo == nullptr){
        // Primera vez: crear el item y dejarlo detras de todo
        fondo = escena->addPixmap(imagen);
        fondo->setZValue(-1);                                   // debajo de sprites (0) y HUD (100)
        fondo->setPos(0, 0);
        fondo->setCacheMode(QGraphicsItem::DeviceCoordinateCache);  // cache del fondo, como dice el diseño
    }
    else{
        // Ya existia (cambio de nivel): solo se reemplaza la imagen
        fondo->setPixmap(imagen);
    }
}

void VistaJuego::cargarRecursosSonido(){
    // ---------- MUSICAS ----------
    // Solo se comprueba que existan; reproducirMusica() elige cual poner.
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

    // ---------- EFECTOS ----------
    // tipos[i] se asocia con rutas[i]: un QSoundEffect por cada evento.
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