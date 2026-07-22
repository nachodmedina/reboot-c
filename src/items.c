#include <stdio.h>
#include <stdlib.h>
#include "../include/items.h"

Inventario *crearInventario(int capacidadInicial) {
    Inventario *inventario = malloc(sizeof(Inventario));
    inventario->items = malloc(sizeof(Item) * (size_t) capacidadInicial);
    inventario->cantidad = 0;
    inventario->capacidad = capacidadInicial;
    return inventario;
}

void agregarItem(Inventario *inventario, Item item) {
    if (inventario->cantidad == inventario->capacidad) {
        inventario->capacidad *= 2;
        inventario->items = realloc(inventario->items, sizeof(Item) * (size_t) inventario->capacidad);
    }
    inventario->items[inventario->cantidad] = item;
    inventario->cantidad++;
}

void usarItem(Inventario *inventario, int indice, Personaje *objetivo) {
    // TODO: aplicar el efecto real segun el TipoItem (vida, ataque, defensa).
    (void) objetivo;
    if (indice < 0 || indice >= inventario->cantidad) {
        return;
    }
    for (int i = indice; i < inventario->cantidad - 1; i++) {
        inventario->items[i] = inventario->items[i + 1];
    }
    inventario->cantidad--;
}

void liberarInventario(Inventario *inventario) {
    free(inventario->items);
    free(inventario);
}
