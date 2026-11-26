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

// HUD dibujado dentro de la escena: vidas, tiempo, calor, vida del jefe y efectos
class PanelEstado {
public:
    explicit PanelEstado(QGraphicsScene* escena);
    void actualizar(const Juego& juego);
    void mostrar();
    void ocultar();

private:
    QGraphicsScene* escena;   // la escena es dueña de todos los items
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
    QPixmap vidaLlena;
    QPixmap vidaVacia;                     // vida perdida
    int vidasMaximas;                      // mayor numero de vidas visto en la partida (5 en Normal, 3 en Dificil)
    QVector<QGraphicsItem*> elementos;     // todos los items, para mostrar/ocultar de una vez
};

#endif // PANELESTADO_H
