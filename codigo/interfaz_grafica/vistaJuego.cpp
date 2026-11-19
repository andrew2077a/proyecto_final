#include "vistaJuego.h"
#include "datos.h"

VistaJuego::VistaJuego(Juego* juego, QWidget* parent)
    : QGraphicsView(parent),
    escena(new QGraphicsScene(this)),
    juego(juego),
    musica(new QMediaPlayer(this)),
    salidaAudio(new QAudioOutput(this)),
    pistaActual(IdMusica::MENU),
    volumen(0.5),
    panel(new PanelEstado()),
    fondo(nullptr)
{
    setScene(escena);
    escena->setSceneRect(0, 0, Tamanio::ANCHO_PANTALLA, Tamanio::ALTO_PANTALLA);
    musica->setAudioOutput(salidaAudio);
    salidaAudio->setVolume(volumen);
    connect(&temporizador, &QTimer::timeout, this, &VistaJuego::ciclo);
}

VistaJuego::~VistaJuego(){
    delete panel;
}
void VistaJuego:: iniciar(){
    temporizador.start();
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
    sincronizarSprites();
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
void VistaJuego:: sincronizarSprites(){

}
class VistaJuego: public QGraphicsView {
    Q_OBJECT
private:
    QGraphicsScene* escena;
    QTimer temporizador;
    QElapsedTimer reloj;
    Juego* juego;
    QMediaPlayer* musica;
    QAudioOutput* salidaAudio;
    QMap<TipoEvento, QSoundEffect*> efectos;
    IdMusica pistaActual;
    double volumen;
    PanelEstado* panel;
    QHash<Entidad*, SpriteEntidad*> sprites;
    QGraphicsPixmapItem* fondo;

public:
    void sincronizarSprites();
    void cargarFondo(int numNivel);
    void cargarRecursosSonido();
    void reproducirMusica(IdMusica id);
    void pausarMusica();
    void reanudarMusica();
    void detenerMusica();
    void reproducirEventos(const vector<TipoEvento>& eventos);
    void setVolumen(double v);
    void ajustarEscala();
    void conectarSenales();

private slots:
    void ciclo();
};
