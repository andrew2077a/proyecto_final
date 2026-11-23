#include "menu.h"
#include "datos.h"
#include <stdexcept>
#include <string>
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QFont>
#include <QMessageBox>
#include <QTimer>
#include <QResizeEvent>

namespace {

// Crea una página con una imagen de fondo y un layout vertical centrado.
// Si la imagen no existe lanza runtime_error (igual que hace SpriteEntidad).
QLabel* crearPagina(QWidget* padre, const char* rutaFondo) {
    QPixmap fondo(rutaFondo);
    if (fondo.isNull()) {                       // se valida ANTES de hacer new, así no se filtra memoria
        throw std::runtime_error(std::string("No se pudo cargar el fondo: ") + rutaFondo);
    }
    QLabel* pagina = new QLabel(padre);         // un QLabel puede mostrar imagen Y contener botones
    pagina->setPixmap(fondo);
    pagina->setScaledContents(true);            // la imagen se estira al tamaño de la pantalla

    QVBoxLayout* columna = new QVBoxLayout(pagina);
    columna->setAlignment(Qt::AlignRight | Qt::AlignVCenter);   // botones a la derecha, centrados verticalmente
    columna->setContentsMargins(0, 0, 80, 0);                   // 80 px de margen derecho
    columna->setSpacing(20);                    // separación entre botones
    return pagina;
}

// Crea un botón con el estilo común y lo agrega al layout de la página.
QPushButton* crearBoton(QLabel* pagina, const QString& texto) {
    QPushButton* boton = new QPushButton(texto, pagina);   // pagina es su padre: Qt lo libera
    boton->setMinimumSize(320, 70);
    boton->setFont(QFont("Arial", 20, QFont::Bold));
    boton->setStyleSheet("QPushButton { background-color: #5B2A86; color: #7CFF4A; border: 3px solid #7CFF4A; border-radius: 10px; } QPushButton:hover { background-color: #7CFF4A; color: #5B2A86; }");
    pagina->layout()->addWidget(boton);
    return boton;
}

// Pista de música que corresponde a cada nivel.
IdMusica musicaDeNivel(int num) {
    return (num == 1) ? IdMusica::NIVEL1 : IdMusica::NIVEL2;
}
}

// ============ Constructor y destructor ============
Menu::Menu(QWidget* parent)
    : QWidget(parent),
    pantallas(new QStackedWidget(this)),
    modo(ModoDificultad::NORMAL),
    ultimoNivel(1),
    juego(new Juego()),
    vista(nullptr),                  // CAMBIO: se crea dentro del try (ver explicación)
    paginaDificultad(nullptr),
    paginaNiveles(nullptr),
    paginaPausa(nullptr),
    paginaGanado(nullptr),
    paginaPerdido(nullptr)
{
    // CAMBIO: sin layout, el QStackedWidget no crece al cambiar el tamaño de la ventana
    QVBoxLayout* principal = new QVBoxLayout(this);
    principal->setContentsMargins(0, 0, 0, 0);
    principal->addWidget(pantallas);

    // Si algo falla aquí (sonido o fondo faltante) el destructor NO se ejecuta,
    // así que se libera "juego" a mano y se relanza la excepción (mismo patrón de Moustro).
    try {
        vista = new VistaJuego(juego, this);   // this = padre: Qt la borra sola. Carga los sonidos.
        pantallas->addWidget(vista);
        construirPaginas();
        conectarSenales();
    } catch (...) {
        delete juego;
        juego = nullptr;
        throw;
    }

    mostrarPantallaDificultad();               // primera pantalla del juego
}

Menu::~Menu() {
    // "vista" y las páginas son hijas de Menu: Qt las borra después de este destructor.
    // "juego" no es QObject, así que se borra a mano.
    delete juego;
}

