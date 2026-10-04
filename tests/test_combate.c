#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/combate.h"
#include "../include/items.h"

static Personaje crearIsaac(void) {
    return crearPersonaje("Isaac", "Espada", HABILIDAD_CRITICO_GRUPAL, 320, 40, 80);
}

static Personaje crearDante(void) {
    return crearPersonaje("Dante", "Lanza", HABILIDAD_DEFENSA_GRUPAL, 320, 40, 80);
}

static Personaje crearFrieren(void) {
    return crearPersonaje("Frieren", "Baston", HABILIDAD_CURACION_GRUPAL, 320, 40, 80);
}

static Enemigo crearCentinela(void) {
    return crearEnemigo("Centinela Canino", HABILIDAD_ENEMIGO_NINGUNA, 120, 8, 48);
}

static Enemigo crearCazador(void) {
    return crearEnemigo("Cazador de Campo Ceniza", HABILIDAD_ENEMIGO_TRAMPA, 160, 16, 56);
}

static Enemigo crearSpectre(void) {
    return crearEnemigo("Spectre", HABILIDAD_ENEMIGO_CRITICO, 280, 40, 60);
}

// ============================================================
// Tests unitarios
// ============================================================

static void testCalcularDano(void) {
    assert(calcularDano(80, 40) == 40);
    assert(calcularDano(40, 40) == 0);
    assert(calcularDano(30, 80) == 0);
}

static void testDanoArea(void) {
    assert(calcularDanoArea(80, 8) == 47);   // 80/2 + 15 - 8
    assert(calcularDanoArea(80, 60) == 0);
}

static void testAtaqueIndividualYCritico(void) {
    Personaje isaac = crearIsaac();
    EfectoGrupal efecto = crearEfectoGrupal();

    Enemigo normal = crearCentinela();
    ataqueIndividual(isaac, &normal, efecto);
    assert(normal.vida == 120 - 72);

    efecto.tipo = HABILIDAD_CRITICO_GRUPAL;
    Enemigo conCritico = crearCentinela();
    ataqueIndividual(isaac, &conCritico, efecto);
    assert(conCritico.vida == 120 - 86);     // 72 * 1.2

    Enemigo casiMuerto = crearCentinela();
    casiMuerto.vida = 10;
    ataqueIndividual(isaac, &casiMuerto, crearEfectoGrupal());
    assert(casiMuerto.vida == 0);            // nunca queda negativa
}

static void testAtaqueAreaIgnoraMuertos(void) {
    Personaje isaac = crearIsaac();
    Enemigo enemigos[3] = { crearCentinela(), crearCentinela(), crearCentinela() };
    enemigos[1].vida = 0;

    ataqueArea(isaac, enemigos, 3, crearEfectoGrupal());
    assert(enemigos[0].vida == 120 - 47);
    assert(enemigos[1].vida == 0);
    assert(enemigos[2].vida == 120 - 47);
}

static void testDebilitado(void) {
    Personaje isaac = crearIsaac();
    assert(ataqueEfectivo(isaac) == 80);

    Estado debilitado = { ESTADO_DEBILITADO, 1, 25 };
    aplicarEstadoPersonaje(&isaac, debilitado);
    assert(ataqueEfectivo(isaac) == 60);

    actualizarEstadoPersonaje(&isaac);       // dura 1 turno
    assert(isaac.estadoActivo.tipo == ESTADO_NINGUNO);
    assert(ataqueEfectivo(isaac) == 80);
}

// La trampa es aleatoria: en 100 golpes tiene que activarse algunas veces, pero no siempre.
static void testTrampaDebilitaAVeces(void) {
    int debilitados = 0;
    for (int i = 0; i < 100; i++) {
        Personaje equipo[1] = { crearIsaac() };
        turnoEnemigo(crearCazador(), equipo, 1, crearEfectoGrupal());
        assert(equipo[0].vida == 320 - 16);
        if (equipo[0].estadoActivo.tipo == ESTADO_DEBILITADO) {
            debilitados++;
        }
    }
    assert(debilitados > 0);
    assert(debilitados < 100);
}

