#include <stdio.h>
#include <string.h>
#include "../include/enemigos.h"

Enemigo crearEnemigo(const char *nombre, int vidaMax, int defensa, int ataque) {
    Enemigo enemigo;
    strncpy(enemigo.nombre, nombre, sizeof(enemigo.nombre) - 1);
    enemigo.nombre[sizeof(enemigo.nombre) - 1] = '\0';
    enemigo.vida = vidaMax;
    enemigo.vidaMax = vidaMax;
    enemigo.defensa = defensa;
    enemigo.ataque = ataque;
    enemigo.estadoActivo.tipo = ESTADO_NINGUNO;
    enemigo.estadoActivo.duracionTurnos = 0;
    enemigo.estadoActivo.magnitud = 0;
    return enemigo;
}

void mostrarEnemigo(const Enemigo *enemigo) {
    printf("%s - Vida: %d/%d - Ataque: %d - Defensa: %d\n",
           enemigo->nombre, enemigo->vida, enemigo->vidaMax,
           enemigo->ataque, enemigo->defensa);
}

int estaVivoEnemigo(const Enemigo *enemigo) {
    return enemigo->vida > 0;
}

void aplicarEstadoEnemigo(Enemigo *enemigo, Estado estado) {
    enemigo->estadoActivo = estado;
}

void actualizarEstadoEnemigo(Enemigo *enemigo) {
    // TODO: implementar efecto de cada TipoEstado, igual que en personajes.c.
    if (enemigo->estadoActivo.tipo != ESTADO_NINGUNO && enemigo->estadoActivo.duracionTurnos > 0) {
        enemigo->estadoActivo.duracionTurnos--;
        if (enemigo->estadoActivo.duracionTurnos == 0) {
            enemigo->estadoActivo.tipo = ESTADO_NINGUNO;
        }
    }
}
