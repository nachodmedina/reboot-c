#include "../include/progresion.h"
#include <stddef.h>

enum {
    VIDA_JUGADOR_POR_MEJORA = 30,
    ATAQUE_JUGADOR_POR_MEJORA = 15,
    DEFENSA_JUGADOR_POR_MEJORA = 10,
    VIDA_ENEMIGO_POR_MEJORA = 15,
    ATAQUE_ENEMIGO_POR_MEJORA = 8,
    DEFENSA_ENEMIGO_POR_MEJORA = 5,
    CANT_PROTAGONISTAS = 3,
    CANT_ZONAS_PLANIFICADAS = 2
};

static const int aliadosPorRuta[CANT_PROTAGONISTAS][CANT_ZONAS_PLANIFICADAS] = {
    { INDICE_FRIEREN, INDICE_DANTE },
    { INDICE_FRIEREN, INDICE_ISAAC },
    { INDICE_ISAAC, INDICE_DANTE }
};

ProgresoPartida_t crearProgresoPartida(void) {
    ProgresoPartida_t progreso = { { 0, 0, 0 }, { 0, 0, 0 } };
    return progreso;
}

const char *nombreMejora(TipoMejora mejora) {
    switch (mejora) {
        case MEJORA_VIDA:
            return "vida";
        case MEJORA_ATAQUE:
            return "ataque";
        case MEJORA_DEFENSA:
            return "defensa";
    }
    return "desconocida";
}

int aumentoMejora(TipoMejora mejora) {
    switch (mejora) {
        case MEJORA_VIDA:
            return VIDA_JUGADOR_POR_MEJORA;
        case MEJORA_ATAQUE:
            return ATAQUE_JUGADOR_POR_MEJORA;
        case MEJORA_DEFENSA:
            return DEFENSA_JUGADOR_POR_MEJORA;
    }
    return 0;
}

const char *nombreContrabonus(TipoMejora mejora) {
    switch (mejora) {
        case MEJORA_VIDA:
            return "ataque";
        case MEJORA_ATAQUE:
            return "defensa";
        case MEJORA_DEFENSA:
            return "vida";
    }
    return "estadisticas";
}

int aumentoContrabonus(TipoMejora mejora) {
    switch (mejora) {
        case MEJORA_VIDA:
            return ATAQUE_ENEMIGO_POR_MEJORA;
        case MEJORA_ATAQUE:
            return DEFENSA_ENEMIGO_POR_MEJORA;
        case MEJORA_DEFENSA:
            return VIDA_ENEMIGO_POR_MEJORA;
    }
    return 0;
}

int contrabonusAcumulado(TipoMejora mejora, const ProgresoPartida_t *progreso_ptr) {
    if (progreso_ptr == NULL) {
        return 0;
    }
    switch (mejora) {
        case MEJORA_VIDA:
            return progreso_ptr->enemigos.ataque;
        case MEJORA_ATAQUE:
            return progreso_ptr->enemigos.defensa;
        case MEJORA_DEFENSA:
            return progreso_ptr->enemigos.vidaMaxima;
    }
    return 0;
}

static void sumarMejora(Personaje_t *personaje_ptr, TipoMejora mejora) {
    switch (mejora) {
        case MEJORA_VIDA:
            personaje_ptr->vidaMax += VIDA_JUGADOR_POR_MEJORA;
            if (personaje_ptr->vida > 0) {
                personaje_ptr->vida += VIDA_JUGADOR_POR_MEJORA;
            }
            break;
        case MEJORA_ATAQUE:
            personaje_ptr->ataque += ATAQUE_JUGADOR_POR_MEJORA;
            break;
        case MEJORA_DEFENSA:
            personaje_ptr->defensa += DEFENSA_JUGADOR_POR_MEJORA;
            break;
    }
}

int aplicarMejoraEquipo(Personaje_t equipo_ptr[], const int desbloqueados_ptr[],
                        int cantPersonajes, TipoMejora mejora,
                        ProgresoPartida_t *progreso_ptr) {
    if (equipo_ptr == NULL || desbloqueados_ptr == NULL || progreso_ptr == NULL ||
        cantPersonajes <= 0 || aumentoMejora(mejora) == 0) {
        return 0;
    }

    switch (mejora) {
        case MEJORA_VIDA:
            progreso_ptr->personajes.vidaMaxima += VIDA_JUGADOR_POR_MEJORA;
            progreso_ptr->enemigos.ataque += ATAQUE_ENEMIGO_POR_MEJORA;
            break;
        case MEJORA_ATAQUE:
            progreso_ptr->personajes.ataque += ATAQUE_JUGADOR_POR_MEJORA;
            progreso_ptr->enemigos.defensa += DEFENSA_ENEMIGO_POR_MEJORA;
            break;
        case MEJORA_DEFENSA:
            progreso_ptr->personajes.defensa += DEFENSA_JUGADOR_POR_MEJORA;
            progreso_ptr->enemigos.vidaMaxima += VIDA_ENEMIGO_POR_MEJORA;
            break;
    }

    for (int i = 0; i < cantPersonajes; i++) {
        if (desbloqueados_ptr[i]) {
            sumarMejora(&equipo_ptr[i], mejora);
        }
    }
    return 1;
}

int aplicarMejorasAcumuladas(Personaje_t *personaje_ptr, const ProgresoPartida_t *progreso_ptr) {
    if (personaje_ptr == NULL || progreso_ptr == NULL) {
        return 0;
    }

    personaje_ptr->vidaMax += progreso_ptr->personajes.vidaMaxima;
    if (personaje_ptr->vida > 0) {
        personaje_ptr->vida += progreso_ptr->personajes.vidaMaxima;
    }
    personaje_ptr->ataque += progreso_ptr->personajes.ataque;
    personaje_ptr->defensa += progreso_ptr->personajes.defensa;
    return 1;
}

int aplicarContrabonusEnemigos(Enemigo_t enemigos_ptr[], int cantEnemigos,
                               const ProgresoPartida_t *progreso_ptr) {
    if ((enemigos_ptr == NULL && cantEnemigos > 0) || progreso_ptr == NULL || cantEnemigos < 0) {
        return 0;
    }

    for (int i = 0; i < cantEnemigos; i++) {
        enemigos_ptr[i].vidaMax += progreso_ptr->enemigos.vidaMaxima;
        if (enemigos_ptr[i].vida > 0) {
            enemigos_ptr[i].vida += progreso_ptr->enemigos.vidaMaxima;
        }
        enemigos_ptr[i].ataque += progreso_ptr->enemigos.ataque;
        enemigos_ptr[i].defensa += progreso_ptr->enemigos.defensa;
    }
    return 1;
}

int aliadoQueSeUne(int indiceProtagonista, int numeroZona) {
    if (indiceProtagonista < 0 || indiceProtagonista >= CANT_PROTAGONISTAS ||
        numeroZona < 1 || numeroZona > CANT_ZONAS_PLANIFICADAS) {
        return -1;
    }
    return aliadosPorRuta[indiceProtagonista][numeroZona - 1];
}
