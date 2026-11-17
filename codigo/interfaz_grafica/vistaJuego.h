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
#include <vector>
#include "nucleo/juego.h"
#include "entidades/entidad.h"
#include "interfaz_grafica/panelEstado.h"
#include "interfaz_grafica/spriteEntidad.h"

enum class IdMusica { MENU, PAUSA, NIVEL1, NIVEL2 };
class VistaJuego: public QGraphicsView{
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
    VistaJuego(Juego* juego, QWidget* parent = nullptr);
    ~VistaJuego();
    void iniciar();
    void detener();
    void keyPressEvent(QKeyEvent* evento) override;
    void keyReleaseEvent(QKeyEvent* evento) override;
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
signals:
    void nivelTerminado(bool gano);
    void pausaSolicitada();
    void pantallaCompletaSolicitada();

private slots:
    void ciclo();
};

#endif // VISTAJUEGO_H
