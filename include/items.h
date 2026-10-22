#ifndef ITEMS_H
#define ITEMS_H

#include "enemigos.h"
#include "personajes.h"

#define SLOTS_POR_PERSONAJE 5
#define MAX_PERSONAJES_INVENTARIO 3
#define MAX_SLOTS_INVENTARIO (SLOTS_POR_PERSONAJE * MAX_PERSONAJES_INVENTARIO)

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

void mostrarInventario(const Inventario_t *inventario_ptr);

#endif
