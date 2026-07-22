#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/utils.h"

void inicializarRandom(void) {
    srand((unsigned int) time(NULL));
}

int numeroAleatorio(int min, int max) {
    return min + rand() % (max - min + 1);
}

void limpiarBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // descarta caracteres sobrantes
    }
}

void pausar(void) {
    printf("Presione Enter para continuar...");
    limpiarBufferEntrada();
}

int leerOpcionMenu(int min, int max) {
    int opcion;
    while (1) {
        int resultado = scanf("%d", &opcion);
        if (resultado == EOF) {
            // No queda mas entrada para leer (p.ej. Ctrl+D): cerramos el juego.
            printf("\nEntrada finalizada. Cerrando el juego.\n");
            exit(0);
        }
        if (resultado == 1 && opcion >= min && opcion <= max) {
            limpiarBufferEntrada();
            return opcion;
        }
        limpiarBufferEntrada();
        printf("Opcion invalida. Intente de nuevo: ");
    }
}
