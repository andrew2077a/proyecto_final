#include "panelEstado.h"
#include "datos.h"
#include <QPixmap>
#include <QFont>
#include <QPen>
#include <QBrush>

namespace {
// Posiciones en coordenadas logicas (pantalla de 1920 x 1080)
constexpr int MAX_ICONOS_VIDA = ParametrosComunes::VIDAS_REI;   // 5, el maximo entre Normal y Dificil
constexpr double VIDA_X = 30;
constexpr double VIDA_Y = 20;
constexpr double VIDA_SEPARACION = 50;
constexpr int VIDA_TAMANIO = 40;

constexpr double TIEMPO_X = 860;
constexpr double TIEMPO_Y = 15;

constexpr double CALOR_X = 30;
constexpr double CALOR_Y = 80;
constexpr double CALOR_ANCHO_MAX = 300;
constexpr double CALOR_ALTO = 20;

constexpr double JEFE_X = 660;
constexpr double JEFE_Y = 70;
constexpr double JEFE_ANCHO_MAX = 600;
constexpr double JEFE_ALTO = 24;

constexpr double EFECTOS_X = 30;
constexpr double EFECTOS_Y = 110;

constexpr double CAPA_HUD = 100;   // por encima de los sprites
}

PanelEstado::PanelEstado(QGraphicsScene* escena)
    : escena(escena), visible(true)
{
    // Iconos de vida (imagen provisional: cuadro rojo)
    QPixmap pixVida(VIDA_TAMANIO, VIDA_TAMANIO);
    pixVida.fill(Qt::red);
    for(int i = 0; i < MAX_ICONOS_VIDA; ++i){
        QGraphicsPixmapItem* icono = escena->addPixmap(pixVida);
        icono->setPos(VIDA_X + i * VIDA_SEPARACION, VIDA_Y);
        icono->setZValue(CAPA_HUD);
        iconosVida.push_back(icono);
    }

    // Texto del tiempo
    textoTiempo = escena->addText("Tiempo: 00:00");
    textoTiempo->setDefaultTextColor(Qt::white);
    textoTiempo->setFont(QFont("Arial", 20, QFont::Bold));
    textoTiempo->setPos(TIEMPO_X, TIEMPO_Y);
    textoTiempo->setZValue(CAPA_HUD);

    // Barra de calor (empieza vacia; actualizar() le cambia el ancho)
    barraCalor = escena->addRect(CALOR_X, CALOR_Y, 0, CALOR_ALTO,QPen(Qt::NoPen), QBrush(QColor(255, 140, 0)));
    barraCalor->setZValue(CAPA_HUD);

    // Barra de vida del jefe
    barraJefe = escena->addRect(JEFE_X, JEFE_Y, JEFE_ANCHO_MAX, JEFE_ALTO,QPen(Qt::NoPen), QBrush(Qt::red));
    barraJefe->setZValue(CAPA_HUD);

    // Texto de efectos activos
    textoEfectos = escena->addText("");
    textoEfectos->setDefaultTextColor(Qt::yellow);
    textoEfectos->setFont(QFont("Arial", 16, QFont::Bold));
    textoEfectos->setPos(EFECTOS_X, EFECTOS_Y);
    textoEfectos->setZValue(CAPA_HUD);
}

void PanelEstado::actualizar(const Juego& juego){
    if(!visible){
        return;
    }

    Nivel* nivel = juego.getNivel();
    if(nivel == nullptr){
        return;
    }
    Rei* rei = nivel->getRei();
    if(rei == nullptr){
        return;
    }

    // ---------- 1. VIDAS ----------
    // Un icono por vida: se muestran los primeros "vidas" iconos y se ocultan los demas.
    int vidas = rei->getVidas();
    for(int i = 0; i < iconosVida.size(); ++i){
        iconosVida[i]->setVisible(i < vidas);
    }

    // ---------- 2. TIEMPO ----------
    // TiempoRestante() devuelve -1 si el nivel no tiene limite (nivel 1).
    double restante = nivel->TiempoRestante();
    if(restante < 0){
        textoTiempo->setVisible(false);
    }
    else{
        textoTiempo->setVisible(true);
        int segundosTotales = static_cast<int>(restante);
        int minutos = segundosTotales / 60;
        int segundos = segundosTotales % 60;
        // %1 y %2 se reemplazan con arg(); el 2 y el '0' rellenan con ceros (ej. "03:07").
        textoTiempo->setPlainText(QString("Tiempo: %1:%2").arg(minutos, 2, 10, QChar('0')).arg(segundos, 2, 10, QChar('0')));
    }

    // ---------- 3. CALOR ----------
    // Solo existe en el nivel 1. Es una proporcion entre 0 y 1; el ancho
    // de la barra es el ancho maximo multiplicado por esa proporcion.
    if(juego.getNumNivel() == 1){
        barraCalor->setVisible(true);
        barraCalor->setRect(CALOR_X, CALOR_Y,
                            CALOR_ANCHO_MAX * rei->getCalorRelativo(), CALOR_ALTO);
    }
    else{
        barraCalor->setVisible(false);
    }

    // ---------- 4. VIDA DEL JEFE ----------
    // getVidaJefeRelativa() devuelve -1 en los niveles sin jefe (nivel 2).
    double vidaJefe = nivel->getVidaJefeRelativa();
    if(vidaJefe < 0){
        barraJefe->setVisible(false);
    }
    else{
        barraJefe->setVisible(true);
        barraJefe->setRect(JEFE_X, JEFE_Y, JEFE_ANCHO_MAX * vidaJefe, JEFE_ALTO);
    }

    // ---------- 5. EFECTOS ACTIVOS ----------
    // Se arma un texto con los efectos que esten activos; vacio si no hay ninguno.
    QString efectos;
    if(rei->tieneBateria()){
        efectos += "Bateria  ";
    }
    if(rei->tieneEscudo()){
        efectos += "Escudo";
    }
    textoEfectos->setPlainText(efectos);
}

void PanelEstado::mostrar(){
    this->visible = true;
    for(int i = 0; i < iconosVida.size(); ++i){
        iconosVida[i]->setVisible(true);
    }
    textoTiempo->setVisible(true);
    barraCalor->setVisible(true);
    barraJefe->setVisible(true);
    textoEfectos->setVisible(true);
}

void PanelEstado::ocultar(){
    this->visible = false;
    for(int i = 0; i < iconosVida.size(); ++i){
        iconosVida[i]->setVisible(false);
    }
    textoTiempo->setVisible(false);
    barraCalor->setVisible(false);
    barraJefe->setVisible(false);
    textoEfectos->setVisible(false);
}