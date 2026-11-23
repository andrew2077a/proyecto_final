#ifndef PANELESTADO_H
#define PANELESTADO_H

#include <QGraphicsScene>
#include <QVector>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsPathItem>
#include "nucleo/juego.h"

class PanelEstado {
public:
    explicit PanelEstado(QGraphicsScene* escena);
    void actualizar(const Juego& juego);
    void mostrar();
    void ocultar();

private:
    QGraphicsScene* escena;
    QVector<QGraphicsPixmapItem*> iconosVida;
    QGraphicsTextItem* textoTiempo;
    QGraphicsRectItem* barraCalor;
    QGraphicsRectItem* barraJefe;
    QGraphicsTextItem* textoEfectos;
    bool visible;


    QGraphicsPathItem* placaIzquierda;     // fondo oscuro con vidas, calor y efectos
    QGraphicsPathItem* placaTiempo;        // fondo del reloj (nivel 2)
    QGraphicsPathItem* placaJefe;          // fondo de la barra del monstruo (nivel 1)
    QGraphicsRectItem* marcoCalor;         // contorno de la barra de calor
    QGraphicsRectItem* marcoJefe;          // contorno de la barra del jefe
    QGraphicsTextItem* textoCalor;         // "CALOR" / "SOBRECALENTADA"
    QGraphicsTextItem* textoJefe;          // "MONSTRUO"
    QPixmap vidaLlena;                     // corazon lleno
    QPixmap vidaVacia;                     // corazon vacio (vida perdida)
    int vidasMaximas;                      // mayor numero de vidas visto en esta partida (3 en Dificil, 5 en Normal)
    QVector<QGraphicsItem*> elementos;     // todos los elementos, para mostrar/ocultar de una vez
};

#endif // PANELESTADO_H
