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
    int vida;
    int vidaMax;
    int defensa;
    int ataque;
    Estado estadoActivo;
} Enemigo;

// Crea un enemigo con sus stats iniciales (definidos segun la zona).
Enemigo crearEnemigo(const char *nombre, TipoHabilidadEnemigo habilidad, int vidaMax, int defensa, int ataque);

// Muestra por consola los datos actuales del enemigo.
void mostrarEnemigo(const Enemigo *enemigo);

// Devuelve 1 si el enemigo sigue con vida, 0 si fue derrotado.
int estaVivoEnemigo(const Enemigo *enemigo);

// Aplica un nuevo estado alterado al enemigo.
void aplicarEstadoEnemigo(Enemigo *enemigo, Estado estado);

// Procesa el estado activo al final de un turno.
void actualizarEstadoEnemigo(Enemigo *enemigo);

#endif
