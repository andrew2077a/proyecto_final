#include "panelEstado.h"
#include "datos.h"
#include <algorithm>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QGraphicsDropShadowEffect>

namespace {
// Posiciones en coordenadas logicas (pantalla de 1920 x 1080)
constexpr int MAX_ICONOS_VIDA = ParametrosComunes::VIDAS_REI;   // el maximo entre Normal y Dificil
constexpr double CAPA_HUD = 100;   // por encima de los sprites (1-8) y del fondo (-10)

// Placa izquierda: vidas, calor y efectos
constexpr double PLACA_X = 20;
constexpr double PLACA_Y = 16;
constexpr double PLACA_ANCHO = 360;
constexpr double PLACA_ALTO = 176;

constexpr double VIDA_X = 38;
constexpr double VIDA_Y = 28;
constexpr double VIDA_SEPARACION = 64;
constexpr int VIDA_ANCHO = 52;
constexpr int VIDA_ALTO = 48;

constexpr double CALOR_ROTULO_Y = 84;
constexpr double CALOR_X = 38;
constexpr double CALOR_Y = 116;
constexpr double CALOR_ANCHO_MAX = 310;
constexpr double CALOR_ALTO = 22;

constexpr double EFECTOS_X = 34;
constexpr double EFECTOS_Y = 142;

// Reloj (nivel 2), centrado arriba
constexpr double TIEMPO_CENTRO_X = Tamanio::ANCHO_PANTALLA / 2.0;
constexpr double TIEMPO_Y = 22;
constexpr double PLACA_TIEMPO_ANCHO = 280;
constexpr double PLACA_TIEMPO_ALTO = 64;

// Barra del jefe (nivel 1), centrada arriba
constexpr double JEFE_ANCHO_MAX = 960;
constexpr double JEFE_ALTO = 26;
constexpr double JEFE_X = (Tamanio::ANCHO_PANTALLA - JEFE_ANCHO_MAX) / 2.0;
constexpr double JEFE_Y = 62;
constexpr double PLACA_JEFE_ANCHO = JEFE_ANCHO_MAX + 60;
constexpr double PLACA_JEFE_ALTO = 86;

const QColor COLOR_TEXTO(191, 247, 224);      // el mismo verde de los menus
const QColor COLOR_PLACA(6, 26, 28, 175);
const QColor COLOR_BORDE(191, 247, 224, 170);

// Dibuja el corazon con una curva de Bezier, sin usar imagen
QPixmap crearCorazon(bool lleno) {
    QPixmap pix(VIDA_ANCHO, VIDA_ALTO);
    pix.fill(Qt::transparent);
    QPainter pintor(&pix);
    pintor.setRenderHint(QPainter::Antialiasing);
    const double sx = VIDA_ANCHO / 44.0;
    const double sy = VIDA_ALTO / 40.0;
    pintor.scale(sx, sy);
    QPainterPath ruta;
    ruta.moveTo(22, 37);
    ruta.cubicTo(-4, 20, 5, -2, 22, 10);
    ruta.cubicTo(39, -2, 48, 20, 22, 37);
    ruta.closeSubpath();
    if (lleno) {
        pintor.setPen(QPen(QColor(90, 0, 12), 2.5));
        pintor.setBrush(QColor(232, 44, 62));
    } else {
        pintor.setPen(QPen(QColor(120, 130, 135), 2.5));
        pintor.setBrush(QColor(30, 40, 42, 170));
    }
    pintor.drawPath(ruta);
    if (lleno) {
        pintor.setPen(Qt::NoPen);
        pintor.setBrush(QColor(255, 255, 255, 110));
        pintor.drawEllipse(QPointF(12, 12), 3.5, 2.5);   // brillo
    }
    return pix;
}

QGraphicsPathItem* crearPlaca(QGraphicsScene* escena, double x, double y, double ancho, double alto) {
    QPainterPath ruta;
    ruta.addRoundedRect(QRectF(x, y, ancho, alto), 14, 14);
    QGraphicsPathItem* placa = escena->addPath(ruta, QPen(COLOR_BORDE, 2), QBrush(COLOR_PLACA));
    placa->setZValue(CAPA_HUD - 1);
    return placa;
}

QGraphicsTextItem* crearTexto(QGraphicsScene* escena, const QString& texto, int puntos, const QColor& color) {
    QGraphicsTextItem* t = escena->addText(texto);
    t->setDefaultTextColor(color);
    t->setFont(QFont("Arial", puntos, QFont::Bold));
    t->setZValue(CAPA_HUD + 1);
    QGraphicsDropShadowEffect* sombra = new QGraphicsDropShadowEffect();   // el item se queda con el efecto y lo libera
    sombra->setBlurRadius(3);
    sombra->setOffset(2, 2);
    sombra->setColor(QColor(0, 0, 0, 220));
    t->setGraphicsEffect(sombra);
    return t;
}

QGraphicsRectItem* crearMarco(QGraphicsScene* escena, double x, double y, double ancho, double alto) {
    QGraphicsRectItem* marco = escena->addRect(x - 3, y - 3, ancho + 6, alto + 6,
                                               QPen(COLOR_BORDE, 2), QBrush(QColor(0, 0, 0, 190)));
    marco->setZValue(CAPA_HUD);
    return marco;
}
}

