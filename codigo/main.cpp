#include <QApplication>
#include <QMessageBox>
#include <exception>
#include "interfaz_grafica/menu.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    try {
        Menu menu;                  // crea Juego, VistaJuego y las pantallas
        menu.showFullScreen();      // el juego arranca en pantalla completa (no show())
        return app.exec();          // ciclo de eventos; al terminar, ~Menu libera Juego
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error al iniciar el juego", QString::fromStdString(e.what()));
        return 1;                   // código distinto de 0 = terminó con error
    }
}