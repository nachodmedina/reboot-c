#include <string.h>
#include "../include/zonas.h"

static Zona_t crearZona(int numero, const char *nombre_ptr, const char *descripcion_ptr,
                        const char *recorrido_ptr) {
    Zona_t zona;
    zona.numero = numero;
    strncpy(zona.nombre, nombre_ptr, sizeof(zona.nombre) - 1);
    zona.nombre[sizeof(zona.nombre) - 1] = '\0';
    strncpy(zona.descripcion, descripcion_ptr, sizeof(zona.descripcion) - 1);
    zona.descripcion[sizeof(zona.descripcion) - 1] = '\0';
    strncpy(zona.recorrido, recorrido_ptr, sizeof(zona.recorrido) - 1);
    zona.recorrido[sizeof(zona.recorrido) - 1] = '\0';
    zona.cantFases = 0;
    return zona;
}

// Agrega una fase al final de la zona. Los enemigos se copian.
// Devuelve 1 si la agrego, 0 si no hay lugar.
static int agregarFase(Zona_t *zona_ptr, const char *amenaza_ptr, const Enemigo_t enemigos_ptr[],
                       int cantEnemigos, int fogataAntes) {
    if (zona_ptr->cantFases >= MAX_FASES_ZONA || cantEnemigos > MAX_ENEMIGOS_COMBATE) {
        return 0;
    }
    Fase_t *fase_ptr = &zona_ptr->fases[zona_ptr->cantFases];
    strncpy(fase_ptr->amenaza, amenaza_ptr, sizeof(fase_ptr->amenaza) - 1);
    fase_ptr->amenaza[sizeof(fase_ptr->amenaza) - 1] = '\0';
    for (int i = 0; i < cantEnemigos; i++) {
        fase_ptr->batalla.enemigos[i] = enemigos_ptr[i];
    }
    fase_ptr->batalla.cantEnemigos = cantEnemigos;
    fase_ptr->fogataAntes = fogataAntes;
    zona_ptr->cantFases++;
    return 1;
}

// ============================================================
// Zona 1: Campo Ceniza (en solitario)
// ============================================================

static Enemigo_t crearCentinela(void) {
    return crearEnemigo("Centinela Canino", HABILIDAD_ENEMIGO_NINGUNA, 120, 8, 48);
}

static Enemigo_t crearCazador(void) {
    return crearEnemigo("Cazador de Campo Ceniza", HABILIDAD_ENEMIGO_TRAMPA, 160, 16, 56);
}

static Zona_t crearZonaCampoCeniza(void) {
    Zona_t zona = crearZona(1, "Campo Ceniza",
        "Lo que fue un pueblo agricola es hoy tierra quemada, vigilada por\n"
        "cazadores al servicio de las maquinas y sus perros mecanicos.",
        "entre las ruinas del pueblo");

    Enemigo_t centinela = crearCentinela();
    Enemigo_t cazador = crearCazador();
    Enemigo_t spectre = crearJefe("Spectre", HABILIDAD_ENEMIGO_CRITICO, 280, 40, 60);

    Enemigo_t fase1[] = { centinela, centinela };
    agregarFase(&zona, "dos perros mecanicos emergen entre la ceniza y corren directo hacia vos.",
                fase1, 2, 0);

    Enemigo_t fase2[] = { cazador };
    agregarFase(&zona, "un cazador sale de entre los restos de un granero y te apunta.",
                fase2, 1, 0);

    Enemigo_t fase3[] = { cazador, centinela, centinela };
    agregarFase(&zona, "otro cazador aparece, esta vez acompañado por sus perros.",
                fase3, 3, 0);

    // Fase del jefe: antes se descansa en una fogata.
    Enemigo_t fase4[] = { cazador, spectre };
    agregarFase(&zona, "una figura enorme avanza detras de un cazador: es Spectre.",
                fase4, 2, 1);

    return zona;
}

Zona_t crearZonaPorNumero(int numero) {
    switch (numero) {
        case 1:
        default:
            return crearZonaCampoCeniza();
    }
}
