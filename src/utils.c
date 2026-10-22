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

const char *estiloTerminal(const char *codigo_ptr) {
    return coloresHabilitados ? codigo_ptr : "";
}

void mostrarCajaTerminal(const char *titulo_ptr, const char *lineas_ptr[], int cantidadLineas, const char *color_ptr) {
    const char *estilo_ptr = estiloTerminal(color_ptr);
    const char *reset_ptr = estiloTerminal(COLOR_RESET);

    printf("%s+----------------------------------------------------------+%s\n", estilo_ptr, reset_ptr);
    printf("%s| %-56.56s |%s\n", estilo_ptr, titulo_ptr, reset_ptr);
    printf("%s+----------------------------------------------------------+%s\n", estilo_ptr, reset_ptr);
    for (int i = 0; i < cantidadLineas; i++) {
        printf("%s| %-56.56s |%s\n", estilo_ptr, lineas_ptr[i], reset_ptr);
    }
    printf("%s+----------------------------------------------------------+%s\n", estilo_ptr, reset_ptr);
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
