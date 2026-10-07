#ifndef HISTORIA_H
#define HISTORIA_H

#include "personajes.h"


// Una zona agrupa la narrativa y los enemigos asociados a una etapa del juego.
typedef struct {
    char nombre[30];
    char descripcion[200];
    int nivel;
} Zona;

// Crea una zona con su nombre, descripcion y nivel de dificultad.
Zona crearZona(const char *nombre, const char *descripcion, int nivel);

// Muestra la narrativa de la zona antes de avanzar.
void mostrarNarrativaZona(const Zona *zona);

// Muestra la introduccion general del mundo (Bloques 1 y 2 del guion):
// el contexto del año 2526 y el ataque a Raiz. No depende de que personaje
// elija el jugador, por eso no recibe ningun parametro. Se muestra siempre
// antes de que el jugador elija con quien empezar.
void mostrarIntroduccionMundo(void);
 
// Muestra la narrativa inicial del juego ya centrada en el protagonista
// elegido (Bloques 3, 4 y 5 del guion): la separacion, el hallazgo del
// arma comun junto a la baliza, y la transformacion del arma segun el
// personaje.
void mostrarNarrativaInicial(const Personaje *personaje);



// La baliza empieza a guiar el camino y arranca el viaje: se muestra
// despues de mostrarNarrativaInicial, antes de entrar a la primera zona.
void mostrarSalidaDeRaiz(const Personaje *personaje);
 
// Muestra el cartel de entrada a una zona nueva (nombre + descripcion),
// usando los datos de la struct Zona.
void mostrarEntradaZona(const Zona *zona);


// Escena corta de transicion justo antes de un combate: el personaje
// escucha algo raro y se topa con lo que lo va a atacar. "amenaza" describe
// que aparece (por ejemplo "dos perros mecanicos corren hacia vos"), para
// poder reusar esta misma funcion en cada encuentro de cada zona sin
// reescribir el texto fijo cada vez.
void mostrarEncuentroEnemigos(const Personaje *personaje, const char *amenaza);


#endif
