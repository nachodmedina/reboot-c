#ifndef HISTORIA_H
#define HISTORIA_H

// Una zona agrupa la narrativa y los enemigos asociados a una etapa del juego.
typedef struct {
    char nombre[30];
    char descripcion[200];
    int nivel;
} Zona;

// Crea una zona con su nombre, descripcion y nivel de dificultad.
Zona crearZona(const char *nombre, const char *descripcion, int nivel);

// Muestra la narrativa de la zona antes de avanzar.
void mostrarNarrativaZona(const Zona *zona);

#endif
