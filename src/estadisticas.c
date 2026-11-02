#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/estadisticas.h"
#include "../include/utils.h"

#define CAPACIDAD_INICIAL 8

// Totales calculados a partir de los registros de todas las rondas.
typedef struct {
    int danoCausado;
    int danoRecibido;
    int enemigosDerrotados;
    int mejorDanoRonda;
    int rondasCombateLargo;
    int zonaCombateLargo;
    int faseCombateLargo;
} ResumenPartida_t;

Estadisticas_t crearEstadisticas(void) {
    Estadisticas_t estadisticas = { NULL, 0, 0 };
    estadisticas.registros_ptr = malloc(CAPACIDAD_INICIAL * sizeof(RegistroRonda_t));
    if (estadisticas.registros_ptr != NULL) {
        estadisticas.capacidad = CAPACIDAD_INICIAL;
    }
    return estadisticas;
}

int agregarRegistroRonda(Estadisticas_t *estadisticas_ptr, RegistroRonda_t registro) {
    if (estadisticas_ptr->cantidad == estadisticas_ptr->capacidad) {
        int capacidadNueva = estadisticas_ptr->capacidad > 0 ? estadisticas_ptr->capacidad * 2 : CAPACIDAD_INICIAL;
        // Si realloc falla devuelve NULL pero el bloque viejo sigue reservado:
        // por eso se usa un puntero temporal y no se pisa registros_ptr.
        RegistroRonda_t *nuevos_ptr = realloc(estadisticas_ptr->registros_ptr,
                                              (size_t) capacidadNueva * sizeof(RegistroRonda_t));
        if (nuevos_ptr == NULL) {
            return 0;
        }
        estadisticas_ptr->registros_ptr = nuevos_ptr;
        estadisticas_ptr->capacidad = capacidadNueva;
    }
    estadisticas_ptr->registros_ptr[estadisticas_ptr->cantidad] = registro;
    estadisticas_ptr->cantidad++;
    return 1;
}

static ResumenPartida_t calcularResumen(const Estadisticas_t *estadisticas_ptr) {
    ResumenPartida_t resumen = { 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < estadisticas_ptr->cantidad; i++) {
        const RegistroRonda_t *registro_ptr = &estadisticas_ptr->registros_ptr[i];
        resumen.danoCausado += registro_ptr->danoCausado;
        resumen.danoRecibido += registro_ptr->danoRecibido;
        resumen.enemigosDerrotados += registro_ptr->enemigosDerrotados;
        if (registro_ptr->danoCausado > resumen.mejorDanoRonda) {
            resumen.mejorDanoRonda = registro_ptr->danoCausado;
        }
        // Las rondas de un combate van en orden: la ultima tiene el total.
        if (registro_ptr->ronda > resumen.rondasCombateLargo) {
            resumen.rondasCombateLargo = registro_ptr->ronda;
            resumen.zonaCombateLargo = registro_ptr->zona;
            resumen.faseCombateLargo = registro_ptr->fase;
        }
    }
    return resumen;
}

void mostrarEstadisticas(const Estadisticas_t *estadisticas_ptr) {
    ResumenPartida_t resumen = calcularResumen(estadisticas_ptr);
    printf("\n%s%s===== ESTADISTICAS DE LA PARTIDA =====%s\n",
           estiloTerminal(COLOR_AZUL_CLARO), estiloTerminal(ESTILO_NEGRITA), estiloTerminal(COLOR_RESET));
    printf("Rondas jugadas:      %d\n", estadisticas_ptr->cantidad);
    printf("Dano causado:        %d\n", resumen.danoCausado);
    printf("Dano recibido:       %d\n", resumen.danoRecibido);
    printf("Enemigos derrotados: %d\n", resumen.enemigosDerrotados);
    printf("Mejor ronda:         %d de dano\n", resumen.mejorDanoRonda);
    if (resumen.rondasCombateLargo > 0) {
        printf("Combate mas largo:   %d rondas (zona %d, fase %d)\n", resumen.rondasCombateLargo,
               resumen.zonaCombateLargo, resumen.faseCombateLargo);
    }
    printf("Memoria reservada:   %d registros de %zu bytes\n", estadisticas_ptr->capacidad,
           sizeof(RegistroRonda_t));
}

int guardarEstadisticas(const Estadisticas_t *estadisticas_ptr) {
    FILE *archivo_ptr = fopen(ARCHIVO_ESTADISTICAS, "a");
    if (archivo_ptr == NULL) {
        return 0;
    }
    ResumenPartida_t resumen = calcularResumen(estadisticas_ptr);
    time_t ahora = time(NULL);
    char fechaTexto[26];
    snprintf(fechaTexto, sizeof(fechaTexto), "%s", ctime(&ahora));
    fechaTexto[24] = '\0'; // quitar el salto de linea que agrega ctime
    fprintf(archivo_ptr, "[%s] rondas=%d causado=%d recibido=%d derrotados=%d mejor_ronda=%d\n",
            fechaTexto, estadisticas_ptr->cantidad, resumen.danoCausado, resumen.danoRecibido,
            resumen.enemigosDerrotados, resumen.mejorDanoRonda);
    return fclose(archivo_ptr) == 0;
}

void liberarEstadisticas(Estadisticas_t *estadisticas_ptr) {
    free(estadisticas_ptr->registros_ptr);
    estadisticas_ptr->registros_ptr = NULL;
    estadisticas_ptr->cantidad = 0;
    estadisticas_ptr->capacidad = 0;
}
