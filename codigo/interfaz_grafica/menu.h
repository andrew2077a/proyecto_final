#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QStackedWidget>
#include "nucleo/juego.h"
#include "interfaz_grafica/vistaJuego.h"

class QWidget;
class QResizeEvent;
class QKeyEvent;

// Ventana principal: maneja la navegacion entre pantallas (menus, juego, pausa, fin)
class Menu : public QWidget {
    Q_OBJECT

private:
    QStackedWidget* pantallas;   // muestra una sola pantalla a la vez
    ModoDificultad modo;
    int ultimoNivel;             // para poder reiniciar
    Juego* juego;                // Menu es su dueño (no es QObject, se libera a mano)
    VistaJuego* vista;           // hija de Menu: Qt la libera

    QWidget* paginaDificultad;
    QWidget* paginaNiveles;
    QWidget* paginaPausa;
    QWidget* paginaGanado;
    QWidget* paginaPerdido;

    void construirPaginas();
    void conectarSenales();
    void mostrarJuego();

protected:
    bool eventFilter(QObject* objeto, QEvent* evento) override;
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
