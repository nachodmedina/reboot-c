#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "estadisticas.h"
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

// En que punto de la historia esta la partida, para poder retomarla.
typedef struct {
    ModoJuego modo;
    int zonaActual;     // 1..CANT_ZONAS
    int faseSiguiente;  // indice de la proxima fase a jugar dentro de zonaActual
} EstadoPartida_t;

// Guarda el punto de la historia, equipo, desbloqueos, mejoras acumuladas,
// inventario compartido y estadisticas. Reemplaza la partida anterior.
int guardarPartida(const char *nombreArchivo_ptr, const EstadoPartida_t *estado_ptr,
                   const Personaje_t equipo_ptr[], int cantPersonajes,
                   const int desbloqueados_ptr[], const Inventario_t *inventario_ptr,
                   const ProgresoPartida_t *progreso_ptr, const Estadisticas_t *estadisticas_ptr);

// Carga los datos guardados; devuelve 1 si el archivo es valido. Si no lo es,
// no cambia nada. estadisticas_ptr debe estar inicializado (vacio o con
// crearEstadisticas): su memoria se reemplaza por la de los registros cargados.
int cargarPartida(const char *nombreArchivo_ptr, EstadoPartida_t *estado_ptr,
                  Personaje_t equipo_ptr[], int cantPersonajes,
                  int desbloqueados_ptr[], Inventario_t *inventario_ptr, ProgresoPartida_t *progreso_ptr,
                  Estadisticas_t *estadisticas_ptr);


// Borra el archivo de partida guardada. Se usa en el modo dificil
// cuando el equipo pierde un combate.
void eliminarPartida(void);

// Agrega una linea al archivo de log del juego (eventos importantes).
void registrarLog(const char *mensaje_ptr);

#endif
