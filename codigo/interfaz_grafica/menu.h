#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QStackedWidget>
#include "nucleo/juego.h"
#include "interfaz_grafica/vistaJuego.h"   // CAMBIO: ruta corregida (antes "VistaJuego.h", no compilaba)

// CAMBIO: declaraciones adelantadas, para no incluir las clases completas en el header
class QLabel;
class QResizeEvent;

class Menu : public QWidget {
    Q_OBJECT

private:
    QStackedWidget* pantallas;
    ModoDificultad modo;
    int ultimoNivel;
    Juego* juego;
    VistaJuego* vista;

    // CAMBIO: una página por pantalla, para mostrarlas con setCurrentWidget()
    QLabel* paginaDificultad;
    QLabel* paginaNiveles;
    QLabel* paginaPausa;
    QLabel* paginaGanado;
    QLabel* paginaPerdido;

    // CAMBIO: métodos auxiliares privados
    void construirPaginas();   // crea las 5 páginas con sus botones
    void conectarSenales();    // conecta las señales de VistaJuego
    void mostrarJuego();       // muestra la vista del juego y ajusta su escala

protected:
    // CAMBIO: reajusta la escala de la vista cuando cambia el tamaño de la ventana
    void resizeEvent(QResizeEvent* evento) override;

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