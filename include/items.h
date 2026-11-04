#ifndef ITEMS_H
#define ITEMS_H

#include <stddef.h>
#include "enemigos.h"
#include "personajes.h"

#define SLOTS_POR_PERSONAJE 3
#define MAX_PERSONAJES_INVENTARIO 3
#define MAX_SLOTS_INVENTARIO (SLOTS_POR_PERSONAJE * MAX_PERSONAJES_INVENTARIO)
#define DURACION_VENENO_ITEM 4   // turnos que dura el veneno de una pocion

typedef enum {
    ITEM_VIDA,
    ITEM_VENENO
} TipoItem;

typedef struct {
    TipoItem tipo;
    int valor;
} Item_t;

typedef struct {
    Item_t items[MAX_SLOTS_INVENTARIO];
    int cantidad;
    int capacidad;
} Inventario_t;

// Crea una bolsa compartida con cinco espacios por personaje desbloqueado.
Inventario_t crearInventario(int cantPersonajesDesbloqueados);

// Actualiza la capacidad sin perder objetos; devuelve 0 si la nueva capacidad no alcanza.
int actualizarCapacidadInventario(Inventario_t *inventario_ptr, int cantPersonajesDesbloqueados);

// Agrega un consumible si queda espacio; devuelve 1 si lo agrego.
int agregarItem(Inventario_t *inventario_ptr, Item_t item);

// Usa y consume el objeto solo si su efecto pudo aplicarse.
int usarItem(Inventario_t *inventario_ptr, int indice, Personaje_t *usuario_ptr, Enemigo_t *objetivoEnemigo_ptr);

// Nombre para mostrar de un tipo de objeto ("Pocion de vida").
const char *nombreItem(TipoItem tipo);

// Escribe en texto_ptr el nombre del objeto con su efecto ("Pocion de vida (+100 de vida)").
void describirItem(const Item_t *item_ptr, char texto_ptr[], size_t tamanio);

void mostrarInventario(const Inventario_t *inventario_ptr);

#endif
