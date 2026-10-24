#include <stdio.h>
#include <string.h>
#include "../include/enemigos.h"
#include "../include/utils.h"

Enemigo_t crearEnemigo(const char *nombre_ptr, TipoHabilidadEnemigo habilidad, int vidaMax, int defensa, int ataque) {
    Enemigo_t enemigo;
    strncpy(enemigo.nombre, nombre_ptr, sizeof(enemigo.nombre) - 1);
    enemigo.nombre[sizeof(enemigo.nombre) - 1] = '\0';
    enemigo.habilidad = habilidad;
    enemigo.vida = vidaMax;
    enemigo.vidaMax = vidaMax;
    enemigo.defensa = defensa;
    enemigo.ataque = ataque;
    enemigo.estadoActivo.tipo = ESTADO_NINGUNO;
    enemigo.estadoActivo.duracionTurnos = 0;
    enemigo.estadoActivo.magnitud = 0;
    return enemigo;
}

void mostrarEnemigo(const Enemigo_t *enemigo_ptr) {
    printf("%s%s%s%s - Vida: %d/%d - Ataque: %d - Defensa: %d%s\n",
           estiloTerminal(COLOR_ROJO), estiloTerminal(ESTILO_NEGRITA),
           enemigo_ptr->nombre, estiloTerminal(COLOR_ROJO),
           enemigo_ptr->vida, enemigo_ptr->vidaMax, enemigo_ptr->ataque,
           enemigo_ptr->defensa, estiloTerminal(COLOR_RESET));
}

int estaVivoEnemigo(const Enemigo_t *enemigo_ptr) {
    return enemigo_ptr->vida > 0;
}

void aplicarEstadoEnemigo(Enemigo_t *enemigo_ptr, Estado_t estado) {
    enemigo_ptr->estadoActivo = estado;
}

int aplicarVenenoEnemigo(Enemigo_t *enemigo_ptr, int danoPorTurno, int duracionTurnos) {
    if (enemigo_ptr == NULL || !estaVivoEnemigo(enemigo_ptr) || danoPorTurno <= 0 ||
        duracionTurnos <= 0 ||
        (enemigo_ptr->estadoActivo.tipo != ESTADO_NINGUNO &&
         enemigo_ptr->estadoActivo.tipo != ESTADO_VENENO)) {
        return 0;
    }

    Estado_t veneno = { ESTADO_VENENO, duracionTurnos, danoPorTurno };
    enemigo_ptr->estadoActivo = veneno;
    return 1;
}

void actualizarEstadoEnemigo(Enemigo_t *enemigo_ptr) {
    if (enemigo_ptr->estadoActivo.tipo == ESTADO_VENENO && enemigo_ptr->estadoActivo.duracionTurnos > 0) {
        enemigo_ptr->vida -= enemigo_ptr->estadoActivo.magnitud;
        if (enemigo_ptr->vida < 0) {
            enemigo_ptr->vida = 0;
        }
        printf("%s recibe %d de dano por veneno (vida %d/%d).\n",
               enemigo_ptr->nombre, enemigo_ptr->estadoActivo.magnitud,
               enemigo_ptr->vida, enemigo_ptr->vidaMax);
    }
    if (enemigo_ptr->estadoActivo.tipo != ESTADO_NINGUNO && enemigo_ptr->estadoActivo.duracionTurnos > 0) {
        enemigo_ptr->estadoActivo.duracionTurnos--;
        if (enemigo_ptr->estadoActivo.duracionTurnos == 0) {
            enemigo_ptr->estadoActivo.tipo = ESTADO_NINGUNO;
        }
    }
}
