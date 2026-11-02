#ifndef COMBATE_H
#define COMBATE_H

#include "personajes.h"
#include "enemigos.h"
#include "estadisticas.h"
#include "items.h"

#define MAX_ENEMIGOS_COMBATE 3   // enemigos vivos a la vez en un combate
#define MAX_REFUERZOS_BATALLA 3  // unidades distintas que un jefe puede invocar

typedef enum {
    EFECTO_GRUPAL_INACTIVO,
    EFECTO_GRUPAL_CRITICO,
    EFECTO_GRUPAL_DEFENSA,
    EFECTO_GRUPAL_CURACION
} TipoEfectoGrupal;

// Efecto de la habilidad especial grupal durante un combate.
typedef struct {
    TipoEfectoGrupal tipo;
    int turnosRestantes;
} EfectoGrupal_t;

// Lo que enfrenta el equipo en un combate: los enemigos y, si el jefe puede
// pedir ayuda, los refuerzos que tiene disponibles para invocar.
typedef struct {
    Enemigo_t enemigos[MAX_ENEMIGOS_COMBATE];
    int cantEnemigos;                          // lugares de "enemigos" en uso
    Enemigo_t refuerzos[MAX_REFUERZOS_BATALLA];
    int cantRefuerzos;                         // 0 si nadie invoca
} Batalla_t;

// Crea un efecto grupal inactivo para empezar un combate.
EfectoGrupal_t crearEfectoGrupal(void);

// ataque - defensa, con minimo 0 (una defensa alta bloquea el golpe).
int calcularDano(int ataque, int defensa);

// Dano del ataque en area contra un enemigo: mitad del ataque + 15, menos su defensa.
int calcularDanoArea(int ataque, int defensa);

// Ataque del personaje teniendo en cuenta si esta debilitado.
int ataqueEfectivo(const Personaje_t *personaje_ptr);

// Ataque 1 a 1 sobre un enemigo (x1.2 si el critico grupal esta activo).
void ataqueIndividual(const Personaje_t *atacante_ptr, Enemigo_t *objetivo_ptr, EfectoGrupal_t efecto);

// Ataque en area: golpea a todos los enemigos vivos.
void ataqueArea(const Personaje_t *atacante_ptr, Enemigo_t enemigos_ptr[], int cantEnemigos, EfectoGrupal_t efecto);

// Cura a todos los personajes vivos, sin pasar su vida maxima.
void curarEquipo(Personaje_t equipo_ptr[], int cantPersonajes, int cantidad);

// Activa la habilidad especial del usuario por 2 turnos si no hay otro efecto activo.
// Devuelve 1 si se activo o 0 si ya hay un efecto activo.
int activarHabilidad(const Personaje_t *usuario_ptr, Personaje_t equipo_ptr[], int cantPersonajes,
                     EfectoGrupal_t *efecto_ptr);

// Efectos de comienzo de ronda (segunda curacion de Frieren).
void iniciarRonda(Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto);

// Turno de un enemigo: elige un personaje vivo al azar y lo ataca.
void turnoEnemigo(const Enemigo_t *atacante_ptr, Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto);

// Recupera la vida de regeneracion del enemigo, sin pasar su vida maxima.
void regenerarEnemigo(Enemigo_t *enemigo_ptr);

// Suma a la batalla una copia al azar de sus refuerzos, en el lugar de un
// enemigo caido o en uno libre. Devuelve 0 si no hay refuerzos o lugar.
int invocarRefuerzo(Batalla_t *batalla_ptr);

// Turno de todos los enemigos vivos: regeneracion, acciones de Sefirot y
// ataques. Los invocados durante esta fase actuan recien en la ronda siguiente.
void faseEnemigos(Batalla_t *batalla_ptr, Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto);

// Descuenta un turno del efecto grupal y lo limpia cuando llega a 0.
void cerrarRonda(EfectoGrupal_t *efecto_ptr);

int quedanPersonajesVivos(const Personaje_t equipo_ptr[], int cantPersonajes);
int quedanEnemigosVivos(const Enemigo_t enemigos_ptr[], int cantEnemigos);

// Combate interactivo por turnos. inventario puede ser NULL (sin items).
// Los enemigos de batalla_ptr pierden vida durante la pelea: pasar una copia.
// estadisticas_ptr puede ser NULL (sin registro de rondas).
// Devuelve 1 si gana el equipo, 0 si pierde.
int iniciarCombate(Personaje_t equipo_ptr[], int cantPersonajes, Batalla_t *batalla_ptr, Inventario_t *inventario_ptr,
                   Estadisticas_t *estadisticas_ptr, int zona, int fase);

#endif