static void testEnemigoNoAtacaAMuertos(void) {
    Personaje equipo[2] = { crearIsaac(), crearDante() };
    equipo[0].vida = 0;
    turnoEnemigo(crearCentinela(), equipo, 2, crearEfectoGrupal());
    assert(equipo[0].vida == 0);
    assert(equipo[1].vida == 320 - 8);
}

static void testDefensaGrupal(void) {
    Enemigo golpeador = crearEnemigo("Golpeador", HABILIDAD_ENEMIGO_NINGUNA, 100, 0, 64);

    Personaje sinDefensa[1] = { crearDante() };
    turnoEnemigo(golpeador, sinDefensa, 1, crearEfectoGrupal());
    assert(sinDefensa[0].vida == 320 - 24);  // 64 - 40

    Personaje conDefensa[1] = { crearDante() };
    EfectoGrupal efecto = crearEfectoGrupal();
    assert(activarHabilidad(conDefensa[0], conDefensa, 1, &efecto) == 1);
    turnoEnemigo(golpeador, conDefensa, 1, efecto);
    assert(conDefensa[0].vida == 320 - 4);   // 64 - 60
}

static void testCuracionGrupalDosVeces(void) {
    Personaje equipo[3] = { crearFrieren(), crearIsaac(), crearDante() };
    equipo[0].vida = 100;
    equipo[1].vida = 300;
    equipo[2].vida = 0;                      // muerto: no se cura

    EfectoGrupal efecto = crearEfectoGrupal();
    assert(activarHabilidad(equipo[0], equipo, 3, &efecto) == 1);
    assert(equipo[0].vida == 140);           // 1a curacion, al activarla
    assert(equipo[1].vida == 320);           // no pasa de vidaMax
    assert(equipo[2].vida == 0);

    cerrarRonda(&efecto);
    iniciarRonda(equipo, 3, efecto);
    assert(equipo[0].vida == 180);           // 2a curacion, ronda siguiente

    cerrarRonda(&efecto);
    assert(efecto.tipo == HABILIDAD_NINGUNA);
    iniciarRonda(equipo, 3, efecto);
    assert(equipo[0].vida == 180);           // no hay 3a curacion
}

static void testHabilidadUnaVezPorCombate(void) {
    Personaje equipo[1] = { crearIsaac() };
    EfectoGrupal efecto = crearEfectoGrupal();
    assert(activarHabilidad(equipo[0], equipo, 1, &efecto) == 1);
    assert(efecto.tipo == HABILIDAD_CRITICO_GRUPAL);
    assert(activarHabilidad(equipo[0], equipo, 1, &efecto) == 0);

    cerrarRonda(&efecto);
    assert(efecto.tipo == HABILIDAD_CRITICO_GRUPAL);  // sigue en la 2a ronda
    cerrarRonda(&efecto);
    assert(efecto.tipo == HABILIDAD_NINGUNA);
    assert(activarHabilidad(equipo[0], equipo, 1, &efecto) == 0);

    Personaje sinHabilidad[1] = { crearPersonaje("Nadie", "Palo", HABILIDAD_NINGUNA, 100, 10, 10) };
    EfectoGrupal otro = crearEfectoGrupal();
    assert(activarHabilidad(sinHabilidad[0], sinHabilidad, 1, &otro) == 0);
}

static void testUsarItem(void) {
    Personaje isaac = crearIsaac();
    isaac.vida = 100;

    Inventario *inventario = crearInventario(1);
    Item pocion = { ITEM_VIDA, 100, 0 };
    Item gema = { ITEM_ATAQUE, 10, 0 };
    agregarItem(inventario, pocion);
    agregarItem(inventario, gema);           // fuerza el realloc
    assert(inventario->cantidad == 2);

    usarItem(inventario, 0, &isaac);
    assert(isaac.vida == 200);
    assert(inventario->cantidad == 1);
    assert(inventario->items[0].tipo == ITEM_ATAQUE);

    usarItem(inventario, 0, &isaac);
    assert(isaac.ataque == 90);
    assert(inventario->cantidad == 0);

    liberarInventario(inventario);
}

