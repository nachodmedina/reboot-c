#include <assert.h>
#include <stdio.h>
#include "../include/progresion.h"

static Personaje_t crearPersonajePrueba(void) {
    return crearPersonaje("Isaac", "Espada", HABILIDAD_CRITICO_GRUPAL, 320, 40, 80);
}

static Enemigo_t crearEnemigoPrueba(void) {
    return crearEnemigo("Centinela", HABILIDAD_ENEMIGO_NINGUNA, 120, 8, 48);
}

static void testCadaMejoraYContrabonus(void) {
    Personaje_t equipo[3] = {
        crearPersonajePrueba(),
        crearPersonajePrueba(),
        crearPersonajePrueba()
    };
    int desbloqueados[3] = { 1, 0, 0 };
    ProgresoPartida_t progreso = crearProgresoPartida();

    equipo[0].vida = 200;
    assert(aplicarMejoraEquipo(equipo, desbloqueados, 3, MEJORA_VIDA, &progreso));
    assert(equipo[0].vidaMax == 350);
    assert(equipo[0].vida == 230);
    assert(equipo[1].vidaMax == 320);
    assert(progreso.enemigos.ataque == aumentoContrabonus(MEJORA_VIDA));

    assert(aplicarMejoraEquipo(equipo, desbloqueados, 3, MEJORA_ATAQUE, &progreso));
    assert(equipo[0].ataque == 80 + aumentoMejora(MEJORA_ATAQUE));
    assert(progreso.enemigos.defensa == aumentoContrabonus(MEJORA_ATAQUE));

    assert(aplicarMejoraEquipo(equipo, desbloqueados, 3, MEJORA_DEFENSA, &progreso));
    assert(equipo[0].defensa == 40 + aumentoMejora(MEJORA_DEFENSA));
    assert(progreso.enemigos.vidaMaxima == aumentoContrabonus(MEJORA_DEFENSA));
    assert(contrabonusAcumulado(MEJORA_VIDA, &progreso) == 8);
    assert(contrabonusAcumulado(MEJORA_ATAQUE, &progreso) == 5);
    assert(contrabonusAcumulado(MEJORA_DEFENSA, &progreso) == 15);

    assert(aplicarMejoraEquipo(equipo, desbloqueados, 3, MEJORA_VIDA, &progreso));
    assert(equipo[0].vidaMax == 380);
    assert(equipo[0].vida == 260);
    assert(contrabonusAcumulado(MEJORA_VIDA, &progreso) == 16);
}

static void testMejorasAcumuladasEnAliadoNuevo(void) {
    Personaje_t aliado = crearPersonajePrueba();
    ProgresoPartida_t progreso = crearProgresoPartida();
    int desbloqueados[3] = { 1, 0, 0 };
    Personaje_t equipo[3] = { crearPersonajePrueba(), aliado, crearPersonajePrueba() };

    assert(aplicarMejoraEquipo(equipo, desbloqueados, 3, MEJORA_VIDA, &progreso));
    assert(aplicarMejoraEquipo(equipo, desbloqueados, 3, MEJORA_ATAQUE, &progreso));
    assert(aplicarMejorasAcumuladas(&equipo[1], &progreso));
    assert(equipo[1].vidaMax == 350);
    assert(equipo[1].vida == 350);
    assert(equipo[1].ataque == 95);
    assert(equipo[1].defensa == 40);
}

static void testMejoraDeVidaNoRevive(void) {
    Personaje_t muerto = crearPersonajePrueba();
    muerto.vida = 0;
    Personaje_t equipo[1] = { muerto };
    int desbloqueados[1] = { 1 };
    ProgresoPartida_t progreso = crearProgresoPartida();

    assert(aplicarMejoraEquipo(equipo, desbloqueados, 1, MEJORA_VIDA, &progreso));
    assert(equipo[0].vidaMax == 350);
    assert(equipo[0].vida == 0);

    Personaje_t futuro = crearPersonajePrueba();
    futuro.vida = 0;
    assert(aplicarMejorasAcumuladas(&futuro, &progreso));
    assert(futuro.vidaMax == 350);
    assert(futuro.vida == 0);
}

static void testContrabonusEnemigoSinModificarPlantilla(void) {
    Enemigo_t plantilla = crearEnemigoPrueba();
    Enemigo_t copia = plantilla;
    ProgresoPartida_t progreso = crearProgresoPartida();
    Personaje_t equipo[1] = { crearPersonajePrueba() };
    int desbloqueados[1] = { 1 };

    assert(aplicarMejoraEquipo(equipo, desbloqueados, 1, MEJORA_VIDA, &progreso));
    assert(aplicarMejoraEquipo(equipo, desbloqueados, 1, MEJORA_ATAQUE, &progreso));
    assert(aplicarMejoraEquipo(equipo, desbloqueados, 1, MEJORA_DEFENSA, &progreso));
    assert(aplicarContrabonusEnemigos(&copia, 1, &progreso));
    assert(copia.vidaMax == 135);
    assert(copia.vida == 135);
    assert(copia.ataque == 56);
    assert(copia.defensa == 13);
    assert(plantilla.vidaMax == 120);
    assert(plantilla.ataque == 48);
    assert(plantilla.defensa == 8);
}

static void testRutasDeAliados(void) {
    const int esperados[3][2] = {
        { INDICE_FRIEREN, INDICE_DANTE },
        { INDICE_FRIEREN, INDICE_ISAAC },
        { INDICE_ISAAC, INDICE_DANTE }
    };

    for (int protagonista = 0; protagonista < 3; protagonista++) {
        for (int zona = 1; zona <= 2; zona++) {
            assert(aliadoQueSeUne(protagonista, zona) == esperados[protagonista][zona - 1]);
        }
    }
    assert(aliadoQueSeUne(-1, 1) == -1);
    assert(aliadoQueSeUne(INDICE_ISAAC, 0) == -1);
}

static void testArgumentosInvalidos(void) {
    Personaje_t equipo[1] = { crearPersonajePrueba() };
    int desbloqueados[1] = { 1 };
    ProgresoPartida_t progreso = crearProgresoPartida();
    Enemigo_t enemigo = crearEnemigoPrueba();

    assert(!aplicarMejoraEquipo(equipo, desbloqueados, 1, (TipoMejora) 99, &progreso));
    assert(!aplicarMejoraEquipo(equipo, desbloqueados, 1, MEJORA_VIDA, NULL));
    assert(!aplicarMejorasAcumuladas(NULL, &progreso));
    assert(!aplicarContrabonusEnemigos(NULL, 1, &progreso));
    assert(!aplicarContrabonusEnemigos(&enemigo, -1, &progreso));
}

int main(void) {
    testCadaMejoraYContrabonus();
    testMejorasAcumuladasEnAliadoNuevo();
    testMejoraDeVidaNoRevive();
    testContrabonusEnemigoSinModificarPlantilla();
    testRutasDeAliados();
    testArgumentosInvalidos();
    printf("===== Todos los tests de progresion pasaron =====\n");
    return 0;
}