PanelEstado::PanelEstado(QGraphicsScene* escena)
    : escena(escena), visible(true), vidasMaximas(0)
{
    placaIzquierda = crearPlaca(escena, PLACA_X, PLACA_Y, PLACA_ANCHO, PLACA_ALTO);
    placaTiempo = crearPlaca(escena, TIEMPO_CENTRO_X - PLACA_TIEMPO_ANCHO / 2.0, TIEMPO_Y - 8,
                             PLACA_TIEMPO_ANCHO, PLACA_TIEMPO_ALTO);
    placaJefe = crearPlaca(escena, JEFE_X - 30, 16, PLACA_JEFE_ANCHO, PLACA_JEFE_ALTO);

    vidaLlena = crearCorazon(true);
    vidaVacia = crearCorazon(false);
    for(int i = 0; i < MAX_ICONOS_VIDA; ++i){
        QGraphicsPixmapItem* icono = escena->addPixmap(vidaLlena);
        icono->setPos(VIDA_X + i * VIDA_SEPARACION - 8, VIDA_Y);
        icono->setZValue(CAPA_HUD + 1);
        iconosVida.push_back(icono);
    }

    textoTiempo = crearTexto(escena, "00:00", 26, Qt::white);
    textoTiempo->setPos(TIEMPO_CENTRO_X - 50, TIEMPO_Y - 6);

    textoCalor = crearTexto(escena, "CALOR", 13, COLOR_TEXTO);
    textoCalor->setPos(CALOR_X - 8, CALOR_ROTULO_Y);
    marcoCalor = crearMarco(escena, CALOR_X, CALOR_Y, CALOR_ANCHO_MAX, CALOR_ALTO);
    barraCalor = escena->addRect(CALOR_X, CALOR_Y, 0, CALOR_ALTO, QPen(Qt::NoPen), QBrush(QColor(255, 180, 0)));
    barraCalor->setZValue(CAPA_HUD + 1);

    textoJefe = crearTexto(escena, "MONSTRUO", 14, COLOR_TEXTO);
    textoJefe->setPos(TIEMPO_CENTRO_X - 52, 16);
    marcoJefe = crearMarco(escena, JEFE_X, JEFE_Y, JEFE_ANCHO_MAX, JEFE_ALTO);
    barraJefe = escena->addRect(JEFE_X, JEFE_Y, JEFE_ANCHO_MAX, JEFE_ALTO, QPen(Qt::NoPen), QBrush(QColor(220, 30, 50)));
    barraJefe->setZValue(CAPA_HUD + 1);

    textoEfectos = crearTexto(escena, "", 14, QColor(255, 225, 90));
    textoEfectos->setPos(EFECTOS_X, EFECTOS_Y);

    elementos << placaIzquierda << placaTiempo << placaJefe << textoTiempo << textoCalor << marcoCalor
              << barraCalor << textoJefe << marcoJefe << barraJefe << textoEfectos;
    for(QGraphicsPixmapItem* i : iconosVida){
        elementos << i;
    }
}

