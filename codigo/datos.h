#ifndef DATOS_H
#define DATOS_H
namespace Tamanio {
// Rei
constexpr double REI_MECHA_ANCHO = 140;
constexpr double REI_MECHA_ALTO = 220;
constexpr double REI_SOLDADO_ANCHO = 70;
constexpr double REI_SOLDADO_ALTO = 110;

// Monstruo
constexpr double MONSTRUO_ANCHO = 300;
constexpr double MONSTRUO_ALTO = 500;

// Camion
constexpr double CAMION_ANCHO = 180;
constexpr double CAMION_ALTO = 90;

// Proyectiles
constexpr double BALA_REI_ANCHO = 30;
constexpr double BALA_REI_ALTO = 12;
constexpr double PARABOLICO_ANCHO = 50;
constexpr double PARABOLICO_ALTO = 50;
constexpr double ONDULATORIO_ANCHO = 60;
constexpr double ONDULATORIO_ALTO = 40;
constexpr double PUA_ANCHO = 20;
constexpr double PUA_ALTO = 20;
constexpr double BALA_LATERAL_ANCHO = 40;
constexpr double BALA_LATERAL_ALTO = 14;
constexpr double BOMBA_ANCHO = 40;
constexpr double BOMBA_ALTO = 60;

// Obstaculos (bloque y fosa varian por pieza; estos son valores base)
constexpr double BLOQUE_ANCHO = 200;
constexpr double BLOQUE_ALTO = 120;
constexpr double FOSA_ANCHO = 300;
constexpr double FOSA_ALTO = 180;
constexpr double PUAS_ANCHO = 120;
constexpr double PUAS_ALTO = 30;

// Items
constexpr double BATERIA_ANCHO = 40;
constexpr double BATERIA_ALTO = 60;
constexpr double GASEOSA_ANCHO = 40;
constexpr double GASEOSA_ALTO = 70;

// Meta
constexpr double META_ANCHO = 80;
constexpr double META_ALTO = 200;

// Pantalla logica
constexpr double ANCHO_PANTALLA = 1920;
constexpr double ALTO_PANTALLA = 1080;
constexpr double ESPERA_FIN=100;
}
namespace ParametrosComunes{
// NORMAL
constexpr int VIDAS_REI = 5;
constexpr int DANIO_BASE = 1;
constexpr double INVULNERABILIDAD = 1.5;
constexpr double GRAVEDAD = 1500;
constexpr double VELOCIDAD_MECHA = 300;
constexpr double VELOCIDAD_SOLDADO = 400;
constexpr double VELOCIDAD_SALTO = 900;
constexpr double ANGULO_SALTO = 60;
}
namespace comunesDificil {
// DIFICIL
constexpr int VIDAS_REI = 3;
constexpr int DANIO_BASE = 1;
constexpr double INVULNERABILIDAD = 1.0;
constexpr double GRAVEDAD = 1500;
constexpr double VELOCIDAD_MECHA = 300;
constexpr double VELOCIDAD_SOLDADO = 400;
constexpr double VELOCIDAD_SALTO = 900;
constexpr double ANGULO_SALTO = 60;
}
namespace Rutas {
constexpr const char* CARPETA_SPRITES = ":/sprites/";
constexpr const char* EXTENSION_SPRITES = ".png";

constexpr const char* ARCHIVO_NIVEL2 = ":/maps/mundo.txt";

constexpr const char* ARCHIVO_FONDO_NIVEL1 = ":/fondos/nivel1.png";
constexpr const char* ARCHIVO_FONDO_NIVEL2 = ":/fondos/nivel2.png";

constexpr const char* ARCHIVO_FONDO_MENU = ":/fondos/menu.png";
constexpr const char* ARCHIVO_FONDO_PAUSA = ":/fondos/pausa.png";
constexpr const char* ARCHIVO_FONDO_GANADO = ":/fondos/ganado.png";
constexpr const char* ARCHIVO_FONDO_PERDIDO = ":/fondos/perdido.png";

constexpr const char* MUSICA_MENU = ":/musica/menu.mp3";
constexpr const char* MUSICA_PAUSA = ":/musica/pausa.mp3";
constexpr const char* MUSICA_NIVEL1 = ":/musica/nivel1.mp3";
constexpr const char* MUSICA_NIVEL2 = ":/musica/nivel2.mp3";

constexpr const char* SONIDO_DISPARO_REI = ":/sonidos/disparo_rei.wav";
constexpr const char* SONIDO_BOMBA_CAE = ":/sonidos/bomba_cae.wav";
constexpr const char* SONIDO_MONSTRUO_LANZA = ":/sonidos/monstruo_lanza.wav";
constexpr const char* SONIDO_REI_DANIO = ":/sonidos/rei_danio.wav";
constexpr const char* SONIDO_EXPLOSION = ":/sonidos/explosion.wav";
constexpr const char* SONIDO_ITEM = ":/sonidos/item.wav";
}
namespace Nivel2Mundo {
constexpr double REI_INICIO_X = 100;
constexpr double Y_SUELO = 900;
constexpr double ANCHO_MUNDO = 15360;        // 8 pantallas de 1920
constexpr double META_X = ANCHO_MUNDO - Tamanio::META_ANCHO - 100;
constexpr double MARGEN_APARICION_BALA = 100;    // px a la izquierda de la camara
constexpr double ALTURA_APARICION_BOMBA = -200;  // y por encima de la pantalla
}

