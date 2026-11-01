#ifndef ZONAS_H
#define ZONAS_H

#include "combate.h"

#define MAX_FASES_ZONA 4

// Una fase es un combate dentro de una zona.
typedef struct {
    char amenaza[150];      // texto para mostrarEncuentroEnemigos
    Batalla_t batalla;      // enemigos en su estado inicial (vida llena)
    int fogataAntes;        // 1 si los luchadores descansan antes de esta fase
} Fase_t;

// Una zona agrupa los combates de una etapa del juego.
typedef struct {
    int numero;                    // en el orden en que se juegan
    char nombre[30];
    char descripcion[200];
    char recorrido[60];            // por donde avanza el equipo ("entre las ruinas del pueblo")
    Fase_t fases[MAX_FASES_ZONA];  // en el orden en que se juegan
    int cantFases;                 // cuantos lugares de "fases" se usan
} Zona_t;

// Crea la zona con ese numero con todas sus fases (por ahora solo existe la 1).
Zona_t crearZonaPorNumero(int numero);

#endif
