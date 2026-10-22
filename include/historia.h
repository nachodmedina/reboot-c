#ifndef HISTORIA_H
#define HISTORIA_H

#include "personajes.h"
#include "enemigos.h"
#include "items.h"

#define MAX_ENEMIGOS_FASE 3
#define MAX_FASES_ZONA 4

// Una fase es un combate dentro de una zona.
typedef struct {
    char amenaza[150];                    // texto para mostrarEncuentroEnemigos
    Enemigo_t enemigos[MAX_ENEMIGOS_FASE];  // enemigos en su estado inicial (vida llena)
    int cantEnemigos;                     // cuantos lugares de "enemigos" se usan
    int fogataAntes;                      // 1 si los luchadores descansan antes de esta fase
} Fase_t;

// Una zona agrupa la narrativa y los enemigos asociados a una etapa del juego.
typedef struct {
    char nombre[30];
    char descripcion[200];
    Fase_t fases[MAX_FASES_ZONA];  // en el orden en que se juegan
    int cantFases;               // cuantos lugares de "fases" se usan
} Zona_t;

// Crea una zona con su nombre y descripcion (sin fases).
Zona_t crearZona(const char *nombre_ptr, const char *descripcion_ptr);

// Agrega una fase al final de la zona. Los enemigos se copian.
// Devuelve 1 si la agrego, 0 si no hay lugar.
int agregarFase(Zona_t *zona_ptr, const char *amenaza_ptr, const Enemigo_t enemigos_ptr[], int cantEnemigos, int fogataAntes);

// Zona 1: Campo Ceniza, con sus 4 fases (la ultima es el jefe).
Zona_t crearZonaCampoCeniza(void);

// Muestra la introduccion general del mundo (Bloques 1 y 2 del guion):
// el contexto del año 2526 y el ataque a Raiz. No depende de que personaje
// elija el jugador, por eso no recibe ningun parametro. Se muestra siempre
// antes de que el jugador elija con quien empezar.
void mostrarIntroduccionMundo(void);
 
// Muestra la narrativa inicial del juego ya centrada en el protagonista
// elegido (Bloques 3, 4 y 5 del guion): la separacion, el hallazgo del
// arma comun junto a la baliza, y la transformacion del arma segun el
// personaje.
void mostrarNarrativaInicial(const Personaje_t *personaje_ptr);



// La baliza empieza a guiar el camino y arranca el viaje: se muestra
// despues de mostrarNarrativaInicial, antes de entrar a la primera zona.
void mostrarSalidaDeRaiz(const Personaje_t *personaje_ptr);
 
// Muestra el cartel de entrada a una zona nueva (nombre + descripcion),
// usando los datos de la struct Zona_t.
void mostrarEntradaZona(const Zona_t *zona_ptr);


// Escena corta de transicion justo antes de un combate: el personaje
// escucha algo raro y se topa con lo que lo va a atacar. "amenaza" describe
// que aparece (por ejemplo "dos perros mecanicos corren hacia vos"), para
// poder reusar esta misma funcion en cada encuentro de cada zona sin
// reescribir el texto fijo cada vez.
void mostrarEncuentroEnemigos(const Personaje_t *personaje_ptr, const char *amenaza_ptr);


//escena luego de la batalla

void mostrarPostBatalla(const Personaje_t *personaje_ptr);

// Muestra como Sephirot detecta la mejora y anuncia el contrabonus enemigo.
void mostrarReaccionSephirot(const char *atributoEnemigo_ptr, int aumentoAcumulado);

// Muestra la liberacion y union de un aliado, usando sus datos del personaje.
void mostrarLiberacionAliado(const Personaje_t *protagonista_ptr, const Personaje_t *aliado_ptr);

// Evento narrativo aleatorio entre fases consecutivas.
void eventoEntreFases(Personaje_t *personaje_ptr, Inventario_t *inventario_ptr);

#endif
