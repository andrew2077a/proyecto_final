#ifndef VISTAJUEGO_H
#define VISTAJUEGO_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QMap>
#include <QHash>
#include <QSet>
#include <QPixmap>
#include <vector>
#include "nucleo/juego.h"
#include "entidades/entidad.h"
#include "interfaz_grafica/panelEstado.h"
#include "interfaz_grafica/spriteEntidad.h"
#include <QVector>

enum class IdMusica { MENU, PAUSA, NIVEL1, NIVEL2 };

// Pantalla del juego: cada ciclo avanza la logica y dibuja lo que esta
// devuelve (sprites, HUD, fondo y sonidos). Tambien lee el teclado.
class VistaJuego: public QGraphicsView{
    Q_OBJECT
private:
    QGraphicsScene* escena;
    QTimer temporizador;     // dispara ciclo() cada 16 ms
    QElapsedTimer reloj;     // mide el tiempo real entre ciclos
    Juego* juego;            // pertenece a Menu
    QMediaPlayer* musica;
    QAudioOutput* salidaAudio;
    QMap<TipoEvento, QSoundEffect*> efectos;
    IdMusica pistaActual;
    double volumen;          // entre 0 y 1
    PanelEstado* panel;
    QHash<Entidad*, SpriteEntidad*> sprites;   // un sprite por entidad dibujada
    QGraphicsPixmapItem* fondo;

    QGraphicsPixmapItem* fondo2;      // segundo fondo para el scroll del nivel 2
    QVector<QPixmap> fondosNivel2;
    int tramoFondo;                   // tramo de pantalla (1920 px) que muestra el scroll
    QGraphicsPixmapItem* meta;

public:
    VistaJuego(Juego* juego, QWidget* parent = nullptr);
    ~VistaJuego();
    void iniciar();
    void detener();
    void keyPressEvent(QKeyEvent* evento) override;
    void keyReleaseEvent(QKeyEvent* evento) override;
    void sincronizarSprites(double dt);
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
private:
    void limpiarSprites();
    void actualizarFondo(double camaraX);
    void actualizarMeta(double camaraX);
signals:
    void nivelTerminado(bool gano);
    void pausaSolicitada();
    void pantallaCompletaSolicitada();

private slots:
    void ciclo();
};

#endif // VISTAJUEGO_H
