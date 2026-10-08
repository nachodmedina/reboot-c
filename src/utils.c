#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "../include/utils.h"

static int coloresHabilitados = 1;

void inicializarColoresTerminal(void) {
#ifdef _WIN32
    HANDLE salida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo;
    if (salida != INVALID_HANDLE_VALUE && GetConsoleMode(salida, &modo) &&
        !SetConsoleMode(salida, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        coloresHabilitados = 0;
    }
#endif
}

const char *estiloTerminal(const char *codigo) {
    return coloresHabilitados ? codigo : "";
}

void mostrarCajaTerminal(const char *titulo, const char *lineas[], int cantidadLineas, const char *color) {
    const char *estilo = estiloTerminal(color);
    const char *reset = estiloTerminal(COLOR_RESET);

    printf("%s+----------------------------------------------------------+%s\n", estilo, reset);
    printf("%s| %-56.56s |%s\n", estilo, titulo, reset);
    printf("%s+----------------------------------------------------------+%s\n", estilo, reset);
    for (int i = 0; i < cantidadLineas; i++) {
        printf("%s| %-56.56s |%s\n", estilo, lineas[i], reset);
    }
    printf("%s+----------------------------------------------------------+%s\n", estilo, reset);
}

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
    printf("...");
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
