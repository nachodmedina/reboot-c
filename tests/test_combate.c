#define _POSIX_C_SOURCE 200809L   // dup, dup2, fileno (para silenciar la simulacion)
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <io.h>
#define dup _dup
#define dup2 _dup2
#define close _close
#define RUTA_NULA "NUL"
#else
#include <unistd.h>
#define RUTA_NULA "/dev/null"
#endif
#include "../include/archivos.h"
#include "../include/combate.h"
#include "../include/estadisticas.h"
#include "../include/items.h"
#include "../include/progresion.h"
#include "../include/zonas.h"

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
    return crearJefe("Spectre", HABILIDAD_ENEMIGO_CRITICO, 280, 40, 60);
}

// ============================================================
// Tests unitarios
// ============================================================

static void testCalcularDano(void) {
    assert(calcularDano(80, 40) == 40);
    assert(calcularDano(40, 40) == 0);
    assert(calcularDano(30, 80) == 0);
}

static void testIdentificarJefe(void) {
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
    ataqueIndividual(&isaac, &normal, efecto);
    assert(normal.vida == 120 - 72);

    efecto.tipo = EFECTO_GRUPAL_CRITICO;
    Enemigo_t conCritico = crearCentinela();
    ataqueIndividual(&isaac, &conCritico, efecto);
    assert(conCritico.vida == 120 - 86);     // 72 * 1.2

    Enemigo_t casiMuerto = crearCentinela();
    casiMuerto.vida = 10;
    ataqueIndividual(&isaac, &casiMuerto, crearEfectoGrupal());
    assert(casiMuerto.vida == 0);            // nunca queda negativa
}

static void testAtaqueAreaIgnoraMuertos(void) {
    Personaje_t isaac = crearIsaac();
    Enemigo_t enemigos[3] = { crearCentinela(), crearCentinela(), crearCentinela() };
    enemigos[1].vida = 0;

    ataqueArea(&isaac, enemigos, 3, crearEfectoGrupal());
    assert(enemigos[0].vida == 120 - 47);
    assert(enemigos[1].vida == 0);
    assert(enemigos[2].vida == 120 - 47);
}

static void testDebilitado(void) {
    Personaje_t isaac = crearIsaac();
    assert(ataqueEfectivo(&isaac) == 80);

    Estado_t debilitado = { ESTADO_DEBILITADO, 1, 25 };
    aplicarEstadoPersonaje(&isaac, debilitado);
    assert(ataqueEfectivo(&isaac) == 60);

    actualizarEstadoPersonaje(&isaac);       // dura 1 turno
    assert(isaac.estadoActivo.tipo == ESTADO_NINGUNO);
    assert(ataqueEfectivo(&isaac) == 80);
}

