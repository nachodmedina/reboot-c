#include <stdio.h>
#include <time.h>
#include "../include/archivos.h"

void guardarPartida(const Personaje equipo[], int cantPersonajes) {
    // TODO: guardar tambien la zona actual y el inventario, no solo el equipo.
    FILE *archivo = fopen(ARCHIVO_PARTIDA, "wb");
    if (archivo == NULL) {
        printf("No se pudo guardar la partida.\n");
        return;
    }
    fwrite(equipo, sizeof(Personaje), (size_t) cantPersonajes, archivo);
    fclose(archivo);
    registrarLog("Partida guardada");
}

int cargarPartida(Personaje equipo[], int cantPersonajes) {
    FILE *archivo = fopen(ARCHIVO_PARTIDA, "rb");
    if (archivo == NULL) {
        return 0;
    }
    size_t leidos = fread(equipo, sizeof(Personaje), (size_t) cantPersonajes, archivo);
    fclose(archivo);
    if (leidos == (size_t) cantPersonajes) {
        registrarLog("Partida cargada");
        return 1;
    }
    return 0;
}

void eliminarPartida(void) {
    // remove() esta declarada en <stdio.h> (la misma libreria de fopen/fclose).
    // Recibe el nombre del archivo y lo borra del disco.
    // Devuelve 0 si pudo borrarlo, y un valor distinto de 0 si hubo un error
    // (por ejemplo, si el archivo no existia).
    if (remove(ARCHIVO_PARTIDA) == 0) {
        registrarLog("Partida eliminada (modo dificil)");
    }
}
void registrarLog(const char *mensaje) {
    FILE *archivo = fopen(ARCHIVO_LOG, "a");
    if (archivo == NULL) {
        return;
    }
    time_t ahora = time(NULL);
    char fechaTexto[26];
    snprintf(fechaTexto, sizeof(fechaTexto), "%s", ctime(&ahora));
    fechaTexto[24] = '\0'; // quitar el salto de linea que agrega ctime
    fprintf(archivo, "[%s] %s\n", fechaTexto, mensaje);
    fclose(archivo);
}
