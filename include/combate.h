#ifndef COMBATE_H
#define COMBATE_H

#include "personajes.h"
#include "enemigos.h"
#include "items.h"

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

// Crea un efecto grupal inactivo para empezar un combate.
EfectoGrupal_t crearEfectoGrupal(void);

// ataque - defensa, con minimo 0 (una defensa alta bloquea el golpe).
int calcularDano(int ataque, int defensa);

// Dano del ataque en area contra un enemigo: mitad del ataque + 15, menos su defensa.
int calcularDanoArea(int ataque, int defensa);

// Ataque del personaje teniendo en cuenta si esta debilitado.
int ataqueEfectivo(Personaje_t personaje);

// Ataque 1 a 1 sobre un enemigo (x1.2 si el critico grupal esta activo).
void ataqueIndividual(Personaje_t atacante, Enemigo_t *objetivo_ptr, EfectoGrupal_t efecto);

// Ataque en area: golpea a todos los enemigos vivos.
void ataqueArea(Personaje_t atacante, Enemigo_t enemigos_ptr[], int cantEnemigos, EfectoGrupal_t efecto);

// Cura a todos los personajes vivos, sin pasar su vida maxima.
void curarEquipo(Personaje_t equipo_ptr[], int cantPersonajes, int cantidad);

// Activa la habilidad especial del usuario por 2 turnos si no hay otro efecto activo.
// Devuelve 1 si se activo o 0 si ya hay un efecto activo.
int activarHabilidad(Personaje_t usuario, Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t *efecto_ptr);

// Efectos de comienzo de ronda (segunda curacion de Frieren).
void iniciarRonda(Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto);

// Turno de un enemigo: elige un personaje vivo al azar y lo ataca.
void turnoEnemigo(Enemigo_t atacante, Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto);

// Descuenta un turno del efecto grupal y lo limpia cuando llega a 0.
void cerrarRonda(EfectoGrupal_t *efecto_ptr);

int quedanPersonajesVivos(Personaje_t equipo_ptr[], int cantPersonajes);
int quedanEnemigosVivos(Enemigo_t enemigos_ptr[], int cantEnemigos);

// Combate interactivo por turnos. inventario puede ser NULL (sin items).
// Devuelve 1 si gana el equipo, 0 si pierde.
int iniciarCombate(Personaje_t equipo_ptr[], int cantPersonajes, Enemigo_t enemigos_ptr[], int cantEnemigos, Inventario_t *inventario_ptr);

#endif