// ============================================================
// Simulacion de Zona 1 (sin teclado)
// Peor caso de la doc: sin habilidad ni items; 1 a 1, y area
// solo con 3 o mas enemigos vivos. Protagonista solo.
// ============================================================

static int primerEnemigoVivo(Enemigo enemigos[], int cant) {
    for (int i = 0; i < cant; i++) {
        if (estaVivoEnemigo(&enemigos[i])) {
            return i;
        }
    }
    return -1;
}

// Igual que iniciarCombate, pero el jugador decide solo. Devuelve 1 si gana.
static int combateAutomatico(Personaje equipo[], int cantP, Enemigo enemigos[], int cantE) {
    EfectoGrupal efecto = crearEfectoGrupal();
    while (1) {
        iniciarRonda(equipo, cantP, efecto);
        for (int i = 0; i < cantP && quedanEnemigosVivos(enemigos, cantE); i++) {
            if (!estaVivoPersonaje(&equipo[i])) {
                continue;
            }
            if (quedanEnemigosVivos(enemigos, cantE) >= 3) {
                ataqueArea(equipo[i], enemigos, cantE, efecto);
            } else {
                ataqueIndividual(equipo[i], &enemigos[primerEnemigoVivo(enemigos, cantE)], efecto);
            }
            actualizarEstadoPersonaje(&equipo[i]);
        }
        if (!quedanEnemigosVivos(enemigos, cantE)) {
            return 1;
        }
        for (int j = 0; j < cantE; j++) {
            if (estaVivoEnemigo(&enemigos[j])) {
                turnoEnemigo(enemigos[j], equipo, cantP, efecto);
                actualizarEstadoEnemigo(&enemigos[j]);
            }
        }
        if (!quedanPersonajesVivos(equipo, cantP)) {
            return 0;
        }
        cerrarRonda(&efecto);
    }
}

static void informarFase(const char *fase, int gano, Personaje protagonista) {
    printf("\n>>> %s: %s - vida %d/%d (%d%%)\n\n", fase, gano ? "GANA" : "PIERDE",
           protagonista.vida, protagonista.vidaMax, protagonista.vida * 100 / protagonista.vidaMax);
}

static void simularZona1(void) {
    Personaje equipo[1] = { crearIsaac() };
    int gano;

    Enemigo fase1[2] = { crearCentinela(), crearCentinela() };
    gano = combateAutomatico(equipo, 1, fase1, 2);
    informarFase("Fase 1 (2 Centinela)", gano, equipo[0]);
    if (!gano) return;

    Enemigo fase2[1] = { crearCazador() };
    gano = combateAutomatico(equipo, 1, fase2, 1);
    informarFase("Fase 2 (1 Cazador)", gano, equipo[0]);
    if (!gano) return;

    Enemigo fase3[3] = { crearCazador(), crearCentinela(), crearCentinela() };
    gano = combateAutomatico(equipo, 1, fase3, 3);
    informarFase("Fase 3 (1 Cazador + 2 Centinela)", gano, equipo[0]);
    if (!gano) return;

    equipo[0].vida = equipo[0].vidaMax;
    equipo[0].estadoActivo.tipo = ESTADO_NINGUNO;
    printf(">>> Fogata: vida al 100%%\n");

    Enemigo fase4[2] = { crearCazador(), crearSpectre() };
    gano = combateAutomatico(equipo, 1, fase4, 2);
    informarFase("Fase 4 (Cazador + Spectre)", gano, equipo[0]);
}

int main(int argc, char *argv[]) {
    testCalcularDano();
    testDanoArea();
    testAtaqueIndividualYCritico();
    testAtaqueAreaIgnoraMuertos();
    testDebilitado();
    testTrampaDebilitaAVeces();
    testEnemigoNoAtacaAMuertos();
    testDefensaGrupal();
    testCuracionGrupalDosVeces();
    testHabilidadUnaVezPorCombate();
    testUsarItem();
    printf("\n===== Todos los tests pasaron =====\n");

    unsigned int semilla = argc > 1 ? (unsigned int) atoi(argv[1]) : 42;
    srand(semilla);
    printf("\n===== Simulacion Zona 1 (semilla %u) =====\n", semilla);
    simularZona1();
    return 0;
}
