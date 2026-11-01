#ifndef HISTORIA_H
#define HISTORIA_H

#include "personajes.h"
#include "items.h"
#include "zonas.h"

// Introduccion general del mundo y el ataque a La Raiz. Es igual para
// todos los protagonistas, por eso se muestra antes de elegir personaje.
void mostrarIntroduccionMundo(void);

// La separacion, el hallazgo de la baliza y la transformacion del arma
// del protagonista elegido.
void mostrarNarrativaInicial(const Personaje_t *personaje_ptr);

// La baliza empieza a guiar el camino: se muestra despues de
// mostrarNarrativaInicial, antes de entrar a la primera zona.
void mostrarSalidaDeRaiz(const Personaje_t *personaje_ptr);

// Cartel de entrada a una zona (nombre + descripcion).
void mostrarEntradaZona(const Zona_t *zona_ptr);

// Escena corta justo antes de un combate: el personaje avanza por
// "recorrido" y se topa con lo que describe "amenaza".
void mostrarEncuentroEnemigos(const Personaje_t *personaje_ptr, const char *recorrido_ptr,
                              const char *amenaza_ptr);

// Escena despues de ganar un combate que no es el ultimo de la zona.
void mostrarPostBatalla(const Personaje_t *personaje_ptr);

// Hallazgo de una mejora tras vencer al jefe de la zona.
void mostrarHallazgoMejora(void);

// Sefirot detecta la mejora y anuncia el contrabonus enemigo.
void mostrarReaccionSefirot(const char *atributoEnemigo_ptr, int aumentoAcumulado);

// Liberacion del aliado que se une al cerrar la Zona 1 o la Zona 2. El texto
// depende del protagonista; despues se muestra el arte del aliado y su arma.
void mostrarLiberacionAliado(const Personaje_t *protagonista_ptr, const Personaje_t *aliado_ptr,
                             int numeroZona);

// Dialogo junto a la fogata antes del jefe de las Zonas 2 a 4 (en la Zona 1 no hay).
void mostrarDescansoZona(const Personaje_t *protagonista_ptr, int numeroZona);

// La caida de Sefirot, el epilogo y la pantalla final.
void mostrarCaidaSefirot(const Personaje_t *protagonista_ptr);
void mostrarEpilogo(const Personaje_t *protagonista_ptr);
void mostrarFinDelJuego(void);

// Evento aleatorio entre fases consecutivas: puede dar objetos, envenenar,
// curar o mejorar al personaje.
void eventoEntreFases(Personaje_t *personaje_ptr, Inventario_t *inventario_ptr);

#endif
