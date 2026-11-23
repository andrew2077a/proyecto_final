#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QStackedWidget>
#include "nucleo/juego.h"
#include "interfaz_grafica/vistaJuego.h"


class QWidget;
class QResizeEvent;
class QKeyEvent;

class Menu : public QWidget {
    Q_OBJECT

private:
    QStackedWidget* pantallas;
    ModoDificultad modo;
    int ultimoNivel;
    Juego* juego;
    VistaJuego* vista;


    QWidget* paginaDificultad;
    QWidget* paginaNiveles;
    QWidget* paginaPausa;
    QWidget* paginaGanado;
    QWidget* paginaPerdido;


    void construirPaginas();   // crea las 5 páginas con sus botones
    void conectarSenales();    // conecta las señales de VistaJuego
    void mostrarJuego();       // muestra la vista del juego y ajusta su escala

protected:

    void resizeEvent(QResizeEvent* evento) override;

    void keyPressEvent(QKeyEvent* evento) override;

public:
    explicit Menu(QWidget* parent = nullptr);
    ~Menu();

    void mostrarPantallaDificultad();
    void mostrarPantallaNiveles();
    void mostrarPausa();
    void mostrarFin(bool gano);

public slots:
    void alElegirDificultad(ModoDificultad modo);
    void alElegirNivel(int num);
    void alPausar();
    void alReanudar();
    void alReiniciar();
    void alVolverAlMenu();
    void alTerminarNivel(bool gano);
    void alternarPantallaCompleta();
    void alSalir();
};

#endif // MENU_H
