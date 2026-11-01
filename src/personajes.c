#include <stdio.h>
#include <string.h>
#include "../include/personajes.h"
#include "../include/utils.h"

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
    printf("%s%s%s%s (%s) - Vida: %d/%d - Ataque: %d - Defensa: %d%s\n",
           estiloTerminal(COLOR_VERDE), estiloTerminal(ESTILO_NEGRITA),
           personaje_ptr->nombre, estiloTerminal(COLOR_VERDE), personaje_ptr->arma,
           personaje_ptr->vida, personaje_ptr->vidaMax, personaje_ptr->ataque,
           personaje_ptr->defensa, estiloTerminal(COLOR_RESET));
}

int estaVivoPersonaje(const Personaje_t *personaje_ptr) {
    return personaje_ptr->vida > 0;
}

void aplicarEstadoPersonaje(Personaje_t *personaje_ptr, Estado_t estado) {
    // Solo hay un estado activo: el nuevo reemplaza al anterior.
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
