#ifndef ITEMS_H
#define ITEMS_H

#include "personajes.h"

typedef enum {
    ITEM_VIDA,
    ITEM_ATAQUE,
    ITEM_DEFENSA
} TipoItem;

typedef struct {
    TipoItem tipo;
    int valor;
    int esGrupal;
} Item;

// Inventario con memoria dinamica: crece con realloc a medida que hace falta.
typedef struct {
    Item *items;
    int cantidad;
    int capacidad;
} Inventario;

// Reserva un inventario vacio con una capacidad inicial.
Inventario *crearInventario(int capacidadInicial);

// Agrega un item al inventario, agrandando el arreglo con realloc si hace falta.
void agregarItem(Inventario *inventario, Item item);

// Aplica el efecto del item sobre un personaje y lo saca del inventario.
void usarItem(Inventario *inventario, int indice, Personaje *objetivo);

// Libera toda la memoria pedida para el inventario.
void liberarInventario(Inventario *inventario);

#endif
