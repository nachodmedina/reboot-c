#include <stdio.h>
#include <string.h>
#include "../include/personajes.h"

Personaje crearPersonaje(const char *nombre, const char *arma, TipoHabilidadEspecial habilidad, int vidaMax, int defensa, int ataque) {
    Personaje personaje;
    strncpy(personaje.nombre, nombre, sizeof(personaje.nombre) - 1);
    personaje.nombre[sizeof(personaje.nombre) - 1] = '\0';
    strncpy(personaje.arma, arma, sizeof(personaje.arma) - 1);
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

void mostrarPersonaje(const Personaje *personaje) {
    // TODO: mejorar formato de impresion cuando este definido el combate.
    printf("%s (%s) - Vida: %d/%d - Ataque: %d - Defensa: %d\n",
           personaje->nombre, personaje->arma, personaje->vida, personaje->vidaMax,
           personaje->ataque, personaje->defensa);
}

int estaVivoPersonaje(const Personaje *personaje) {
    return personaje->vida > 0;
}

void aplicarEstadoPersonaje(Personaje *personaje, Estado estado) {
    // TODO: definir reglas cuando ya hay un estado activo (se reemplaza o se acumula).
    personaje->estadoActivo = estado;
}

void actualizarEstadoPersonaje(Personaje *personaje) {
    // TODO: implementar efecto de cada TipoEstado (dano por veneno, saltear turno, etc.).
    if (personaje->estadoActivo.tipo != ESTADO_NINGUNO && personaje->estadoActivo.duracionTurnos > 0) {
        personaje->estadoActivo.duracionTurnos--;
        if (personaje->estadoActivo.duracionTurnos == 0) {
            personaje->estadoActivo.tipo = ESTADO_NINGUNO;
        }
    }
}