namespace Nivel2Normal {
constexpr double TIEMPO_LIMITE = 180;
constexpr double INTERVALO_BOMBA = 4;
constexpr int NUM_BOMBAS = 1;
constexpr double RANGO_CAIDA_BOMBA = 200;
constexpr double RADIO_DANIO_BOMBA = 60;
constexpr double RADIO_EMPUJE_BOMBA = 150;
constexpr double FUERZA_EMPUJE = 400;
constexpr double AMORTIGUAMIENTO_EMPUJE = 3;
constexpr double INTERVALO_BALA_LATERAL = 5;
constexpr int NUM_BALAS_LATERALES = 1;
constexpr double VELOCIDAD_BALA = 500;
constexpr double VELOCIDAD_CAMION_PATRULLA = 120;
constexpr double DURACION_GASEOSA = 8;
constexpr double MULT_VELOCIDAD_GASEOSA = 0.5;
constexpr double ESPERA_PUAS = 1;
}

namespace Nivel2Dificil {
constexpr double TIEMPO_LIMITE = 150;
constexpr double INTERVALO_BOMBA = 4;
constexpr int NUM_BOMBAS = 2;
constexpr double RANGO_CAIDA_BOMBA = 100;
constexpr double RADIO_DANIO_BOMBA = 60;
constexpr double RADIO_EMPUJE_BOMBA = 150;
constexpr double FUERZA_EMPUJE = 400;
constexpr double AMORTIGUAMIENTO_EMPUJE = 3;
constexpr double INTERVALO_BALA_LATERAL = 4;
constexpr int NUM_BALAS_LATERALES = 2;
constexpr double VELOCIDAD_BALA = 600;
constexpr double VELOCIDAD_CAMION_PATRULLA = 200;
constexpr double DURACION_GASEOSA = 8;
constexpr double MULT_VELOCIDAD_GASEOSA = 0.5;
constexpr double ESPERA_PUAS = 1;
}