// Cada seccion se muestra u oculta segun lo que tenga el nivel actual
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

    // 1. Vidas: corazon lleno por cada vida y vacio por cada vida perdida.
    // Se recuerda el maximo visto para saber cuantos corazones mostrar (5 en Normal, 3 en Dificil).
    int vidas = rei->getVidas();
    vidasMaximas = std::max(vidasMaximas, vidas);
    vidasMaximas = std::min(vidasMaximas, static_cast<int>(iconosVida.size()));
    for(int i = 0; i < iconosVida.size(); ++i){
        iconosVida[i]->setVisible(i < vidasMaximas);
        iconosVida[i]->setPixmap(i < vidas ? vidaLlena : vidaVacia);
    }

    // 2. Tiempo (TiempoRestante devuelve -1 si el nivel no tiene limite)
    double restante = nivel->TiempoRestante();
    const bool hayTiempo = restante >= 0;
    placaTiempo->setVisible(hayTiempo);
    textoTiempo->setVisible(hayTiempo);
    if(hayTiempo){
        int segundosTotales = static_cast<int>(restante);
        int minutos = segundosTotales / 60;
        int segundos = segundosTotales % 60;
        textoTiempo->setPlainText(QString("%1:%2").arg(minutos, 2, 10, QChar('0')).arg(segundos, 2, 10, QChar('0')));
        textoTiempo->setDefaultTextColor(restante < 30 ? QColor(255, 90, 90) : QColor(Qt::white));   // ultimos 30 s en rojo
        const QRectF caja = textoTiempo->boundingRect();
        textoTiempo->setPos(TIEMPO_CENTRO_X - caja.width() / 2.0, TIEMPO_Y - 6);   // recentra cuando cambia el ancho del texto
    }

    // 3. Calor: solo existe en el nivel 1; el color cambia al acercarse al limite
    const bool hayCalor = (juego.getNumNivel() == 1);
    textoCalor->setVisible(hayCalor);
    marcoCalor->setVisible(hayCalor);
    barraCalor->setVisible(hayCalor);
    if(hayCalor){
        const double calor = std::min(1.0, std::max(0.0, rei->getCalorRelativo()));
        barraCalor->setRect(CALOR_X, CALOR_Y, CALOR_ANCHO_MAX * calor, CALOR_ALTO);
        QColor color(255, 190, 0);
        if(calor > 0.85){
            color = QColor(235, 40, 40);
        }
        else if(calor > 0.6){
            color = QColor(255, 120, 0);
        }
        if(rei->estaBloqueadaPorCalor()){
            color = QColor(200, 0, 0);
            textoCalor->setPlainText("SOBRECALENTADA");
            textoCalor->setDefaultTextColor(QColor(255, 90, 90));
        }
        else{
            textoCalor->setPlainText("CALOR");
            textoCalor->setDefaultTextColor(COLOR_TEXTO);
        }
        barraCalor->setBrush(QBrush(color));
    }

    // 4. Vida del jefe (getVidaJefeRelativa devuelve -1 si el nivel no tiene jefe)
    double vidaJefe = nivel->getVidaJefeRelativa();
    const bool hayJefe = vidaJefe >= 0;
    placaJefe->setVisible(hayJefe);
    textoJefe->setVisible(hayJefe);
    marcoJefe->setVisible(hayJefe);
    barraJefe->setVisible(hayJefe);
    if(hayJefe){
        vidaJefe = std::min(1.0, vidaJefe);
        barraJefe->setRect(JEFE_X, JEFE_Y, JEFE_ANCHO_MAX * vidaJefe, JEFE_ALTO);
    }

    // 5. Efectos activos
    QString efectos;
    if(rei->tieneBateria()){
        efectos += "BATERIA   ";
    }
    if(rei->tieneEscudo()){
        efectos += "ESCUDO";
    }
    textoEfectos->setPlainText(efectos);
}

void PanelEstado::mostrar(){
    this->visible = true;
    this->vidasMaximas = 0;   // partida nueva: se vuelven a contar las vidas
    for(QGraphicsItem* e : elementos){
        e->setVisible(true);
    }
}

void PanelEstado::ocultar(){
    this->visible = false;
    for(QGraphicsItem* e : elementos){
        e->setVisible(false);
    }
}
