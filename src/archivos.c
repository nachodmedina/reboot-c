#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "../include/archivos.h"

static const unsigned char FIRMA_PARTIDA[8] = { 'R', 'E', 'B', 'O', 'O', 'T', 'C', 0 };
static const uint32_t VERSION_PARTIDA = 1;

static int reemplazarArchivo(const char *temporal_ptr, const char *destino_ptr) {
#ifdef _WIN32
    return MoveFileExA(temporal_ptr, destino_ptr,
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(temporal_ptr, destino_ptr) == 0;
#endif
}

static int escribirProgreso(FILE *archivo_ptr, const ProgresoPartida_t *progreso_ptr) {
    const int valores[] = {
        progreso_ptr->personajes.vidaMaxima,
        progreso_ptr->personajes.ataque,
        progreso_ptr->personajes.defensa,
        progreso_ptr->enemigos.vidaMaxima,
        progreso_ptr->enemigos.ataque,
        progreso_ptr->enemigos.defensa
    };
    return fwrite(valores, sizeof(valores[0]), sizeof(valores) / sizeof(valores[0]), archivo_ptr)
           == sizeof(valores) / sizeof(valores[0]);
}

static int leerProgreso(FILE *archivo_ptr, ProgresoPartida_t *progreso_ptr) {
    int valores[6];
    if (fread(valores, sizeof(valores[0]), 6, archivo_ptr) != 6) {
        return 0;
    }
    progreso_ptr->personajes.vidaMaxima = valores[0];
    progreso_ptr->personajes.ataque = valores[1];
    progreso_ptr->personajes.defensa = valores[2];
    progreso_ptr->enemigos.vidaMaxima = valores[3];
    progreso_ptr->enemigos.ataque = valores[4];
    progreso_ptr->enemigos.defensa = valores[5];
    return 1;
}

int guardarPartida(const char *nombreArchivo_ptr, const Personaje_t equipo_ptr[], int cantPersonajes,
                   const int desbloqueados_ptr[], const Inventario_t *inventario_ptr,
                   const ProgresoPartida_t *progreso_ptr) {
    if (nombreArchivo_ptr == NULL || equipo_ptr == NULL || desbloqueados_ptr == NULL ||
        inventario_ptr == NULL || progreso_ptr == NULL ||
        cantPersonajes <= 0 || cantPersonajes > MAX_PERSONAJES_INVENTARIO ||
        inventario_ptr->cantidad < 0 || inventario_ptr->cantidad > inventario_ptr->capacidad ||
        inventario_ptr->capacidad > MAX_SLOTS_INVENTARIO) {
        printf("No se pudo guardar la partida: datos invalidos.\n");
        return 0;
    }

    int cantDesbloqueados = 0;
    for (int i = 0; i < cantPersonajes; i++) {
        if (desbloqueados_ptr[i] != 0 && desbloqueados_ptr[i] != 1) {
            printf("No se pudo guardar la partida: estado de desbloqueo invalido.\n");
            return 0;
        }
        cantDesbloqueados += desbloqueados_ptr[i];
    }
    if (cantDesbloqueados == 0 ||
        inventario_ptr->capacidad != cantDesbloqueados * SLOTS_POR_PERSONAJE) {
        printf("No se pudo guardar la partida: capacidad de inventario incorrecta.\n");
        return 0;
    }
    for (int i = 0; i < inventario_ptr->cantidad; i++) {
        int tipo = (int) inventario_ptr->items[i].tipo;
        if ((tipo != ITEM_VIDA && tipo != ITEM_VENENO) || inventario_ptr->items[i].valor <= 0) {
            printf("No se pudo guardar la partida: objeto de inventario invalido.\n");
            return 0;
        }
    }

    char archivoTemporal[256];
    int largoRuta = snprintf(archivoTemporal, sizeof(archivoTemporal), "%s.tmp", nombreArchivo_ptr);
    if (largoRuta < 0 || (size_t) largoRuta >= sizeof(archivoTemporal)) {
        printf("No se pudo guardar la partida: la ruta es demasiado larga.\n");
        return 0;
    }

    FILE *archivo_ptr = fopen(archivoTemporal, "wb");
    if (archivo_ptr == NULL) {
        printf("No se pudo guardar la partida.\n");
        return 0;
    }

    uint32_t cantidadPersonajes = (uint32_t) cantPersonajes;
    int ok = fwrite(FIRMA_PARTIDA, sizeof(FIRMA_PARTIDA), 1, archivo_ptr) == 1 &&
             fwrite(&VERSION_PARTIDA, sizeof(VERSION_PARTIDA), 1, archivo_ptr) == 1 &&
             fwrite(&cantidadPersonajes, sizeof(cantidadPersonajes), 1, archivo_ptr) == 1 &&
             fwrite(equipo_ptr, sizeof(equipo_ptr[0]), (size_t) cantPersonajes, archivo_ptr) ==
                 (size_t) cantPersonajes &&
             fwrite(desbloqueados_ptr, sizeof(desbloqueados_ptr[0]), (size_t) cantPersonajes, archivo_ptr) ==
                 (size_t) cantPersonajes &&
             escribirProgreso(archivo_ptr, progreso_ptr) &&
             fwrite(&inventario_ptr->cantidad, sizeof(inventario_ptr->cantidad), 1, archivo_ptr) == 1;

    for (int i = 0; ok && i < inventario_ptr->cantidad; i++) {
        int tipo = (int) inventario_ptr->items[i].tipo;
        int valor = inventario_ptr->items[i].valor;
        ok = fwrite(&tipo, sizeof(tipo), 1, archivo_ptr) == 1 &&
             fwrite(&valor, sizeof(valor), 1, archivo_ptr) == 1;
    }

    if (fclose(archivo_ptr) != 0) {
        ok = 0;
    }
    if (!ok) {
        remove(archivoTemporal);
        printf("No se pudo completar el guardado de la partida.\n");
        return 0;
    }
    if (!reemplazarArchivo(archivoTemporal, nombreArchivo_ptr)) {
#ifdef _WIN32
        printf("Error del sistema al reemplazar el archivo: %lu.\n",
               (unsigned long) GetLastError());
#endif
        remove(archivoTemporal);
        printf("No se pudo reemplazar el archivo de partida anterior.\n");
        return 0;
    }
    registrarLog("Partida guardada");
    return 1;
}

int cargarPartida(const char *nombreArchivo_ptr, Personaje_t equipo_ptr[], int cantPersonajes,
                  int desbloqueados_ptr[], Inventario_t *inventario_ptr, ProgresoPartida_t *progreso_ptr) {
    if (nombreArchivo_ptr == NULL || equipo_ptr == NULL || desbloqueados_ptr == NULL ||
        inventario_ptr == NULL || progreso_ptr == NULL ||
        cantPersonajes <= 0 || cantPersonajes > MAX_PERSONAJES_INVENTARIO) {
        return 0;
    }

    FILE *archivo_ptr = fopen(nombreArchivo_ptr, "rb");
    if (archivo_ptr == NULL) {
        return 0;
    }

    unsigned char firma[sizeof(FIRMA_PARTIDA)];
    uint32_t version;
    uint32_t cantidadPersonajes;
    Personaje_t equipoTemporal[MAX_PERSONAJES_INVENTARIO];
    int desbloqueadosTemporales[MAX_PERSONAJES_INVENTARIO];
    ProgresoPartida_t progresoTemporal = { { 0, 0, 0 }, { 0, 0, 0 } };
    int cantidadItems = -1;
    int ok = fread(firma, sizeof(firma), 1, archivo_ptr) == 1 &&
             fread(&version, sizeof(version), 1, archivo_ptr) == 1 &&
             fread(&cantidadPersonajes, sizeof(cantidadPersonajes), 1, archivo_ptr) == 1 &&
             memcmp(firma, FIRMA_PARTIDA, sizeof(firma)) == 0 &&
             version == VERSION_PARTIDA &&
             cantidadPersonajes == (uint32_t) cantPersonajes;

    if (ok) {
        ok = fread(equipoTemporal, sizeof(equipoTemporal[0]), (size_t) cantPersonajes, archivo_ptr)
                 == (size_t) cantPersonajes &&
             fread(desbloqueadosTemporales, sizeof(desbloqueadosTemporales[0]),
                   (size_t) cantPersonajes, archivo_ptr) == (size_t) cantPersonajes &&
             leerProgreso(archivo_ptr, &progresoTemporal) &&
             fread(&cantidadItems, sizeof(cantidadItems), 1, archivo_ptr) == 1;
    }

    int cantDesbloqueados = 0;
    for (int i = 0; ok && i < cantPersonajes; i++) {
        if (desbloqueadosTemporales[i] != 0 && desbloqueadosTemporales[i] != 1) {
            ok = 0;
        } else {
            cantDesbloqueados += desbloqueadosTemporales[i];
        }
    }
    if (cantDesbloqueados == 0 || cantidadItems < 0 ||
        cantidadItems > cantDesbloqueados * SLOTS_POR_PERSONAJE ||
        cantidadItems > MAX_SLOTS_INVENTARIO) {
        ok = 0;
    }

    Inventario_t inventarioTemporal = crearInventario(cantDesbloqueados);
    for (int i = 0; ok && i < cantidadItems; i++) {
        int tipo;
        int valor;
        if (fread(&tipo, sizeof(tipo), 1, archivo_ptr) != 1 ||
            fread(&valor, sizeof(valor), 1, archivo_ptr) != 1 ||
            !agregarItem(&inventarioTemporal, (Item_t) { (TipoItem) tipo, valor })) {
            ok = 0;
        }
    }

    if (fclose(archivo_ptr) != 0) {
        ok = 0;
    }
    if (!ok) {
        printf("El archivo de partida esta incompleto o no corresponde a esta version.\n");
        return 0;
    }

    memcpy(equipo_ptr, equipoTemporal, sizeof(equipoTemporal[0]) * (size_t) cantPersonajes);
    memcpy(desbloqueados_ptr, desbloqueadosTemporales,
           sizeof(desbloqueadosTemporales[0]) * (size_t) cantPersonajes);
    *inventario_ptr = inventarioTemporal;
    *progreso_ptr = progresoTemporal;
    registrarLog("Partida cargada");
    return 1;
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
void registrarLog(const char *mensaje_ptr) {
    FILE *archivo_ptr = fopen(ARCHIVO_LOG, "a");
    if (archivo_ptr == NULL) {
        return;
    }
    time_t ahora = time(NULL);
    char fechaTexto[26];
    snprintf(fechaTexto, sizeof(fechaTexto), "%s", ctime(&ahora));
    fechaTexto[24] = '\0'; // quitar el salto de linea que agrega ctime
    fprintf(archivo_ptr, "[%s] %s\n", fechaTexto, mensaje_ptr);
    fclose(archivo_ptr);
}
