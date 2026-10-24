#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/combate.h"
#include "../include/items.h"
#include "../include/archivos.h"

static Personaje_t crearIsaac(void) {
    return crearPersonaje("Isaac", "Espada", HABILIDAD_CRITICO_GRUPAL, 320, 40, 80);
}

static Personaje_t crearDante(void) {
    return crearPersonaje("Dante", "Lanza", HABILIDAD_DEFENSA_GRUPAL, 320, 40, 80);
}

static Personaje_t crearFrieren(void) {
    return crearPersonaje("Frieren", "Baston", HABILIDAD_CURACION_GRUPAL, 320, 40, 80);
}

static Enemigo_t crearCentinela(void) {
    return crearEnemigo("Centinela Canino", HABILIDAD_ENEMIGO_NINGUNA, 120, 8, 48);
}

static Enemigo_t crearCazador(void) {
    return crearEnemigo("Cazador de Campo Ceniza", HABILIDAD_ENEMIGO_TRAMPA, 160, 16, 56);
}

static Enemigo_t crearSpectre(void) {
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

static void testIdentificarJefePorNombre(void) {
    Enemigo_t jefe = crearSpectre();
    Enemigo_t enemigo = crearCentinela();

    assert(esJefeEnemigo(&jefe));
    assert(!esJefeEnemigo(&enemigo));
    assert(!esJefeEnemigo(NULL));
}

static void testDanoArea(void) {
    assert(calcularDanoArea(80, 8) == 47);   // 80/2 + 15 - 8
    assert(calcularDanoArea(80, 60) == 0);
}

static void testAtaqueIndividualYCritico(void) {
    Personaje_t isaac = crearIsaac();
    EfectoGrupal_t efecto = crearEfectoGrupal();

    Enemigo_t normal = crearCentinela();
    ataqueIndividual(isaac, &normal, efecto);
    assert(normal.vida == 120 - 72);

    efecto.tipo = EFECTO_GRUPAL_CRITICO;
    Enemigo_t conCritico = crearCentinela();
    ataqueIndividual(isaac, &conCritico, efecto);
    assert(conCritico.vida == 120 - 86);     // 72 * 1.2

    Enemigo_t casiMuerto = crearCentinela();
    casiMuerto.vida = 10;
    ataqueIndividual(isaac, &casiMuerto, crearEfectoGrupal());
    assert(casiMuerto.vida == 0);            // nunca queda negativa
}

static void testAtaqueAreaIgnoraMuertos(void) {
    Personaje_t isaac = crearIsaac();
    Enemigo_t enemigos[3] = { crearCentinela(), crearCentinela(), crearCentinela() };
    enemigos[1].vida = 0;

    ataqueArea(isaac, enemigos, 3, crearEfectoGrupal());
    assert(enemigos[0].vida == 120 - 47);
    assert(enemigos[1].vida == 0);
    assert(enemigos[2].vida == 120 - 47);
}

static void testDebilitado(void) {
    Personaje_t isaac = crearIsaac();
    assert(ataqueEfectivo(isaac) == 80);

    Estado_t debilitado = { ESTADO_DEBILITADO, 1, 25 };
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
        Personaje_t equipo[1] = { crearIsaac() };
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
    Personaje_t equipo[2] = { crearIsaac(), crearDante() };
    equipo[0].vida = 0;
    turnoEnemigo(crearCentinela(), equipo, 2, crearEfectoGrupal());
    assert(equipo[0].vida == 0);
    assert(equipo[1].vida == 320 - 8);
}

static void testDefensaGrupal(void) {
    Enemigo_t golpeador = crearEnemigo("Golpeador", HABILIDAD_ENEMIGO_NINGUNA, 100, 0, 64);

    Personaje_t sinDefensa[1] = { crearDante() };
    turnoEnemigo(golpeador, sinDefensa, 1, crearEfectoGrupal());
    assert(sinDefensa[0].vida == 320 - 24);  // 64 - 40

    Personaje_t conDefensa[1] = { crearDante() };
    EfectoGrupal_t efecto = crearEfectoGrupal();
    assert(activarHabilidad(conDefensa[0], conDefensa, 1, &efecto) == 1);
    turnoEnemigo(golpeador, conDefensa, 1, efecto);
    assert(conDefensa[0].vida == 320 - 4);   // 64 - 60
}

static void testCuracionGrupalDosVeces(void) {
    Personaje_t equipo[3] = { crearFrieren(), crearIsaac(), crearDante() };
    equipo[0].vida = 100;
    equipo[1].vida = 300;
    equipo[2].vida = 0;                      // muerto: no se cura

    EfectoGrupal_t efecto = crearEfectoGrupal();
    assert(activarHabilidad(equipo[0], equipo, 3, &efecto) == 1);
    assert(equipo[0].vida == 140);           // 1a curacion, al activarla
    assert(equipo[1].vida == 320);           // no pasa de vidaMax
    assert(equipo[2].vida == 0);

    cerrarRonda(&efecto);
    iniciarRonda(equipo, 3, efecto);
    assert(equipo[0].vida == 180);           // 2a curacion, ronda siguiente

    cerrarRonda(&efecto);
    assert(efecto.tipo == EFECTO_GRUPAL_INACTIVO);
    iniciarRonda(equipo, 3, efecto);
    assert(equipo[0].vida == 180);           // no hay 3a curacion
}

static void testHabilidadSePuedeReactivarTrasExpirar(void) {
    Personaje_t equipo[1] = { crearIsaac() };
    EfectoGrupal_t efecto = crearEfectoGrupal();
    assert(efecto.tipo == EFECTO_GRUPAL_INACTIVO);
    assert(activarHabilidad(equipo[0], equipo, 1, &efecto) == 1);
    assert(efecto.tipo == EFECTO_GRUPAL_CRITICO);
    assert(efecto.turnosRestantes == 2);
    assert(activarHabilidad(equipo[0], equipo, 1, &efecto) == 0);

    cerrarRonda(&efecto);
    assert(efecto.tipo == EFECTO_GRUPAL_CRITICO);  // sigue en la 2a ronda
    assert(activarHabilidad(equipo[0], equipo, 1, &efecto) == 0);
    cerrarRonda(&efecto);
    assert(efecto.tipo == EFECTO_GRUPAL_INACTIVO);
    assert(efecto.turnosRestantes == 0);
    assert(activarHabilidad(equipo[0], equipo, 1, &efecto) == 1);
    assert(efecto.tipo == EFECTO_GRUPAL_CRITICO);
    assert(efecto.turnosRestantes == 2);
}

static void testUsarItem(void) {
    Personaje_t isaac = crearIsaac();
    isaac.vida = 100;

    Inventario_t inventario = crearInventario(1);
    Item_t pocionVida = { ITEM_VIDA, 100 };
    Item_t pocionVeneno = { ITEM_VENENO, 5 };
    assert(agregarItem(&inventario, pocionVida));
    assert(agregarItem(&inventario, pocionVeneno));
    assert(inventario.cantidad == 2);

    assert(usarItem(&inventario, 0, &isaac, NULL));
    assert(isaac.vida == 200);
    assert(inventario.cantidad == 1);
    assert(inventario.items[0].tipo == ITEM_VENENO);

    Enemigo_t centinela = crearCentinela();
    assert(usarItem(&inventario, 0, &isaac, &centinela));
    assert(inventario.cantidad == 0);
    assert(centinela.estadoActivo.tipo == ESTADO_VENENO);
    assert(centinela.estadoActivo.duracionTurnos == 4);
    assert(centinela.estadoActivo.magnitud == 5);
}

static void testCapacidadBolsaCompartida(void) {
    Inventario_t inventario = crearInventario(1);
    Item_t pocion = { ITEM_VIDA, 100 };

    assert(inventario.capacidad == 5);
    for (int i = 0; i < 5; i++) {
        assert(agregarItem(&inventario, pocion));
    }
    assert(!agregarItem(&inventario, pocion));
    assert(actualizarCapacidadInventario(&inventario, 2));
    assert(inventario.capacidad == 10);
    assert(agregarItem(&inventario, pocion));
    assert(inventario.cantidad == 6);
}

static void testVenenoDuraCuatroTurnosYSeRenueva(void) {
    Enemigo_t enemigo = crearCentinela();
    assert(aplicarVenenoEnemigo(&enemigo, 5, 4));

    actualizarEstadoEnemigo(&enemigo);
    assert(enemigo.vida == 115);
    assert(enemigo.estadoActivo.duracionTurnos == 3);

    assert(aplicarVenenoEnemigo(&enemigo, 5, 4));
    assert(enemigo.estadoActivo.duracionTurnos == 4);
    for (int i = 0; i < 4; i++) {
        actualizarEstadoEnemigo(&enemigo);
    }
    assert(enemigo.vida == 95);
    assert(enemigo.estadoActivo.tipo == ESTADO_NINGUNO);

    Estado_t debilitado = { ESTADO_DEBILITADO, 1, 25 };
    aplicarEstadoEnemigo(&enemigo, debilitado);
    assert(!aplicarVenenoEnemigo(&enemigo, 5, 4));
    assert(enemigo.estadoActivo.tipo == ESTADO_DEBILITADO);
}

static void testGuardarYRecuperarInventarioCompartido(void) {
    char ruta[80];
    unsigned int intento = 0;
    FILE *archivo_ptr;
    do {
        snprintf(ruta, sizeof(ruta), "partida_test_%ld_%u.dat",
                 (long) time(NULL), intento++);
        archivo_ptr = fopen(ruta, "rb");
        if (archivo_ptr != NULL) {
            fclose(archivo_ptr);
        }
    } while (archivo_ptr != NULL);

    Personaje_t equipo[3] = { crearIsaac(), crearDante(), crearFrieren() };
    int desbloqueados[3] = { 1, 1, 0 };
    Inventario_t inventario = crearInventario(2);
    ProgresoPartida_t progreso = crearProgresoPartida();
    progreso.personajes.ataque = 15;
    progreso.enemigos.vidaMaxima = 15;
    assert(agregarItem(&inventario, (Item_t) { ITEM_VIDA, 100 }));
    assert(agregarItem(&inventario, (Item_t) { ITEM_VENENO, 5 }));

    assert(guardarPartida(ruta, equipo, 3, desbloqueados, &inventario, &progreso));
    assert(agregarItem(&inventario, (Item_t) { ITEM_VENENO, 5 }));
    assert(guardarPartida(ruta, equipo, 3, desbloqueados, &inventario, &progreso));
    Inventario_t inventarioInvalido = inventario;
    inventarioInvalido.cantidad = inventarioInvalido.capacidad + 1;
    assert(!guardarPartida(ruta, equipo, 3, desbloqueados, &inventarioInvalido, &progreso));

    Personaje_t equipoCargado[3];
    int desbloqueadosCargados[3] = { 0, 0, 0 };
    Inventario_t inventarioCargado = crearInventario(0);
    ProgresoPartida_t progresoCargado = crearProgresoPartida();
    assert(cargarPartida(ruta, equipoCargado, 3, desbloqueadosCargados,
                         &inventarioCargado, &progresoCargado));
    assert(desbloqueadosCargados[0] == 1);
    assert(desbloqueadosCargados[1] == 1);
    assert(desbloqueadosCargados[2] == 0);
    assert(inventarioCargado.cantidad == 3);
    assert(inventarioCargado.capacidad == 10);
    assert(inventarioCargado.items[0].tipo == ITEM_VIDA);
    assert(inventarioCargado.items[1].tipo == ITEM_VENENO);
    assert(inventarioCargado.items[2].tipo == ITEM_VENENO);
    assert(progresoCargado.personajes.ataque == 15);
    assert(progresoCargado.enemigos.vidaMaxima == 15);
    assert(strcmp(equipoCargado[1].nombre, equipo[1].nombre) == 0);

    archivo_ptr = fopen(ruta, "wb");
    assert(archivo_ptr != NULL);
    assert(fwrite("RE", 1, 2, archivo_ptr) == 2);
    assert(fclose(archivo_ptr) == 0);
    strcpy(equipoCargado[0].nombre, "Sin cambios");
    assert(!cargarPartida(ruta, equipoCargado, 3, desbloqueadosCargados,
                          &inventarioCargado, &progresoCargado));
    assert(strcmp(equipoCargado[0].nombre, "Sin cambios") == 0);
    assert(inventarioCargado.cantidad == 3);
    assert(remove(ruta) == 0);
}

// ============================================================
// Simulacion de Zona 1 (sin teclado)
// Peor caso de la doc: sin habilidad ni items; 1 a 1, y area
// solo con 3 o mas enemigos vivos. Protagonista solo.
// ============================================================

static int primerEnemigoVivo(Enemigo_t enemigos_ptr[], int cant) {
    for (int i = 0; i < cant; i++) {
        if (estaVivoEnemigo(&enemigos_ptr[i])) {
            return i;
        }
    }
    return -1;
}

// Igual que iniciarCombate, pero el jugador decide solo. Devuelve 1 si gana.
static int combateAutomatico(Personaje_t equipo_ptr[], int cantP, Enemigo_t enemigos_ptr[], int cantE) {
    EfectoGrupal_t efecto = crearEfectoGrupal();
    while (1) {
        iniciarRonda(equipo_ptr, cantP, efecto);
        for (int i = 0; i < cantP && quedanEnemigosVivos(enemigos_ptr, cantE); i++) {
            if (!estaVivoPersonaje(&equipo_ptr[i])) {
                continue;
            }
            if (quedanEnemigosVivos(enemigos_ptr, cantE) >= 3) {
                ataqueArea(equipo_ptr[i], enemigos_ptr, cantE, efecto);
            } else {
                ataqueIndividual(equipo_ptr[i], &enemigos_ptr[primerEnemigoVivo(enemigos_ptr, cantE)], efecto);
            }
            actualizarEstadoPersonaje(&equipo_ptr[i]);
        }
        if (!quedanEnemigosVivos(enemigos_ptr, cantE)) {
            return 1;
        }
        for (int j = 0; j < cantE; j++) {
            if (estaVivoEnemigo(&enemigos_ptr[j])) {
                turnoEnemigo(enemigos_ptr[j], equipo_ptr, cantP, efecto);
                actualizarEstadoEnemigo(&enemigos_ptr[j]);
            }
        }
        if (!quedanPersonajesVivos(equipo_ptr, cantP)) {
            return 0;
        }
        cerrarRonda(&efecto);
    }
}

static void informarFase(const char *fase_ptr, int gano, Personaje_t protagonista) {
    printf("\n>>> %s: %s - vida %d/%d (%d%%)\n\n", fase_ptr, gano ? "GANA" : "PIERDE",
           protagonista.vida, protagonista.vidaMax, protagonista.vida * 100 / protagonista.vidaMax);
}

static void simularZona1(void) {
    Personaje_t equipo[1] = { crearIsaac() };
    int gano;

    Enemigo_t fase1[2] = { crearCentinela(), crearCentinela() };
    gano = combateAutomatico(equipo, 1, fase1, 2);
    informarFase("Fase 1 (2 Centinela)", gano, equipo[0]);
    if (!gano) return;

    Enemigo_t fase2[1] = { crearCazador() };
    gano = combateAutomatico(equipo, 1, fase2, 1);
    informarFase("Fase 2 (1 Cazador)", gano, equipo[0]);
    if (!gano) return;

    Enemigo_t fase3[3] = { crearCazador(), crearCentinela(), crearCentinela() };
    gano = combateAutomatico(equipo, 1, fase3, 3);
    informarFase("Fase 3 (1 Cazador + 2 Centinela)", gano, equipo[0]);
    if (!gano) return;

    equipo[0].vida = equipo[0].vidaMax;
    equipo[0].estadoActivo.tipo = ESTADO_NINGUNO;
    printf(">>> Fogata: vida al 100%%\n");

    Enemigo_t fase4[2] = { crearCazador(), crearSpectre() };
    gano = combateAutomatico(equipo, 1, fase4, 2);
    informarFase("Fase 4 (Cazador + Spectre)", gano, equipo[0]);
}

int main(int argc, char *argv_ptr[]) {
    testCalcularDano();
    testIdentificarJefePorNombre();
    testDanoArea();
    testAtaqueIndividualYCritico();
    testAtaqueAreaIgnoraMuertos();
    testDebilitado();
    testTrampaDebilitaAVeces();
    testEnemigoNoAtacaAMuertos();
    testDefensaGrupal();
    testCuracionGrupalDosVeces();
    testHabilidadSePuedeReactivarTrasExpirar();
    testUsarItem();
    testCapacidadBolsaCompartida();
    testVenenoDuraCuatroTurnosYSeRenueva();
    testGuardarYRecuperarInventarioCompartido();
    printf("\n===== Todos los tests pasaron =====\n");

    unsigned int semilla = argc > 1 ? (unsigned int) atoi(argv_ptr[1]) : 42;
    srand(semilla);
    printf("\n===== Simulacion Zona 1 (semilla %u) =====\n", semilla);
    simularZona1();
    return 0;
}
