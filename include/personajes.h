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
} Estado_t;

typedef enum {
    HABILIDAD_CRITICO_GRUPAL,   // Isaac
    HABILIDAD_DEFENSA_GRUPAL,   // Dante
    HABILIDAD_CURACION_GRUPAL   // Frieren
} TipoHabilidadEspecial;

typedef struct {
    char nombre[30];
    char arma[30];
    TipoHabilidadEspecial habilidadEspecial;
    int vida;
    int vidaMax;
    int defensa;
    int ataque;
    Estado_t estadoActivo;
} Personaje_t;

// Crea un personaje jugable con sus stats iniciales.
Personaje_t crearPersonaje(const char *nombre_ptr, const char *arma_ptr, TipoHabilidadEspecial habilidad, int vidaMax, int defensa, int ataque);
// Muestra por consola los datos actuales del personaje.
void mostrarPersonaje(const Personaje_t *personaje_ptr);

// Devuelve 1 si el personaje sigue con vida, 0 si fue derrotado.
int estaVivoPersonaje(const Personaje_t *personaje_ptr);

// Aplica un nuevo estado alterado al personaje (veneno, aturdido, etc.).
void aplicarEstadoPersonaje(Personaje_t *personaje_ptr, Estado_t estado);

// Procesa el estado activo al final de un turno (resta duracion, aplica efecto).
void actualizarEstadoPersonaje(Personaje_t *personaje_ptr);

#endif
