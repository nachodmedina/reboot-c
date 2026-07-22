#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "personajes.h"

#define ARCHIVO_PARTIDA "partida.dat"
#define ARCHIVO_LOG "juego.log"

// Guarda el estado del equipo de personajes en un archivo binario.
void guardarPartida(const Personaje equipo[], int cantPersonajes);

// Carga el equipo de personajes desde el archivo de partida guardada.
// Devuelve 1 si pudo cargar, 0 si no habia partida guardada.
int cargarPartida(Personaje equipo[], int cantPersonajes);

// Agrega una linea al archivo de log del juego (eventos importantes).
void registrarLog(const char *mensaje);

#endif