namespace Nivel1Init {
constexpr double ANCHO_MUNDO = Tamanio::ANCHO_PANTALLA;
constexpr double Y_SUELO = 900;
constexpr double REI_INICIO_X = 100;
constexpr double REI_INICIO_Y = Y_SUELO - Tamanio::REI_MECHA_ALTO;
constexpr double MONSTRUO_INICIO_X = Tamanio::ANCHO_PANTALLA - Tamanio::MONSTRUO_ANCHO - 50;
constexpr double MONSTRUO_INICIO_Y = Y_SUELO - Tamanio::MONSTRUO_ALTO;
constexpr double LIMITE_REI_IZQ = 0;
constexpr double LIMITE_REI_DER = MONSTRUO_INICIO_X - Tamanio::REI_MECHA_ANCHO;
constexpr double CAMION_INICIO_X = Tamanio::ANCHO_PANTALLA;
constexpr double CAMION_INICIO_Y = Y_SUELO - Tamanio::CAMION_ALTO;
constexpr double BATERIA_Y = Y_SUELO - Tamanio::BATERIA_ALTO;
constexpr double BATERIA_X_MIN = 300;
constexpr double BATERIA_X_MAX = LIMITE_REI_DER - Tamanio::BATERIA_ANCHO;
}
namespace Nivel1Normal {
constexpr int VIDA_MONSTRUO = 30;
constexpr double UMBRAL_FASE1 = 0.66;
constexpr double UMBRAL_FASE2 = 0.33;
constexpr double INTERVALO_PARABOLICO = 3;
constexpr double INTERVALO_ONDULATORIO = 6;
constexpr double TIEMPO_CARGA = 1.0;
constexpr int ATAQUES_COMBINADOS_MIN = 2;
constexpr int ATAQUES_COMBINADOS_MAX = 3;
constexpr bool ATAQUES_DOBLES = false;
constexpr bool ESCUDO_MONSTRUO = false;
constexpr double UMBRAL_ESCUDO_1 = 0.75;
constexpr double UMBRAL_ESCUDO_2 = 0.50;
constexpr double UMBRAL_ESCUDO_3 = 0.25;
constexpr double DURACION_ESCUDO = 10;
constexpr bool ESTALLIDO_ACTIVO = false;
constexpr int PUAS_MIN = 6;
constexpr int PUAS_MAX = 8;
constexpr double INTERVALO_ESTALLIDO = 8;
constexpr int DANIO_DISPARO = 1;
constexpr double ANGULO_DISPARO = 45;
constexpr double VELOCIDAD_MAX_DISPARO = 1600;
constexpr int NUM_ZONAS = 3;
constexpr double FACTOR_OLVIDO = 0.80;
constexpr double SUAVIZADO_MIN = 0.15;
constexpr double INTERVALO_CAMION_INICIAL = 4;
constexpr double VELOCIDAD_CAMION_INICIAL = 250;
constexpr double RAMPA_CAMIONES = 0.01;
constexpr double CALOR_MAX = 100;
constexpr double CALOR_POR_DISPARO = 5;
constexpr double TIEMPO_RECARGA_CALOR = 2.5;
constexpr double TIEMPO_BLOQUEO_GOLPE = 2;
constexpr double CADENCIA_BASE = 0.25;
constexpr double DURACION_BATERIA = 10;
constexpr double MULT_CADENCIA_BATERIA = 2;
constexpr double MULT_VELOCIDAD_BATERIA = 0.6;
constexpr double INTERVALO_BATERIA = 15;
}

namespace Nivel1Dificil {
constexpr int VIDA_MONSTRUO = 30;
constexpr double UMBRAL_FASE1 = 0.66;
constexpr double UMBRAL_FASE2 = 0.33;
constexpr double INTERVALO_PARABOLICO = 2;
constexpr double INTERVALO_ONDULATORIO = 4;
constexpr double TIEMPO_CARGA = 0.7;
constexpr int ATAQUES_COMBINADOS_MIN = 5;
constexpr int ATAQUES_COMBINADOS_MAX = 6;
constexpr bool ATAQUES_DOBLES = true;
constexpr bool ESCUDO_MONSTRUO = true;
constexpr double UMBRAL_ESCUDO_1 = 0.75;
constexpr double UMBRAL_ESCUDO_2 = 0.50;
constexpr double UMBRAL_ESCUDO_3 = 0.25;
constexpr double DURACION_ESCUDO = 10;
constexpr bool ESTALLIDO_ACTIVO = true;
constexpr int PUAS_MIN = 6;
constexpr int PUAS_MAX = 8;
constexpr double INTERVALO_ESTALLIDO = 6;
constexpr int DANIO_DISPARO = 1;
constexpr double ANGULO_DISPARO = 45;
constexpr double VELOCIDAD_MAX_DISPARO = 1800;
constexpr int NUM_ZONAS = 5;
constexpr double FACTOR_OLVIDO = 0.98;
constexpr double SUAVIZADO_MIN = 0.05;
constexpr double INTERVALO_CAMION_INICIAL = 3;
constexpr double VELOCIDAD_CAMION_INICIAL = 350;
constexpr double RAMPA_CAMIONES = 0.02;
constexpr double CALOR_MAX = 100;
constexpr double CALOR_POR_DISPARO = 8.5;
constexpr double TIEMPO_RECARGA_CALOR = 3.5;
constexpr double TIEMPO_BLOQUEO_GOLPE = 2;
constexpr double CADENCIA_BASE = 0.3;
constexpr double DURACION_BATERIA = 10;
constexpr double MULT_CADENCIA_BATERIA = 2;
constexpr double MULT_VELOCIDAD_BATERIA = 0.6;
constexpr double INTERVALO_BATERIA = 25;
}
#endif // DATOS_H
