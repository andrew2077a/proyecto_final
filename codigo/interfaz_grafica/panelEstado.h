#ifndef PANELESTADO_H
#define PANELESTADO_H

#include <QGraphicsScene>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
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
};

#endif // PANELESTADO_H
