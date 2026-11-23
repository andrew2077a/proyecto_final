#include "menu.h"
#include "datos.h"
#include <stdexcept>
#include <string>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QFont>
#include <QMessageBox>
#include <QTimer>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QGraphicsDropShadowEffect>

namespace {

const char* ESTILO_TARJETA = "#tarjeta { background-color: rgba(6, 26, 28, 190); border: 2px solid rgba(191, 247, 224, 180); border-radius: 18px; }";
const char* ESTILO_BOTON =
    "QPushButton { background-color: rgba(10, 52, 54, 230); color: #BFF7E0; border: 3px solid #BFF7E0; border-radius: 12px; padding: 6px 18px; }"
    "QPushButton:hover { background-color: #BFF7E0; color: #08302F; }"
    "QPushButton:pressed { background-color: #7FD9B8; color: #08302F; border-color: #7FD9B8; }";


class PaginaConFondo : public QWidget {
public:
    PaginaConFondo(const QPixmap& fondo, QWidget* padre) : QWidget(padre), imagen(fondo) {}

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter pintor(this);
        pintor.setRenderHint(QPainter::SmoothPixmapTransform);
        pintor.fillRect(rect(), Qt::black);

        const QSize destino = imagen.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
        QRect zona(QPoint(0, 0), destino);
        zona.moveCenter(rect().center());
        pintor.drawPixmap(zona, imagen);
    }

private:
    QPixmap imagen;
};

struct Pagina {
    QWidget* widget;          // la pagina completa (se agrega al QStackedWidget)
    QVBoxLayout* contenido;   // donde van titulo y botones
};

// Carga un fondo
QPixmap cargarFondo(const char* ruta) {
    QPixmap fondo(ruta);
    if (fondo.isNull()) {
        throw std::runtime_error(std::string("No se pudo cargar el fondo: ") + ruta);
    }
    return fondo;
}

QLabel* crearEtiqueta(QWidget* padre, const QString& texto, int puntos, const QString& color) {
    QLabel* etiqueta = new QLabel(texto, padre);
    etiqueta->setAlignment(Qt::AlignCenter);
    etiqueta->setFont(QFont("Arial", puntos, QFont::Bold));
    etiqueta->setStyleSheet(QString("QLabel { color: %1; background: transparent; }").arg(color));
    return etiqueta;
}

Pagina crearPagina(QWidget* padre, const QPixmap& fondo, bool alaIzquierda,
                   const QString& titulo, const QString& encabezado) {
    PaginaConFondo* pagina = new PaginaConFondo(fondo, padre);

    QHBoxLayout* externo = new QHBoxLayout(pagina);
    externo->setContentsMargins(100, 60, 100, 60);

    QFrame* tarjeta = new QFrame(pagina);
    tarjeta->setObjectName("tarjeta");
    tarjeta->setStyleSheet(ESTILO_TARJETA);

    QVBoxLayout* contenido = new QVBoxLayout(tarjeta);
    contenido->setContentsMargins(44, 34, 44, 34);
    contenido->setSpacing(18);

    if (alaIzquierda) {
        externo->addWidget(tarjeta, 0, Qt::AlignVCenter);
        externo->addStretch(1);
    } else {
        externo->addStretch(1);
        externo->addWidget(tarjeta, 0, Qt::AlignVCenter);
    }

    if (!titulo.isEmpty()) {
        QLabel* t = crearEtiqueta(tarjeta, titulo, 34, "#BFF7E0");
        QGraphicsDropShadowEffect* sombra = new QGraphicsDropShadowEffect();
        sombra->setBlurRadius(6);
        sombra->setOffset(2, 2);
        sombra->setColor(QColor(0, 0, 0, 200));
        t->setGraphicsEffect(sombra);
        contenido->addWidget(t);
    }
    if (!encabezado.isEmpty()) {
        contenido->addWidget(crearEtiqueta(tarjeta, encabezado, 18, "#8FE3C4"));
    }
    return { pagina, contenido };
}

// Crea un boton con el estilo comun y lo agrega a la tarjeta de la pagina.
QPushButton* crearBoton(Pagina& pagina, const QString& texto) {
    QPushButton* boton = new QPushButton(texto, pagina.widget);
    boton->setMinimumSize(360, 72);
    boton->setFont(QFont("Arial", 20, QFont::Bold));
    boton->setCursor(Qt::PointingHandCursor);
    boton->setStyleSheet(ESTILO_BOTON);
    pagina.contenido->addWidget(boton);
    return boton;
}

// Recordatorio de controles
void agregarAyudaControles(Pagina& pagina) {
    QLabel* ayuda = new QLabel(
        "A / D  mover\nW  saltar\nS  agacharse\nESPACIO  disparar (nivel 1)\nESC  pausa      F11  pantalla completa",
        pagina.widget);
    ayuda->setAlignment(Qt::AlignCenter);
    ayuda->setFont(QFont("Arial", 13));
    ayuda->setStyleSheet("QLabel { color: #8FE3C4; background: transparent; }");
    pagina.contenido->addSpacing(6);
    pagina.contenido->addWidget(ayuda);
}

