#include <stdio.h>
#include <string.h>
#include "../include/historia.h"

Zona crearZona(const char *nombre, const char *descripcion, int nivel) {
    Zona zona;
    strncpy(zona.nombre, nombre, sizeof(zona.nombre) - 1);
    zona.nombre[sizeof(zona.nombre) - 1] = '\0';
    strncpy(zona.descripcion, descripcion, sizeof(zona.descripcion) - 1);
    zona.descripcion[sizeof(zona.descripcion) - 1] = '\0';
    zona.nivel = nivel;
    return zona;
}

void mostrarNarrativaZona(const Zona *zona) {
    // TODO: sumar mas texto narrativo y transiciones entre zonas.
    printf("== %s (nivel %d) ==\n%s\n", zona->nombre, zona->nivel, zona->descripcion);
}
