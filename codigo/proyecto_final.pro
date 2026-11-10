QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    agente_inteligente/accion.cpp \
    agente_inteligente/agente.cpp \
    agente_inteligente/aprendizaje.cpp \
    agente_inteligente/percepcion.cpp \
    agente_inteligente/razonamiento.cpp \
    apoyo/configuraciondificultad.cpp \
    apoyo/errorJuego.cpp \
    entidades/bomba.cpp \
    entidades/camionNorcoreano.cpp \
    entidades/entidad.cpp \
    entidades/moustro.cpp \
    entidades/proyectil.cpp \
    entidades/rei.cpp \
    fisicas/caidaLibre.cpp \
    fisicas/fisica.cpp \
    fisicas/movOndulatorio.cpp \
    fisicas/movParabolico.cpp \
    fisicas/movRectilineo.cpp \
    fisicas/ondaEmpuje.cpp \
    interfaz_grafica/gestorSonido.cpp \
    interfaz_grafica/menu.cpp \
    interfaz_grafica/panelEstado.cpp \
    interfaz_grafica/spriteEntidad.cpp \
    interfaz_grafica/vistaJuego.cpp \
    main.cpp \
    mainwindow.cpp \
    nucleo/juego.cpp \
    nucleo/nivel.cpp \
    nucleo/nivel1.cpp \
    nucleo/nivel2.cpp \
    obstaculos_e_items/bateria.cpp \
    obstaculos_e_items/fosa.cpp \
    obstaculos_e_items/gaseosa.cpp \
    obstaculos_e_items/item.cpp \
    obstaculos_e_items/obstaculo.cpp \
    obstaculos_e_items/puas.cpp

HEADERS += \
    agente_inteligente/accion.h \
    agente_inteligente/agente.h \
    agente_inteligente/aprendizaje.h \
    agente_inteligente/percepcion.h \
    agente_inteligente/razonamiento.h \
    apoyo/configuraciondificultad.h \
    apoyo/errorJuego.h \
    entidades/bomba.h \
    entidades/camionNorcoreano.h \
    entidades/entidad.h \
    entidades/moustro.h \
    entidades/proyectil.h \
    entidades/rei.h \
    fisicas/caidaLibre.h \
    fisicas/fisica.h \
    fisicas/movOndulatorio.h \
    fisicas/movParabolico.h \
    fisicas/movRectilineo.h \
    fisicas/ondaEmpuje.h \
    interfaz_grafica/gestorSonido.h \
    interfaz_grafica/menu.h \
    interfaz_grafica/panelEstado.h \
    interfaz_grafica/spriteEntidad.h \
    interfaz_grafica/vistaJuego.h \
    mainwindow.h \
    nucleo/juego.h \
    nucleo/nivel.h \
    nucleo/nivel1.h \
    nucleo/nivel2.h \
    obstaculos_e_items/bateria.h \
    obstaculos_e_items/fosa.h \
    obstaculos_e_items/gaseosa.h \
    obstaculos_e_items/item.h \
    obstaculos_e_items/obstaculo.h \
    obstaculos_e_items/puas.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