// Pista de música que corresponde a cada nivel.
IdMusica musicaDeNivel(int num) {
    return (num == 1) ? IdMusica::NIVEL1 : IdMusica::NIVEL2;
}
}

// Constructor y destructor
Menu::Menu(QWidget* parent)
    : QWidget(parent),
    pantallas(new QStackedWidget(this)),
    modo(ModoDificultad::NORMAL),
    ultimoNivel(1),
    juego(new Juego()),
    vista(nullptr),
    paginaDificultad(nullptr),
    paginaNiveles(nullptr),
    paginaPausa(nullptr),
    paginaGanado(nullptr),
    paginaPerdido(nullptr)
{
    setStyleSheet("Menu { background-color: black; }");
    setAttribute(Qt::WA_StyledBackground, true);

    QVBoxLayout* principal = new QVBoxLayout(this);
    principal->setContentsMargins(0, 0, 0, 0);
    principal->addWidget(pantallas);


    try {
        vista = new VistaJuego(juego, this);
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
    delete juego;
}

// Construccion de pantallas
void Menu::construirPaginas() {
    // Cada fondo se carga UNA vez (la pantalla de dificultad y la de niveles comparten el mismo).
    const QPixmap fondoMenu    = cargarFondo(Rutas::ARCHIVO_FONDO_MENU);
    const QPixmap fondoPausa   = cargarFondo(Rutas::ARCHIVO_FONDO_PAUSA);
    const QPixmap fondoGanado  = cargarFondo(Rutas::ARCHIVO_FONDO_GANADO);
    const QPixmap fondoPerdido = cargarFondo(Rutas::ARCHIVO_FONDO_PERDIDO);

    // Pantalla 1: dificultad
    Pagina pDif = crearPagina(pantallas, fondoMenu, false, "GUERRA DE COREA", "Elige la dificultad");
    paginaDificultad = pDif.widget;
    QPushButton* bNormal  = crearBoton(pDif, "Normal");
    QPushButton* bDificil = crearBoton(pDif, "Difícil");
    QPushButton* bSalir   = crearBoton(pDif, "Salir");

    connect(bNormal,  &QPushButton::clicked, this, [this]() { alElegirDificultad(ModoDificultad::NORMAL); });
    connect(bDificil, &QPushButton::clicked, this, [this]() { alElegirDificultad(ModoDificultad::DIFICIL); });
    connect(bSalir,   &QPushButton::clicked, this, &Menu::alSalir);
    pantallas->addWidget(paginaDificultad);

    // Pantalla 2: seleccion de nivel (mismo fondo)
    Pagina pNiv = crearPagina(pantallas, fondoMenu, false, "GUERRA DE COREA", "Elige el nivel");
    paginaNiveles = pNiv.widget;
    QPushButton* bNivel1 = crearBoton(pNiv, "Nivel 1");
    QPushButton* bNivel2 = crearBoton(pNiv, "Nivel 2");
    QPushButton* bVolver = crearBoton(pNiv, "Volver");
    agregarAyudaControles(pNiv);
    connect(bNivel1, &QPushButton::clicked, this, [this]() { alElegirNivel(1); });
    connect(bNivel2, &QPushButton::clicked, this, [this]() { alElegirNivel(2); });
    connect(bVolver, &QPushButton::clicked, this, &Menu::mostrarPantallaDificultad);
    pantallas->addWidget(paginaNiveles);

    // Pantalla de pausa
    Pagina pPau = crearPagina(pantallas, fondoPausa, false, "", "");
    paginaPausa = pPau.widget;
    QPushButton* bReanudar = crearBoton(pPau, "Reanudar");
    QPushButton* bReiniciarP = crearBoton(pPau, "Reiniciar");
    QPushButton* bMenuP = crearBoton(pPau, "Volver al menú");
    connect(bReanudar,   &QPushButton::clicked, this, &Menu::alReanudar);
    connect(bReiniciarP, &QPushButton::clicked, this, &Menu::alReiniciar);
    connect(bMenuP,      &QPushButton::clicked, this, &Menu::alVolverAlMenu);
    pantallas->addWidget(paginaPausa);

    // Pantalla de victoria
    Pagina pGan = crearPagina(pantallas, fondoGanado, false, "", "");
    paginaGanado = pGan.widget;
    QPushButton* bReiniciarG = crearBoton(pGan, "Jugar de nuevo");
    QPushButton* bMenuG = crearBoton(pGan, "Volver al menú");
    connect(bReiniciarG, &QPushButton::clicked, this, &Menu::alReiniciar);
    connect(bMenuG,      &QPushButton::clicked, this, &Menu::alVolverAlMenu);
    pantallas->addWidget(paginaGanado);

    // Pantalla de derrota
    Pagina pPer = crearPagina(pantallas, fondoPerdido, true, "", "");
    paginaPerdido = pPer.widget;
    QPushButton* bReiniciarD = crearBoton(pPer, "Reintentar");
    QPushButton* bMenuD = crearBoton(pPer, "Volver al menú");
    connect(bReiniciarD, &QPushButton::clicked, this, &Menu::alReiniciar);
    connect(bMenuD,      &QPushButton::clicked, this, &Menu::alVolverAlMenu);
    pantallas->addWidget(paginaPerdido);
}


void Menu::conectarSenales() {
    connect(vista, &VistaJuego::nivelTerminado,           this, &Menu::alTerminarNivel);
    connect(vista, &VistaJuego::pausaSolicitada,          this, &Menu::alPausar);
    connect(vista, &VistaJuego::pantallaCompletaSolicitada, this, &Menu::alternarPantallaCompleta);
}

// Cambio de pantallas
void Menu::mostrarPantallaDificultad() {
    pantallas->setCurrentWidget(paginaDificultad);
    vista->reproducirMusica(IdMusica::MENU);      // si ya suena el menu no la reinicia
}

void Menu::mostrarPantallaNiveles() {
    pantallas->setCurrentWidget(paginaNiveles);
    vista->reproducirMusica(IdMusica::MENU);      // misma musica que la pantalla anterior
}

void Menu::mostrarPausa() {
    pantallas->setCurrentWidget(paginaPausa);
    vista->reproducirMusica(IdMusica::PAUSA);
}

void Menu::mostrarFin(bool gano) {
    pantallas->setCurrentWidget(gano ? paginaGanado : paginaPerdido);
    vista->reproducirMusica(IdMusica::MENU);      // el documento no define musica de fin
}

// Muestra la vista del juego y ajusta su escala.
void Menu::mostrarJuego() {
    pantallas->setCurrentWidget(vista);
    QTimer::singleShot(0, vista, &VistaJuego::ajustarEscala);
}

void Menu::resizeEvent(QResizeEvent* evento) {
    QWidget::resizeEvent(evento);
    if (vista != nullptr) {
        QTimer::singleShot(0, vista, &VistaJuego::ajustarEscala);
    }
}


void Menu::keyPressEvent(QKeyEvent* evento) {
    if (evento->key() == Qt::Key_F11) {
        alternarPantallaCompleta();
        return;
    }
    if (evento->key() == Qt::Key_Escape && pantallas->currentWidget() == paginaPausa) {
        alReanudar();
        return;
    }
    QWidget::keyPressEvent(evento);
}

// Slots
void Menu::alElegirDificultad(ModoDificultad modo) {
    this->modo = modo;                            // el parametro tapa al miembro: se usa this->
    mostrarPantallaNiveles();
}

void Menu::alElegirNivel(int num) {

    auto volverPorError = [this](const QString& titulo, const std::exception& e) {
        vista->detener();
        juego->volverAlMenu();
        QMessageBox::critical(this, titulo, QString::fromStdString(e.what()));
        mostrarPantallaDificultad();
    };

    try {
        juego->configurar(modo);                  // 1. dificultad elegida
        juego->iniciarNivel(num);                 // 2. crea el nivel
        ultimoNivel = num;
        vista->cargarFondo(num);                  // 3. fondo del nivel
        mostrarJuego();
        vista->reproducirMusica(musicaDeNivel(num));
        vista->iniciar();
    }

    catch (const std::invalid_argument& e) {      // nivel inexistente o parametros incoherentes
        volverPorError("Parámetros inválidos", e);
    }
    catch (const std::runtime_error& e) {         // fondo, mapa o sonido que no se pudo cargar
        volverPorError("Recurso no encontrado", e);
    }
    catch (const std::exception& e) {
        volverPorError("Error inesperado", e);
    }
}

void Menu::alPausar() {
    if (juego->getEstado() != EstadoJuego::JUGANDO) {   // Esc solo pausa si se esta jugando
        return;
    }
    juego->pausar();
    vista->detener();
    juego->manejarTecla(Tecla::IZQUIERDA, false);
    juego->manejarTecla(Tecla::DERECHA, false);
    juego->manejarTecla(Tecla::SALTAR, false);
    juego->manejarTecla(Tecla::DISPARAR, false);
    juego->manejarTecla(Tecla::AGACHARSE, false);
    mostrarPausa();
}

void Menu::alReanudar() {
    juego->reanudar();
    mostrarJuego();                               // primero mostrar la vista, luego iniciar()
    vista->reproducirMusica(musicaDeNivel(juego->getNumNivel()));
    vista->iniciar();                             // reinicia el reloj: evita un salto de tiempo
}

void Menu::alReiniciar() {

    alElegirNivel(ultimoNivel);
}

void Menu::alVolverAlMenu() {
    vista->detener();
    juego->volverAlMenu();
    mostrarPantallaDificultad();
}

void Menu::alTerminarNivel(bool gano) {
    mostrarFin(gano);
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
    QApplication::quit();
}
