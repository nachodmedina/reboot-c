#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "personajes.h"

#define ARCHIVO_PARTIDA "partida.dat"
#define ARCHIVO_LOG "juego.log"

// Modo de juego elegido por el jugador al arrancar una partida nueva.
// MODO_NORMAL  : se puede guardar y cargar la partida sin restricciones.
// MODO_DIFICIL : si el equipo es derrotado en combate, se borra la partida
//                guardada (permadeath) -- no se puede continuar esa partida.
typedef enum {
    MODO_NORMAL,
    MODO_DIFICIL
} ModoJuego;


// Guarda el estado del equipo de personajes en un archivo binario.
void guardarPartida(const Personaje equipo[], int cantPersonajes);

// Carga el equipo de personajes desde el archivo de partida guardada.
// Devuelve 1 si pudo cargar, 0 si no habia partida guardada.
int cargarPartida(Personaje equipo[], int cantPersonajes);


// Borra el archivo de partida guardada. Pensada para el modo dificil,
// cuando el equipo pierde un combate (todavia no se llama desde ningun
// lado: se conecta cuando se implemente el bucle de combate real).
void eliminarPartida(void);

// Agrega una linea al archivo de log del juego (eventos importantes).
void registrarLog(const char *mensaje);

#endif
