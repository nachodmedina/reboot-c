#include <stdio.h>
#include "../include/personajes.h"
#include "../include/utils.h"
#include "../include/archivos.h"

#define CANT_PERSONAJES 3

// TODO: reemplazar por el flujo real (eleccion de zona, combates, guardado automatico, etc.).
static void nuevaPartida(void) {
    Personaje equipo[CANT_PERSONAJES];
    equipo[0] = crearPersonaje("Isaac", 30, 4, 8);
    equipo[1] = crearPersonaje("Dante", 35, 6, 6);
    equipo[2] = crearPersonaje("Frieren", 25, 3, 10);

    printf("\nEquipo inicial:\n");
    for (int i = 0; i < CANT_PERSONAJES; i++) {
        mostrarPersonaje(&equipo[i]);
    }

    guardarPartida(equipo, CANT_PERSONAJES);
    printf("\nPartida nueva creada y guardada (modulo de juego en construccion).\n");
    pausar();
}

// TODO: una vez cargado el equipo, retomar la zona/historia donde quedo.
static void cargarPartidaExistente(void) {
    Personaje equipo[CANT_PERSONAJES];
    if (!cargarPartida(equipo, CANT_PERSONAJES)) {
        printf("\nNo hay ninguna partida guardada todavia.\n");
        pausar();
        return;
    }

    printf("\nPartida cargada:\n");
    for (int i = 0; i < CANT_PERSONAJES; i++) {
        mostrarPersonaje(&equipo[i]);
    }
    pausar();
}

static void mostrarMenuPrincipal(void) {
    printf("\n===== Reboot C =====\n");
    printf("1. Nueva partida\n");
    printf("2. Cargar partida\n");
    printf("3. Salir\n");
    printf("Elija una opcion: ");
}

int main(void) {
    inicializarRandom();

    int opcion;
    do {
        mostrarMenuPrincipal();
        opcion = leerOpcionMenu(1, 3);

        switch (opcion) {
            case 1:
                nuevaPartida();
                break;
            case 2:
                cargarPartidaExistente();
                break;
            case 3:
                printf("\nHasta la proxima.\n");
                break;
        }
    } while (opcion != 3);

    return 0;
}
