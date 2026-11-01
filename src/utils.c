#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <windows.h>
#endif
#include "../include/utils.h"

#define ANCHO_CAJA 68
#define ANCHO_CAJA_BYTES (ANCHO_CAJA * 4 + 1)
#define ANCHO_BORDE_CAJA (ANCHO_CAJA + 2)

static int coloresHabilitados = 1;
static int cursorHabilitado = 0;

void inicializarColoresTerminal(void) {
#ifdef _WIN32
    HANDLE salida_ptr = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE entrada_ptr = GetStdHandle(STD_INPUT_HANDLE);
    DWORD modoSalida;
    DWORD modoEntrada;

    coloresHabilitados = 0;
    cursorHabilitado = 0;
    if (salida_ptr != INVALID_HANDLE_VALUE &&
        GetConsoleMode(salida_ptr, &modoSalida)) {
        SetConsoleOutputCP(CP_UTF8);

        if (SetConsoleMode(salida_ptr, modoSalida | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            coloresHabilitados = 1;
            if (entrada_ptr != INVALID_HANDLE_VALUE &&
                GetConsoleMode(entrada_ptr, &modoEntrada) &&
                _isatty(_fileno(stdin))) {
                cursorHabilitado = 1;
            }
        }
    }
#endif
}

const char *estiloTerminal(const char *codigo_ptr) {
    return coloresHabilitados ? codigo_ptr : "";
}

static int cantidadCaracteresUtf8(const char *texto_ptr) {
    int cantidad = 0;
    for (const unsigned char *caracter_ptr = (const unsigned char *) texto_ptr;
         *caracter_ptr != '\0'; caracter_ptr++) {
        if ((*caracter_ptr & 0xC0) != 0x80) {
            cantidad++;
        }
    }
    return cantidad;
}

static void imprimirLineaCaja(const char *linea_ptr, int ancho, const char *color_ptr) {
    int caracteres = cantidadCaracteresUtf8(linea_ptr);
    const char *colorAplicado_ptr = estiloTerminal(color_ptr);
    const char *reset_ptr = estiloTerminal(COLOR_RESET);

    printf("%s║ %s", colorAplicado_ptr, linea_ptr);
    for (int i = caracteres; i < ancho; i++) {
        printf(" ");
    }
    printf(" ║%s\n", reset_ptr);
}

static void imprimirTituloCaja(const char *titulo_ptr, const char *colorBorde_ptr,
                               const char *colorTitulo_ptr, const char *estiloTitulo_ptr) {
    int caracteres = cantidadCaracteresUtf8(titulo_ptr);
    const char *colorBordeAplicado_ptr = estiloTerminal(colorBorde_ptr);

    printf("%s║ %s%s%s%s%s", colorBordeAplicado_ptr,
           estiloTerminal(colorTitulo_ptr), estiloTerminal(estiloTitulo_ptr),
           titulo_ptr, estiloTerminal(COLOR_RESET), colorBordeAplicado_ptr);
    for (int i = caracteres; i < ANCHO_CAJA; i++) {
        printf(" ");
    }
    printf(" ║%s\n", estiloTerminal(COLOR_RESET));
}

static void imprimirLineaAjustada(char *linea_ptr, int ancho, const char *color_ptr) {
    imprimirLineaCaja(linea_ptr, ancho, color_ptr);
    linea_ptr[0] = '\0';
}

static void imprimirBordeCaja(const char *esquinaIzquierda_ptr,
                              const char *esquinaDerecha_ptr,
                              const char *color_ptr) {
    printf("%s%s", estiloTerminal(color_ptr), esquinaIzquierda_ptr);
    for (int i = 0; i < ANCHO_BORDE_CAJA; i++) {
        printf("═");
    }
    printf("%s%s\n", esquinaDerecha_ptr, estiloTerminal(COLOR_RESET));
}

static void imprimirTextoAjustado(const char *texto_ptr, int ancho, const char *color_ptr) {
    char linea[ANCHO_CAJA_BYTES];
    char palabra[ANCHO_CAJA_BYTES];
    int longitudLinea = 0;
    const char *actual_ptr = texto_ptr;
    linea[0] = '\0';

    while (*actual_ptr != '\0') {
        if (*actual_ptr == '\n') {
            imprimirLineaAjustada(linea, ancho, color_ptr);
            longitudLinea = 0;
            actual_ptr++;
            continue;
        }

        while (*actual_ptr == ' ' || *actual_ptr == '\t') {
            actual_ptr++;
        }
        if (*actual_ptr == '\0') {
            break;
        }
        if (*actual_ptr == '\n') {
            continue;
        }

        size_t longitudPalabra = 0;
        int caracteresPalabra = 0;
        while (*actual_ptr != '\0' && *actual_ptr != '\n' &&
               *actual_ptr != ' ' && *actual_ptr != '\t') {
            size_t bytesCaracter = 1;
            unsigned char byte = (unsigned char) *actual_ptr;
            if ((byte & 0xE0) == 0xC0) {
                bytesCaracter = 2;
            } else if ((byte & 0xF0) == 0xE0) {
                bytesCaracter = 3;
            } else if ((byte & 0xF8) == 0xF0) {
                bytesCaracter = 4;
            }
            if (longitudPalabra + bytesCaracter >= sizeof(palabra)) {
                break;
            }
            memcpy(&palabra[longitudPalabra], actual_ptr, bytesCaracter);
            longitudPalabra += bytesCaracter;
            actual_ptr += bytesCaracter;
            caracteresPalabra++;
        }
        palabra[longitudPalabra] = '\0';

        if (longitudLinea > 0 && longitudLinea + 1 + caracteresPalabra > ancho) {
            imprimirLineaAjustada(linea, ancho, color_ptr);
            longitudLinea = 0;
        }
        if (caracteresPalabra > ancho) {
            const char *fragmento_ptr = palabra;
            while (*fragmento_ptr != '\0') {
                int caracteresFragmento = 0;
                size_t bytesFragmento = 0;
                while (fragmento_ptr[bytesFragmento] != '\0' && caracteresFragmento < ancho) {
                    unsigned char byte = (unsigned char) fragmento_ptr[bytesFragmento];
                    size_t bytesCaracter = (byte & 0xF8) == 0xF0 ? 4 :
                                           (byte & 0xF0) == 0xE0 ? 3 :
                                           (byte & 0xE0) == 0xC0 ? 2 : 1;
                    bytesFragmento += bytesCaracter;
                    caracteresFragmento++;
                }
                memcpy(linea, fragmento_ptr, bytesFragmento);
                linea[bytesFragmento] = '\0';
                imprimirLineaAjustada(linea, ancho, color_ptr);
                fragmento_ptr += bytesFragmento;
            }
            longitudLinea = 0;
            continue;
        }

        size_t bytesLinea = strlen(linea);
        if (longitudLinea > 0) {
            linea[bytesLinea++] = ' ';
            linea[bytesLinea] = '\0';
            longitudLinea++;
        }
        memcpy(&linea[bytesLinea], palabra, longitudPalabra + 1);
        longitudLinea += caracteresPalabra;
    }

    if (longitudLinea > 0 || texto_ptr[0] == '\0' || texto_ptr[strlen(texto_ptr) - 1] == '\n') {
        imprimirLineaCaja(linea, ancho, color_ptr);
    }
}

void mostrarTextoEnCaja(const char *titulo_ptr, const char *texto_ptr, const char *color_ptr) {
    mostrarTextoEnCajaConTituloEstilizado(titulo_ptr, texto_ptr, color_ptr,
                                          color_ptr, "");
}

void mostrarTextoEnCajaConTituloEstilizado(const char *titulo_ptr, const char *texto_ptr,
                                          const char *color_ptr,
                                          const char *colorTitulo_ptr,
                                          const char *estiloTitulo_ptr) {
    int mostrarTitulo = titulo_ptr != NULL && titulo_ptr[0] != '\0';

    imprimirBordeCaja("╔", "╗", color_ptr);
    if (mostrarTitulo) {
        imprimirTituloCaja(titulo_ptr, color_ptr, colorTitulo_ptr, estiloTitulo_ptr);
        imprimirBordeCaja("╠", "╣", color_ptr);
    }
    imprimirTextoAjustado(texto_ptr, ANCHO_CAJA, color_ptr);
    imprimirBordeCaja("╚", "╝", color_ptr);
}

void mostrarArteASCII(const char *const lineas_ptr[], int cantidadLineas) {
    for (int i = 0; i < cantidadLineas; i++) {
        printf("%s%s%s\n", estiloTerminal(COLOR_BLANCO), lineas_ptr[i],
               estiloTerminal(COLOR_RESET));
    }
}

void limpiarPantallaTerminal(void) {
    if (coloresHabilitados) {
        printf("\033[2J\033[H");
        fflush(stdout);
    }
#ifdef _WIN32
    else {
        system("cls");
    }
#else
    else {
        for (int i = 0; i < 40; i++) {
            putchar('\n');
        }
    }
#endif
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
    printf("\n");
    for (int i = 0; i < 3; i++) {
        putchar('.');
        fflush(stdout);
#ifdef _WIN32
        Sleep(250);
#else
        clock_t inicio = clock();
        while (inicio != (clock_t) -1 &&
               clock() - inicio < CLOCKS_PER_SEC / 4) {
        }
#endif
    }
    limpiarBufferEntrada();
    putchar('\n');
}

#ifdef _WIN32
// Menu con flechas: solo existe en la consola de Windows.
static void mostrarOpcionesMenu(const char *opciones_ptr[], int cantidadOpciones, int seleccionado) {
    for (int i = 0; i < cantidadOpciones; i++) {
        if (i == seleccionado) {
            printf("%s%s> %s%s\n", estiloTerminal(COLOR_AMARILLO),
                   estiloTerminal(ESTILO_NEGRITA),
                   opciones_ptr[i], estiloTerminal(COLOR_RESET));
        } else {
            printf("%s  %s%s\n", estiloTerminal(COLOR_CYAN), opciones_ptr[i],
                   estiloTerminal(COLOR_RESET));
        }
    }
    printf("Flechas para elegir, Enter para confirmar: ");
    fflush(stdout);
}
#endif

static int leerOpcionNumerica(int cantidadOpciones) {
    int opcion;
    while (1) {
        int resultado = scanf("%d", &opcion);
        if (resultado == EOF) {
            // No queda mas entrada para leer (p.ej. Ctrl+D): cerramos el juego.
            printf("\nEntrada finalizada. Cerrando el juego.\n");
            exit(0);
        }
        if (resultado == 1 && opcion >= 1 && opcion <= cantidadOpciones) {
            limpiarBufferEntrada();
            return opcion - 1;
        }
        limpiarBufferEntrada();
        printf("Opcion invalida. Intente de nuevo: ");
    }
}

int leerOpcionMenu(const char *opciones_ptr[], int cantidadOpciones) {
    if (opciones_ptr == NULL || cantidadOpciones <= 0) {
        return -1;
    }

    if (!cursorHabilitado) {
        for (int i = 0; i < cantidadOpciones; i++) {
            printf("%s%d. %s%s\n", estiloTerminal(COLOR_CYAN), i + 1,
                   opciones_ptr[i], estiloTerminal(COLOR_RESET));
        }
        printf("Elija una opcion: ");
        return leerOpcionNumerica(cantidadOpciones);
    }

#ifdef _WIN32
    int seleccionado = 0;
    printf("\033[s");
    mostrarOpcionesMenu(opciones_ptr, cantidadOpciones, seleccionado);
    while (1) {
        int tecla = _getch();
        if (tecla == 0 || tecla == 224) {
            tecla = _getch();
            if (tecla == 72) {
                seleccionado = (seleccionado + cantidadOpciones - 1) % cantidadOpciones;
            } else if (tecla == 80) {
                seleccionado = (seleccionado + 1) % cantidadOpciones;
            } else {
                continue;
            }
            printf("\033[u\033[J\033[s");
            mostrarOpcionesMenu(opciones_ptr, cantidadOpciones, seleccionado);
        } else if (tecla == 13) {
            printf("\n");
            return seleccionado;
        } else if (tecla >= '1' && tecla <= '9' &&
                   tecla - '1' < cantidadOpciones) {
            printf("\n");
            return tecla - '1';
        }
    }
#else
    for (int i = 0; i < cantidadOpciones; i++) {
        printf("%s%d. %s%s\n", estiloTerminal(COLOR_CYAN), i + 1,
               opciones_ptr[i], estiloTerminal(COLOR_RESET));
    }
    printf("Elija una opcion: ");
    return leerOpcionNumerica(cantidadOpciones);
#endif
}
