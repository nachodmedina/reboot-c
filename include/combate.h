#ifndef COMBATE_H
#define COMBATE_H

#include "personajes.h"
#include "enemigos.h"
#include "items.h"

// Habilidad especial grupal activa durante un combate. Es un dato del
// combate, no del personaje: por eso no vive en la struct Personaje.
typedef struct {
    TipoHabilidadEspecial tipo;   // HABILIDAD_NINGUNA si no hay nada activo
    int turnosRestantes;
    int yaUsada;                  // una sola habilidad grupal por combate
} EfectoGrupal;

// Crea un EfectoGrupal vacio para empezar un combate.
EfectoGrupal crearEfectoGrupal(void);

// ataque - defensa, con minimo 0 (una defensa alta bloquea el golpe).
int calcularDano(int ataque, int defensa);

// Dano del ataque en area contra un enemigo: mitad del ataque + 15, menos su defensa.
int calcularDanoArea(int ataque, int defensa);

// Ataque del personaje teniendo en cuenta si esta debilitado.
int ataqueEfectivo(Personaje personaje);

// Ataque 1 a 1 sobre un enemigo (x1.2 si el critico grupal esta activo).
void ataqueIndividual(Personaje atacante, Enemigo *objetivo, EfectoGrupal efecto);

// Ataque en area: golpea a todos los enemigos vivos.
void ataqueArea(Personaje atacante, Enemigo enemigos[], int cantEnemigos, EfectoGrupal efecto);

// Cura a todos los personajes vivos, sin pasar su vida maxima.
void curarEquipo(Personaje equipo[], int cantPersonajes, int cantidad);

// Activa la habilidad especial del usuario por 2 turnos (una vez por combate).
// Devuelve 1 si se activo, 0 si ya se habia usado o el personaje no tiene habilidad.
int activarHabilidad(Personaje usuario, Personaje equipo[], int cantPersonajes, EfectoGrupal *efecto);

// Efectos de comienzo de ronda (segunda curacion de Frieren).
void iniciarRonda(Personaje equipo[], int cantPersonajes, EfectoGrupal efecto);

// Turno de un enemigo: elige un personaje vivo al azar y lo ataca.
void turnoEnemigo(Enemigo atacante, Personaje equipo[], int cantPersonajes, EfectoGrupal efecto);

// Descuenta un turno del efecto grupal y lo limpia cuando llega a 0.
void cerrarRonda(EfectoGrupal *efecto);

int quedanPersonajesVivos(Personaje equipo[], int cantPersonajes);
int quedanEnemigosVivos(Enemigo enemigos[], int cantEnemigos);

// Combate interactivo por turnos. inventario puede ser NULL (sin items).
// Devuelve 1 si gana el equipo, 0 si pierde.
int iniciarCombate(Personaje equipo[], int cantPersonajes, Enemigo enemigos[], int cantEnemigos, Inventario *inventario);

#endif
