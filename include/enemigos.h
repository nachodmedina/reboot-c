#ifndef ENEMIGOS_H
#define ENEMIGOS_H

#include "personajes.h"

typedef enum {
    HABILIDAD_ENEMIGO_NINGUNA,
    HABILIDAD_ENEMIGO_TRAMPA,   // el personaje golpeado queda debilitado 1 turno
    HABILIDAD_ENEMIGO_CRITICO   // chance de que el golpe haga x1.5 de dano
} TipoHabilidadEnemigo;

typedef struct {
    char nombre[30];
    TipoHabilidadEnemigo habilidad;
    int esJefe;             // 1 si es el jefe de la zona (se muestra en amarillo)
    int vida;
    int vidaMax;
    int defensa;
    int ataque;
    Estado_t estadoActivo;
} Enemigo_t;

// Crea un enemigo comun con sus stats iniciales (definidos segun la zona).
Enemigo_t crearEnemigo(const char *nombre_ptr, TipoHabilidadEnemigo habilidad, int vidaMax, int defensa, int ataque);

// Igual que crearEnemigo, pero marcado como jefe.
Enemigo_t crearJefe(const char *nombre_ptr, TipoHabilidadEnemigo habilidad, int vidaMax, int defensa, int ataque);

// Indica si el enemigo es un jefe.
int esJefeEnemigo(const Enemigo_t *enemigo_ptr);

// Muestra por consola los datos actuales del enemigo.
void mostrarEnemigo(const Enemigo_t *enemigo_ptr);

// Devuelve 1 si el enemigo sigue con vida, 0 si fue derrotado.
int estaVivoEnemigo(const Enemigo_t *enemigo_ptr);

// Aplica un nuevo estado alterado al enemigo.
void aplicarEstadoEnemigo(Enemigo_t *enemigo_ptr, Estado_t estado);

// Aplica o renueva veneno; no reemplaza otro estado alterado.
int aplicarVenenoEnemigo(Enemigo_t *enemigo_ptr, int danoPorTurno, int duracionTurnos);

// Procesa el estado activo al final de un turno.
void actualizarEstadoEnemigo(Enemigo_t *enemigo_ptr);

#endif