// ============ Construcción de pantallas ============
void Menu::construirPaginas() {
    // ---------- Pantalla 1: dificultad ----------
    paginaDificultad = crearPagina(pantallas, Rutas::ARCHIVO_FONDO_MENU);
    QPushButton* bNormal  = crearBoton(paginaDificultad, "Normal");
    QPushButton* bDificil = crearBoton(paginaDificultad, "Difícil");
    QPushButton* bSalir   = crearBoton(paginaDificultad, "Salir");
    // alElegirDificultad recibe un argumento que clicked() no trae: se usa una lambda.
    connect(bNormal,  &QPushButton::clicked, this, [this]() { alElegirDificultad(ModoDificultad::NORMAL); });
    connect(bDificil, &QPushButton::clicked, this, [this]() { alElegirDificultad(ModoDificultad::DIFICIL); });
    connect(bSalir,   &QPushButton::clicked, this, &Menu::alSalir);
    pantallas->addWidget(paginaDificultad);

    // ---------- Pantalla 2: selección de nivel (mismo fondo) ----------
    paginaNiveles = crearPagina(pantallas, Rutas::ARCHIVO_FONDO_MENU);
    QPushButton* bNivel1 = crearBoton(paginaNiveles, "Nivel 1");
    QPushButton* bNivel2 = crearBoton(paginaNiveles, "Nivel 2");
    QPushButton* bVolver = crearBoton(paginaNiveles, "Volver");
    connect(bNivel1, &QPushButton::clicked, this, [this]() { alElegirNivel(1); });
    connect(bNivel2, &QPushButton::clicked, this, [this]() { alElegirNivel(2); });
    connect(bVolver, &QPushButton::clicked, this, &Menu::mostrarPantallaDificultad);
    pantallas->addWidget(paginaNiveles);

    // ---------- Pantalla de pausa (fondo propio) ----------
    paginaPausa = crearPagina(pantallas, Rutas::ARCHIVO_FONDO_PAUSA);
    QPushButton* bReanudar = crearBoton(paginaPausa, "Reanudar");
    QPushButton* bReiniciarP = crearBoton(paginaPausa, "Reiniciar");
    QPushButton* bMenuP = crearBoton(paginaPausa, "Volver al menú");
    connect(bReanudar,   &QPushButton::clicked, this, &Menu::alReanudar);
    connect(bReiniciarP, &QPushButton::clicked, this, &Menu::alReiniciar);
    connect(bMenuP,      &QPushButton::clicked, this, &Menu::alVolverAlMenu);
    pantallas->addWidget(paginaPausa);

    // ---------- Pantalla de victoria ----------
    paginaGanado = crearPagina(pantallas, Rutas::ARCHIVO_FONDO_GANADO);
    QPushButton* bReiniciarG = crearBoton(paginaGanado, "Jugar de nuevo");
    QPushButton* bMenuG = crearBoton(paginaGanado, "Volver al menú");
    connect(bReiniciarG, &QPushButton::clicked, this, &Menu::alReiniciar);
    connect(bMenuG,      &QPushButton::clicked, this, &Menu::alVolverAlMenu);
    pantallas->addWidget(paginaGanado);

    // ---------- Pantalla de derrota ----------
    paginaPerdido = crearPagina(pantallas, Rutas::ARCHIVO_FONDO_PERDIDO);
    QPushButton* bReiniciarD = crearBoton(paginaPerdido, "Reintentar");
    QPushButton* bMenuD = crearBoton(paginaPerdido, "Volver al menú");
    connect(bReiniciarD, &QPushButton::clicked, this, &Menu::alReiniciar);
    connect(bMenuD,      &QPushButton::clicked, this, &Menu::alVolverAlMenu);
    pantallas->addWidget(paginaPerdido);
}

// Sintaxis: connect(emisor, &ClaseEmisor::señal, receptor, &ClaseReceptor::slot)
// La señal y el slot se comprueban al COMPILAR, y los tipos de argumentos deben coincidir.
void Menu::conectarSenales() {
    connect(vista, &VistaJuego::nivelTerminado,           this, &Menu::alTerminarNivel);        // (bool) -> (bool)
    connect(vista, &VistaJuego::pausaSolicitada,          this, &Menu::alPausar);               // () -> ()
    connect(vista, &VistaJuego::pantallaCompletaSolicitada, this, &Menu::alternarPantallaCompleta);
}

// ============ Cambio de pantallas ============
void Menu::mostrarPantallaDificultad() {
    pantallas->setCurrentWidget(paginaDificultad);
    vista->reproducirMusica(IdMusica::MENU);      // si ya suena el menú no la reinicia
}

void Menu::mostrarPantallaNiveles() {
    pantallas->setCurrentWidget(paginaNiveles);
    vista->reproducirMusica(IdMusica::MENU);      // misma música que la pantalla anterior
}

void Menu::mostrarPausa() {
    pantallas->setCurrentWidget(paginaPausa);
    vista->reproducirMusica(IdMusica::PAUSA);
}

void Menu::mostrarFin(bool gano) {
    pantallas->setCurrentWidget(gano ? paginaGanado : paginaPerdido);
    vista->reproducirMusica(IdMusica::MENU);      // CAMBIO: el documento no define música de fin
}

