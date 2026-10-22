#ifndef PROGRESION_H
#define PROGRESION_H

#include "personajes.h"
#include "enemigos.h"

typedef enum {
    MEJORA_VIDA = 1,
    MEJORA_ATAQUE,
    MEJORA_DEFENSA
} TipoMejora;

enum {
    INDICE_ISAAC = 0,
    INDICE_DANTE,
    INDICE_FRIEREN
};

typedef struct {
    int vidaMaxima;
    int ataque;
    int defensa;
} BonificacionesPersonaje_t;

typedef struct {
    int vidaMaxima;
    int ataque;
    int defensa;
} ContrabonusEnemigo_t;

typedef struct {
    BonificacionesPersonaje_t personajes;
    ContrabonusEnemigo_t enemigos;
} ProgresoPartida_t;

ProgresoPartida_t crearProgresoPartida(void);

const char *nombreMejora(TipoMejora mejora);
int aumentoMejora(TipoMejora mejora);
const char *nombreContrabonus(TipoMejora mejora);
int aumentoContrabonus(TipoMejora mejora);
int contrabonusAcumulado(TipoMejora mejora, const ProgresoPartida_t *progreso_ptr);

// Aplica una mejora a todos los personajes desbloqueados y acumula sus valores.
int aplicarMejoraEquipo(Personaje_t equipo_ptr[], const int desbloqueados_ptr[],
                        int cantPersonajes, TipoMejora mejora,
                        ProgresoPartida_t *progreso_ptr);

// Aplica a un nuevo aliado las mejoras elegidas antes de su desbloqueo.
int aplicarMejorasAcumuladas(Personaje_t *personaje_ptr, const ProgresoPartida_t *progreso_ptr);

// Aplica los contrabonus acumulados a una copia de enemigos de una zona futura.
int aplicarContrabonusEnemigos(Enemigo_t enemigos_ptr[], int cantEnemigos,
                               const ProgresoPartida_t *progreso_ptr);

// Devuelve el indice del aliado de esa ruta, o -1 si la ruta no es valida.
int aliadoQueSeUne(int indiceProtagonista, int numeroZona);

#endif
