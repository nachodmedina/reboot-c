#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "personajes.h"
#include "items.h"
#include "progresion.h"

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


// Guarda equipo, desbloqueos, mejoras acumuladas e inventario compartido.
int guardarPartida(const char *nombreArchivo_ptr, const Personaje_t equipo_ptr[], int cantPersonajes,
                   const int desbloqueados_ptr[], const Inventario_t *inventario_ptr,
                   const ProgresoPartida_t *progreso_ptr);

// Carga los datos guardados; devuelve 1 si el archivo es valido.
int cargarPartida(const char *nombreArchivo_ptr, Personaje_t equipo_ptr[], int cantPersonajes,
                  int desbloqueados_ptr[], Inventario_t *inventario_ptr, ProgresoPartida_t *progreso_ptr);


// Borra el archivo de partida guardada. Pensada para el modo dificil,
// cuando el equipo pierde un combate (todavia no se llama desde ningun
// lado: se conecta cuando se implemente el bucle de combate real).
void eliminarPartida(void);

// Agrega una linea al archivo de log del juego (eventos importantes).
void registrarLog(const char *mensaje_ptr);

#endif
