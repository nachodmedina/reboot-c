#include <stdio.h>
#include "../include/items.h"

static int tipoItemValido(TipoItem tipo) {
    return tipo == ITEM_VIDA || tipo == ITEM_VENENO;
}

static const char *nombreTipoItem(TipoItem tipo) {
    switch (tipo) {
        case ITEM_VIDA:
            return "Pocion de vida";
        case ITEM_VENENO:
            return "Pocion de veneno";
    }
    return "Objeto desconocido";
}

Inventario_t crearInventario(int cantPersonajesDesbloqueados) {
    Inventario_t inventario = { { { ITEM_VIDA, 0 } }, 0, 0 };
    actualizarCapacidadInventario(&inventario, cantPersonajesDesbloqueados);
    return inventario;
}

int actualizarCapacidadInventario(Inventario_t *inventario_ptr, int cantPersonajesDesbloqueados) {
    if (inventario_ptr == NULL || cantPersonajesDesbloqueados < 0 ||
        cantPersonajesDesbloqueados > MAX_PERSONAJES_INVENTARIO) {
        return 0;
    }

    int capacidadNueva = cantPersonajesDesbloqueados * SLOTS_POR_PERSONAJE;
    if (inventario_ptr->cantidad > capacidadNueva) {
        return 0;
    }

    inventario_ptr->capacidad = capacidadNueva;
    return 1;
}

int agregarItem(Inventario_t *inventario_ptr, Item_t item) {
    if (inventario_ptr == NULL || !tipoItemValido(item.tipo) || item.valor <= 0 ||
        inventario_ptr->cantidad < 0 || inventario_ptr->cantidad >= inventario_ptr->capacidad ||
        inventario_ptr->capacidad > MAX_SLOTS_INVENTARIO) {
        return 0;
    }

    inventario_ptr->items[inventario_ptr->cantidad] = item;
    inventario_ptr->cantidad++;
    return 1;
}

static void quitarItem(Inventario_t *inventario_ptr, int indice) {
    for (int i = indice; i < inventario_ptr->cantidad - 1; i++) {
        inventario_ptr->items[i] = inventario_ptr->items[i + 1];
    }
    inventario_ptr->cantidad--;
}

int usarItem(Inventario_t *inventario_ptr, int indice, Personaje_t *usuario_ptr, Enemigo_t *objetivoEnemigo_ptr) {
    if (inventario_ptr == NULL || indice < 0 || indice >= inventario_ptr->cantidad) {
        return 0;
    }

    Item_t item = inventario_ptr->items[indice];
    if (item.tipo == ITEM_VIDA) {
        if (usuario_ptr == NULL || !estaVivoPersonaje(usuario_ptr) || usuario_ptr->vida >= usuario_ptr->vidaMax) {
            return 0;
        }

        usuario_ptr->vida += item.valor;
        if (usuario_ptr->vida > usuario_ptr->vidaMax) {
            usuario_ptr->vida = usuario_ptr->vidaMax;
        }
        printf("%s recupera vida: %d/%d\n", usuario_ptr->nombre, usuario_ptr->vida, usuario_ptr->vidaMax);
    } else if (item.tipo == ITEM_VENENO) {
        if (objetivoEnemigo_ptr == NULL || !aplicarVenenoEnemigo(objetivoEnemigo_ptr, item.valor, 4)) {
            return 0;
        }
        printf("%s queda envenenado: recibira %d de dano durante 4 turnos.\n",
               objetivoEnemigo_ptr->nombre, item.valor);
    } else {
        return 0;
    }

    quitarItem(inventario_ptr, indice);
    return 1;
}

void mostrarInventario(const Inventario_t *inventario_ptr) {
    if (inventario_ptr == NULL) {
        printf("Inventario no disponible.\n");
        return;
    }

    printf("Inventario compartido: %d/%d espacios ocupados.\n",
           inventario_ptr->cantidad, inventario_ptr->capacidad);
    if (inventario_ptr->cantidad == 0) {
        printf("  (vacio)\n");
        return;
    }

    for (int i = 0; i < inventario_ptr->cantidad; i++) {
        printf("  %d. %s (+%d)\n", i + 1,
               nombreTipoItem(inventario_ptr->items[i].tipo), inventario_ptr->items[i].valor);
    }
}
