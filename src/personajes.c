#include <stdio.h>
#include <string.h>
#include "../include/personajes.h"

Personaje_t crearPersonaje(const char *nombre_ptr, const char *arma_ptr, TipoHabilidadEspecial habilidad, int vidaMax, int defensa, int ataque) {
    Personaje_t personaje;
    strncpy(personaje.nombre, nombre_ptr, sizeof(personaje.nombre) - 1);
    personaje.nombre[sizeof(personaje.nombre) - 1] = '\0';
    strncpy(personaje.arma, arma_ptr, sizeof(personaje.arma) - 1);
    personaje.arma[sizeof(personaje.arma) - 1] = '\0';
    personaje.habilidadEspecial = habilidad;
    personaje.vida = vidaMax;
    personaje.vidaMax = vidaMax;
    personaje.defensa = defensa;
    personaje.ataque = ataque;
    personaje.estadoActivo.tipo = ESTADO_NINGUNO;
    personaje.estadoActivo.duracionTurnos = 0;
    personaje.estadoActivo.magnitud = 0;
    return personaje;
}

void mostrarPersonaje(const Personaje_t *personaje_ptr) {
    // TODO: mejorar formato de impresion cuando este definido el combate.
    printf("%s (%s) - Vida: %d/%d - Ataque: %d - Defensa: %d\n",
           personaje_ptr->nombre, personaje_ptr->arma, personaje_ptr->vida, personaje_ptr->vidaMax,
           personaje_ptr->ataque, personaje_ptr->defensa);
}

int estaVivoPersonaje(const Personaje_t *personaje_ptr) {
    return personaje_ptr->vida > 0;
}

void aplicarEstadoPersonaje(Personaje_t *personaje_ptr, Estado_t estado) {
    // TODO: definir reglas cuando ya hay un estado activo (se reemplaza o se acumula).
    personaje_ptr->estadoActivo = estado;
}

void actualizarEstadoPersonaje(Personaje_t *personaje_ptr) {
    // Aturdido y debilitado los aplica combate.c durante el turno; aca solo el veneno.
    if (personaje_ptr->estadoActivo.tipo == ESTADO_VENENO && personaje_ptr->estadoActivo.duracionTurnos > 0) {
        personaje_ptr->vida -= personaje_ptr->estadoActivo.magnitud;
        if (personaje_ptr->vida < 0) {
            personaje_ptr->vida = 0;
        }
    }
    if (personaje_ptr->estadoActivo.tipo != ESTADO_NINGUNO && personaje_ptr->estadoActivo.duracionTurnos > 0) {
        personaje_ptr->estadoActivo.duracionTurnos--;
        if (personaje_ptr->estadoActivo.duracionTurnos == 0) {
            personaje_ptr->estadoActivo.tipo = ESTADO_NINGUNO;
        }
    }
}
