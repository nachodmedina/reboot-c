#include <stdio.h>
#include "../include/combate.h"

int calcularDano(int ataque, int defensa) {
    // TODO: ajustar la formula de dano (criticos, resistencias, etc.).
    int dano = ataque - defensa;
    return dano > 0 ? dano : 1;
}

void turnoPersonaje(Personaje *atacante, Enemigo *objetivo) {
    // TODO: elegir accion (atacar, usar item, defender) desde un menu de combate.
    int dano = calcularDano(atacante->ataque, objetivo->defensa);
    objetivo->vida -= dano;
}

void turnoEnemigo(Enemigo *atacante, Personaje *objetivo) {
    // TODO: agregar variedad de ataques por tipo de enemigo.
    int dano = calcularDano(atacante->ataque, objetivo->defensa);
    objetivo->vida -= dano;
}

void iniciarCombate(Personaje equipo[], int cantPersonajes, Enemigo enemigos[], int cantEnemigos) {
    // TODO: implementar el bucle de turnos completo (orden, condiciones de victoria/derrota). 

    //nacho puto
    (void) equipo;
    (void) cantPersonajes;
    (void) enemigos;
    (void) cantEnemigos;
    printf("Combate: modulo en construccion.\n");
}
