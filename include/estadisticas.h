#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

#define ARCHIVO_ESTADISTICAS "estadisticas.log"
#define MAX_REGISTROS_RONDA 100000   // tope para rechazar un archivo danado al cargar

// Lo que paso en una ronda de combate.
typedef struct {
    int zona;
    int fase;                 // 1..cantFases, como se la muestra al jugador
    int ronda;
    int danoCausado;
    int danoRecibido;
    int enemigosDerrotados;
} RegistroRonda_t;

// Arreglo dinamico: la cantidad de rondas depende de como se juegue la partida.
typedef struct {
    RegistroRonda_t *registros_ptr;
    int cantidad;              // registros en uso
    int capacidad;             // registros reservados
} Estadisticas_t;

// Reserva la capacidad inicial. Si no hay memoria queda vacia y sin capacidad:
// agregarRegistroRonda vuelve a intentar reservar.
Estadisticas_t crearEstadisticas(void);

// Agrega un registro al final, duplicando la capacidad si hace falta.
// Devuelve 0 si no hay memoria; los registros anteriores se conservan.
int agregarRegistroRonda(Estadisticas_t *estadisticas_ptr, RegistroRonda_t registro);

// Muestra los totales de la partida por consola.
void mostrarEstadisticas(const Estadisticas_t *estadisticas_ptr);

// Agrega el resumen de la partida a ARCHIVO_ESTADISTICAS. Devuelve 0 si no pudo.
int guardarEstadisticas(const Estadisticas_t *estadisticas_ptr);

// Libera el arreglo y deja las estadisticas vacias.
void liberarEstadisticas(Estadisticas_t *estadisticas_ptr);

#endif