// Muestra la vista del juego y ajusta su escala.
void Menu::mostrarJuego() {
    pantallas->setCurrentWidget(vista);
    // singleShot(0) = "ejecuta esto apenas Qt termine de acomodar la ventana".
    // Si se llamara directo, fitInView usaría un tamaño todavía sin actualizar.
    QTimer::singleShot(0, vista, &VistaJuego::ajustarEscala);
}

void Menu::resizeEvent(QResizeEvent* evento) {
    QWidget::resizeEvent(evento);                 // comportamiento normal de Qt primero
    if (vista != nullptr) {
        QTimer::singleShot(0, vista, &VistaJuego::ajustarEscala);
    }
}

// ============ Slots ============
void Menu::alElegirDificultad(ModoDificultad modo) {
    this->modo = modo;                            // el parámetro tapa al miembro: se usa this->
    mostrarPantallaNiveles();
}

void Menu::alElegirNivel(int num) {
    // Mismo manejo para las tres excepciones: aviso, limpiar y volver al menú.
    auto volverPorError = [this](const QString& titulo, const std::exception& e) {
        vista->detener();                         // por si el timer ya había arrancado
        juego->volverAlMenu();                    // borra el nivel a medias y deja el estado en MENU
        QMessageBox::critical(this, titulo, QString::fromStdString(e.what()));
        mostrarPantallaDificultad();
    };

    try {
        juego->configurar(modo);                  // 1. dificultad elegida
        juego->iniciarNivel(num);                 // 2. crea el nivel (puede lanzar excepción)
        ultimoNivel = num;                        //    solo se guarda si el nivel se creó bien
        vista->cargarFondo(num);                  // 3. fondo del nivel
        mostrarJuego();                           // 4. ANTES de iniciar(): para que setFocus() funcione
        vista->reproducirMusica(musicaDeNivel(num));
        vista->iniciar();                         // 5. arranca el timer y el teclado
    }
    // Primero las específicas y al final std::exception, que atrapa todo lo demás.
    catch (const std::invalid_argument& e) {      // nivel inexistente o parámetros incoherentes
        volverPorError("Parámetros inválidos", e);
    }
    catch (const std::runtime_error& e) {         // fondo, mapa o sonido que no se pudo cargar
        volverPorError("Recurso no encontrado", e);
    }
    catch (const std::exception& e) {             // cualquier otro error estándar
        volverPorError("Error inesperado", e);
    }
}

void Menu::alPausar() {
    if (juego->getEstado() != EstadoJuego::JUGANDO) {   // Esc solo pausa si se está jugando
        return;
    }
    juego->pausar();
    vista->detener();                             // se detiene el timer: no se gasta CPU en pausa
    // CAMBIO: soltar todas las teclas. Si el jugador suelta una tecla mientras está en el menú
    // de pausa, la vista no recibe el keyRelease y Rei quedaría caminando sola al reanudar.
    juego->manejarTecla(Tecla::IZQUIERDA, false);
    juego->manejarTecla(Tecla::DERECHA, false);
    juego->manejarTecla(Tecla::SALTAR, false);
    juego->manejarTecla(Tecla::DISPARAR, false);
    mostrarPausa();
}

void Menu::alReanudar() {
    juego->reanudar();
    mostrarJuego();                               // primero mostrar la vista, luego iniciar()
    vista->reproducirMusica(musicaDeNivel(juego->getNumNivel()));
    vista->iniciar();                             // reinicia el reloj: evita un salto de tiempo
}

void Menu::alReiniciar() {
    // CAMBIO: se reutiliza alElegirNivel(): ya tiene el try/catch y deja todo listo.
    alElegirNivel(ultimoNivel);
}

void Menu::alVolverAlMenu() {
    vista->detener();
    juego->volverAlMenu();                        // borra el nivel y pasa a EstadoJuego::MENU
    mostrarPantallaDificultad();
}

void Menu::alTerminarNivel(bool gano) {
    mostrarFin(gano);                             // la vista ya detuvo su timer antes de emitir
}

void Menu::alternarPantallaCompleta() {
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void Menu::alSalir() {
    vista->detener();
    // CAMBIO: no se hace delete aquí. quit() cierra el ciclo de eventos, main() termina,
    // Menu se destruye y ~Menu libera Juego. Borrar aquí lo liberaría dos veces.
    QApplication::quit();
}