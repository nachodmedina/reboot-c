#ifndef COMBATE_H
#define COMBATE_H

#include "personajes.h"
#include "enemigos.h"

// Corre un combate por turnos entre el equipo de personajes y los enemigos de la zona.
void iniciarCombate(Personaje equipo[], int cantPersonajes, Enemigo enemigos[], int cantEnemigos);

// Calcula el daño final teniendo en cuenta ataque y defensa.
int calcularDano(int ataque, int defensa);

// Turno de un personaje: elegir accion y aplicarla sobre un enemigo.
void turnoPersonaje(Personaje *atacante, Enemigo *objetivo);

// Turno de un enemigo: ataca a un personaje del equipo.
void turnoEnemigo(Enemigo *atacante, Personaje *objetivo);

#endif