// La trampa es aleatoria: en 100 golpes tiene que activarse algunas veces, pero no siempre.
static void testTrampaDebilitaAVeces(void) {
    int debilitados = 0;
    Enemigo_t cazador = crearCazador();
    for (int i = 0; i < 100; i++) {
        Personaje_t equipo[1] = { crearIsaac() };
        turnoEnemigo(&cazador, equipo, 1, crearEfectoGrupal());
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
    Enemigo_t centinela = crearCentinela();
    turnoEnemigo(&centinela, equipo, 2, crearEfectoGrupal());
    assert(equipo[0].vida == 0);
    assert(equipo[1].vida == 320 - 8);
}

static void testDefensaGrupal(void) {
    Enemigo_t golpeador = crearEnemigo("Golpeador", HABILIDAD_ENEMIGO_NINGUNA, 100, 0, 64);

    Personaje_t sinDefensa[1] = { crearDante() };
    turnoEnemigo(&golpeador, sinDefensa, 1, crearEfectoGrupal());
    assert(sinDefensa[0].vida == 320 - 24);  // 64 - 40

    Personaje_t conDefensa[1] = { crearDante() };
    EfectoGrupal_t efecto = crearEfectoGrupal();
    assert(activarHabilidad(&conDefensa[0], conDefensa, 1, &efecto) == 1);
    turnoEnemigo(&golpeador, conDefensa, 1, efecto);
    assert(conDefensa[0].vida == 320 - 4);   // 64 - 60
}

static void testCuracionGrupalDosVeces(void) {
    Personaje_t equipo[3] = { crearFrieren(), crearIsaac(), crearDante() };
    equipo[0].vida = 100;
    equipo[1].vida = 300;
    equipo[2].vida = 0;                      // muerto: no se cura

    EfectoGrupal_t efecto = crearEfectoGrupal();
    assert(activarHabilidad(&equipo[0], equipo, 3, &efecto) == 1);
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
    assert(activarHabilidad(&equipo[0], equipo, 1, &efecto) == 1);
    assert(efecto.tipo == EFECTO_GRUPAL_CRITICO);
    assert(efecto.turnosRestantes == 2);
    assert(activarHabilidad(&equipo[0], equipo, 1, &efecto) == 0);

    cerrarRonda(&efecto);
    assert(efecto.tipo == EFECTO_GRUPAL_CRITICO);  // sigue en la 2a ronda
    assert(activarHabilidad(&equipo[0], equipo, 1, &efecto) == 0);
    cerrarRonda(&efecto);
    assert(efecto.tipo == EFECTO_GRUPAL_INACTIVO);
    assert(efecto.turnosRestantes == 0);
    assert(activarHabilidad(&equipo[0], equipo, 1, &efecto) == 1);
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
// Estadisticas
// ============================================================

static void testEstadisticasCrecenSinPerderDatos(void) {
    Estadisticas_t estadisticas = crearEstadisticas();
    int capacidadInicial = estadisticas.capacidad;
    assert(estadisticas.registros_ptr != NULL);
    assert(capacidadInicial > 0);

    int cantidad = capacidadInicial * 3 + 1;
    for (int i = 0; i < cantidad; i++) {
        assert(agregarRegistroRonda(&estadisticas, (RegistroRonda_t) { 1, 1, i + 1, i * 2, i, 0 }));
    }
    assert(estadisticas.cantidad == cantidad);
    assert(estadisticas.capacidad >= cantidad);
    assert(estadisticas.capacidad > capacidadInicial);
    for (int i = 0; i < cantidad; i++) {
        assert(estadisticas.registros_ptr[i].ronda == i + 1);
        assert(estadisticas.registros_ptr[i].danoCausado == i * 2);
    }

    liberarEstadisticas(&estadisticas);
    assert(estadisticas.registros_ptr == NULL);
    assert(estadisticas.cantidad == 0);
    assert(estadisticas.capacidad == 0);

    // Despues de liberar se puede volver a usar: realloc de NULL reserva.
    assert(agregarRegistroRonda(&estadisticas, (RegistroRonda_t) { 1, 1, 1, 10, 5, 0 }));
    assert(estadisticas.cantidad == 1);
    liberarEstadisticas(&estadisticas);
}

// ============================================================
// Habilidades de enemigos de las zonas 2 a 4
// ============================================================

static Enemigo_t crearSefirotPrueba(void) {
    return crearJefe("Sefirot", HABILIDAD_ENEMIGO_SEFIROT, 700, 40, 96);
}

static Batalla_t batallaCon(const Enemigo_t enemigos_ptr[], int cantEnemigos) {
    Batalla_t batalla;
    for (int i = 0; i < cantEnemigos; i++) {
        batalla.enemigos[i] = enemigos_ptr[i];
    }
    batalla.cantEnemigos = cantEnemigos;
    batalla.cantRefuerzos = 0;
    return batalla;
}

static void testMitadDeDano(void) {
    Personaje_t isaac = crearIsaac();
    Enemigo_t vergil = crearJefe("Vergil", HABILIDAD_ENEMIGO_MITAD_DANO, 340, 30, 76);
    ataqueIndividual(&isaac, &vergil, crearEfectoGrupal());
    assert(vergil.vida == 340 - 25);         // (80 - 30) / 2
}

static void testRegeneracionSinPasarElMaximo(void) {
    Enemigo_t tiny = crearJefe("Tiny", HABILIDAD_ENEMIGO_REGENERACION, 500, 36, 90);
    tiny.vida = 400;
    regenerarEnemigo(&tiny);
    assert(tiny.vida == 430);
    tiny.vida = 490;
    regenerarEnemigo(&tiny);
    assert(tiny.vida == 500);
}

// Aturdir es aleatorio: algunas veces si, otras no. El aturdido dura un turno.
static void testAturdirDuraUnTurno(void) {
    Enemigo_t golem = crearEnemigo("Golem", HABILIDAD_ENEMIGO_ATURDIR, 260, 30, 76);
    int aturdidos = 0;
    for (int i = 0; i < 100; i++) {
        Personaje_t equipo[1] = { crearIsaac() };
        turnoEnemigo(&golem, equipo, 1, crearEfectoGrupal());
        if (equipo[0].estadoActivo.tipo == ESTADO_ATURDIDO) {
            aturdidos++;
            actualizarEstadoPersonaje(&equipo[0]);   // el turno que pierde
            assert(equipo[0].estadoActivo.tipo == ESTADO_NINGUNO);
        }
    }
    assert(aturdidos > 0);
    assert(aturdidos < 100);
}

static void testInvocarRespetaLugares(void) {
    Enemigo_t cazador = crearCazador();
    Enemigo_t sefirot = crearSefirotPrueba();

    Enemigo_t solo[1] = { sefirot };
    Batalla_t batalla = batallaCon(solo, 1);
    assert(!invocarRefuerzo(&batalla));      // sin refuerzos no invoca

    batalla.refuerzos[0] = cazador;
    batalla.cantRefuerzos = 1;
    assert(invocarRefuerzo(&batalla));       // ocupa un lugar libre al final
    assert(batalla.cantEnemigos == 2);
    assert(strcmp(batalla.enemigos[1].nombre, cazador.nombre) == 0);

    assert(invocarRefuerzo(&batalla));
    assert(batalla.cantEnemigos == 3);
    assert(!invocarRefuerzo(&batalla));      // ya hay 3 vivos

    batalla.enemigos[1].vida = 0;            // reemplaza a un caido, sin crecer
    assert(invocarRefuerzo(&batalla));
    assert(batalla.cantEnemigos == 3);
    assert(estaVivoEnemigo(&batalla.enemigos[1]));
    assert(strcmp(batalla.enemigos[0].nombre, "Sefirot") == 0);
}

// Sefirot elige al azar: en muchas rondas tiene que curarse e invocar alguna
// vez, sin pasar nunca su vida maxima ni la cantidad de lugares.
static void testSefirotCuraEInvoca(void) {
    int curaciones = 0;
    int invocaciones = 0;
    for (int i = 0; i < 200; i++) {
        Enemigo_t sefirot = crearSefirotPrueba();
        sefirot.vida = 650;
        Batalla_t batalla = batallaCon(&sefirot, 1);
        batalla.refuerzos[0] = crearCazador();
        batalla.cantRefuerzos = 1;
        Personaje_t equipo[3] = { crearIsaac(), crearDante(), crearFrieren() };

        faseEnemigos(&batalla, equipo, 3, crearEfectoGrupal());
        assert(batalla.enemigos[0].vida <= batalla.enemigos[0].vidaMax);
        assert(batalla.cantEnemigos <= MAX_ENEMIGOS_COMBATE);
        if (batalla.enemigos[0].vida > 650) {
            curaciones++;
        }
        if (batalla.cantEnemigos == 2) {
            invocaciones++;
            assert(batalla.enemigos[1].vida == batalla.enemigos[1].vidaMax);   // no actuo ni recibio dano
        }
    }
    assert(curaciones > 0);
    assert(invocaciones > 0);
}

// ============================================================
// Simulacion (sin teclado)
// Peor caso de la doc: sin habilidad ni items; 1 a 1, y area
// solo con 3 o mas enemigos vivos.
// ============================================================

static int primerEnemigoVivo(const Batalla_t *batalla_ptr) {
    for (int i = 0; i < batalla_ptr->cantEnemigos; i++) {
        if (estaVivoEnemigo(&batalla_ptr->enemigos[i])) {
            return i;
        }
    }
    return -1;
}

// Igual que iniciarCombate, pero el jugador decide solo. Devuelve 1 si gana.
static int combateAutomatico(Personaje_t equipo_ptr[], int cantP, Batalla_t *batalla_ptr) {
    EfectoGrupal_t efecto = crearEfectoGrupal();
    while (1) {
        iniciarRonda(equipo_ptr, cantP, efecto);
        for (int i = 0; i < cantP && quedanEnemigosVivos(batalla_ptr->enemigos, batalla_ptr->cantEnemigos); i++) {
            if (!estaVivoPersonaje(&equipo_ptr[i])) {
                continue;
            }
            if (equipo_ptr[i].estadoActivo.tipo == ESTADO_ATURDIDO) {
                // pierde el turno
            } else if (quedanEnemigosVivos(batalla_ptr->enemigos, batalla_ptr->cantEnemigos) >= 3) {
                ataqueArea(&equipo_ptr[i], batalla_ptr->enemigos, batalla_ptr->cantEnemigos, efecto);
            } else {
                ataqueIndividual(&equipo_ptr[i], &batalla_ptr->enemigos[primerEnemigoVivo(batalla_ptr)], efecto);
            }
            actualizarEstadoPersonaje(&equipo_ptr[i]);
        }
        if (!quedanEnemigosVivos(batalla_ptr->enemigos, batalla_ptr->cantEnemigos)) {
            return 1;
        }
        faseEnemigos(batalla_ptr, equipo_ptr, cantP, efecto);
        if (!quedanPersonajesVivos(equipo_ptr, cantP)) {
            return 0;
        }
        if (!quedanEnemigosVivos(batalla_ptr->enemigos, batalla_ptr->cantEnemigos)) {
            return 1;
        }
        cerrarRonda(&efecto);
    }
}

// Mientras corre la campana, los mensajes del combate van a la "nada" para
// que solo se vea el resumen final.
static int silenciarSalida(void) {
    fflush(stdout);
    int copia = dup(fileno(stdout));
    if (freopen(RUTA_NULA, "w", stdout) == NULL) {
        return -1;
    }
    return copia;
}

static void restaurarSalida(int copia) {
    fflush(stdout);
    if (copia >= 0) {
        dup2(copia, fileno(stdout));
        close(copia);
    }
}

static int vidaPorcentajeEquipo(const Personaje_t equipo_ptr[], int cant) {
    int vida = 0;
    int vidaMax = 0;
    for (int i = 0; i < cant; i++) {
        vida += equipo_ptr[i].vida;
        vidaMax += equipo_ptr[i].vidaMax;
    }
    return vida * 100 / vidaMax;
}

static void recuperarEquipoSimulado(Personaje_t equipo_ptr[], int cant) {
    for (int i = 0; i < cant; i++) {
        equipo_ptr[i].vida = equipo_ptr[i].vidaMax;
        equipo_ptr[i].estadoActivo.tipo = ESTADO_NINGUNO;
    }
}

// Juega las 4 zonas como el juego real (descanso al empezar cada zona,
// fogatas, aliados, mejoras y contrabonus), sin eventos entre fases. Guarda
// la vida del equipo al terminar cada zona y devuelve cuantas zonas supero.
static int simularCampana(int protagonista, TipoMejora estrategia, int vidaAlTerminar_ptr[]) {
    Personaje_t todos[3] = { crearIsaac(), crearDante(), crearFrieren() };
    Personaje_t equipo[3] = {
        todos[protagonista],
        todos[aliadoQueSeUne(protagonista, 1)],
        todos[aliadoQueSeUne(protagonista, 2)]
    };
    int cant = 1;
    ProgresoPartida_t progreso = crearProgresoPartida();

    for (int numero = 1; numero <= CANT_ZONAS; numero++) {
        Zona_t zona = crearZonaPorNumero(numero);
        recuperarEquipoSimulado(equipo, cant);
        for (int f = 0; f < zona.cantFases; f++) {
            if (zona.fases[f].fogataAntes) {
                recuperarEquipoSimulado(equipo, cant);
            }
            Batalla_t batalla = zona.fases[f].batalla;
            aplicarContrabonusEnemigos(batalla.enemigos, batalla.cantEnemigos, &progreso);
            aplicarContrabonusEnemigos(batalla.refuerzos, batalla.cantRefuerzos, &progreso);
            if (!combateAutomatico(equipo, cant, &batalla)) {
                return numero - 1;
            }
        }
        vidaAlTerminar_ptr[numero - 1] = vidaPorcentajeEquipo(equipo, cant);

        if (numero < CANT_ZONAS) {
            int desbloqueados[3] = { cant > 0, cant > 1, cant > 2 };
            aplicarMejoraEquipo(equipo, desbloqueados, 3, estrategia, &progreso);
            if (cant < 3) {
                aplicarMejorasAcumuladas(&equipo[cant], &progreso);
                cant++;
            }
        }
    }
    return CANT_ZONAS;
}

// Corre la campana con varias semillas para cada estrategia de mejora e
// imprime, por zona, cuantas veces se gano (de las que se llego) y la vida
// media del equipo al terminarla.
static void simularBalance(unsigned int semillaBase, int corridas) {
    const char *nombresEstrategia_ptr[] = { "siempre vida", "siempre ataque", "siempre defensa" };
    printf("\n===== Balance: campana completa, peor caso (semillas %u a %u) =====\n",
           semillaBase, semillaBase + (unsigned int) corridas - 1);

    for (int e = 0; e < 3; e++) {
        TipoMejora estrategia = (TipoMejora) (MEJORA_VIDA + e);
        int llegaron[CANT_ZONAS] = { 0 };
        int ganaron[CANT_ZONAS] = { 0 };
        int vidaSumada[CANT_ZONAS] = { 0 };

        for (int c = 0; c < corridas; c++) {
            srand(semillaBase + (unsigned int) c);
            int vidaAlTerminar[CANT_ZONAS];
            int copia = silenciarSalida();
            int superadas = simularCampana(c % 3, estrategia, vidaAlTerminar);
            restaurarSalida(copia);

            for (int z = 0; z < CANT_ZONAS && z <= superadas; z++) {
                llegaron[z]++;
                if (z < superadas) {
                    ganaron[z]++;
                    vidaSumada[z] += vidaAlTerminar[z];
                }
            }
        }

        printf("\nMejora %s:\n", nombresEstrategia_ptr[e]);
        for (int z = 0; z < CANT_ZONAS; z++) {
            printf("  Zona %d: gana %3d de %3d (%3d%%)", z + 1, ganaron[z], llegaron[z],
                   llegaron[z] ? ganaron[z] * 100 / llegaron[z] : 0);
            if (ganaron[z] > 0) {
                printf(" - vida media al terminar %d%%", vidaSumada[z] / ganaron[z]);
            }
            printf("\n");
        }
    }
}

int main(int argc, char *argv_ptr[]) {
    testCalcularDano();
    testIdentificarJefe();
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
    testEstadisticasCrecenSinPerderDatos();
    testMitadDeDano();
    testRegeneracionSinPasarElMaximo();
    testAturdirDuraUnTurno();
    testInvocarRespetaLugares();
    testSefirotCuraEInvoca();
    printf("\n===== Todos los tests pasaron =====\n");

    unsigned int semillaBase = argc > 1 ? (unsigned int) atoi(argv_ptr[1]) : 1;
    simularBalance(semillaBase, 50);
    return 0;
}
