#ifndef ENEMIGOS_H
#define ENEMIGOS_H

#include "personajes.h"

typedef struct {
    char nombre[30];
    int vida;
    int vidaMax;
    int defensa;
    int ataque;
    Estado estadoActivo;
} Enemigo;

// Crea un enemigo con sus stats iniciales (definidos segun la zona).
Enemigo crearEnemigo(const char *nombre, int vidaMax, int defensa, int ataque);

// Muestra por consola los datos actuales del enemigo.
void mostrarEnemigo(const Enemigo *enemigo);

// Devuelve 1 si el enemigo sigue con vida, 0 si fue derrotado.
int estaVivoEnemigo(const Enemigo *enemigo);

// Aplica un nuevo estado alterado al enemigo.
void aplicarEstadoEnemigo(Enemigo *enemigo, Estado estado);

// Procesa el estado activo al final de un turno.
void actualizarEstadoEnemigo(Enemigo *enemigo);

#endif
