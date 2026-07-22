#ifndef PERSONAJES_H
#define PERSONAJES_H

// Estados alterados que puede sufrir un personaje o enemigo en combate.
typedef enum {
    ESTADO_NINGUNO,
    ESTADO_VENENO,
    ESTADO_ATURDIDO,
    ESTADO_DEBILITADO
} TipoEstado;

typedef struct {
    TipoEstado tipo;
    int duracionTurnos;
    int magnitud;
} Estado;

typedef struct {
    char nombre[30];
    int vida;
    int vidaMax;
    int defensa;
    int ataque;
    Estado estadoActivo;
} Personaje;

// Crea un personaje jugable con sus stats iniciales.
Personaje crearPersonaje(const char *nombre, int vidaMax, int defensa, int ataque);

// Muestra por consola los datos actuales del personaje.
void mostrarPersonaje(const Personaje *personaje);

// Devuelve 1 si el personaje sigue con vida, 0 si fue derrotado.
int estaVivoPersonaje(const Personaje *personaje);

// Aplica un nuevo estado alterado al personaje (veneno, aturdido, etc.).
void aplicarEstadoPersonaje(Personaje *personaje, Estado estado);

// Procesa el estado activo al final de un turno (resta duracion, aplica efecto).
void actualizarEstadoPersonaje(Personaje *personaje);

#endif
