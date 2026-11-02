#include <stdio.h>
#include "../include/juego.h"
#include "../include/archivos.h"
#include "../include/combate.h"
#include "../include/estadisticas.h"
#include "../include/historia.h"
#include "../include/items.h"
#include "../include/personajes.h"
#include "../include/progresion.h"
#include "../include/utils.h"
#include "../include/zonas.h"

#define CANT_PERSONAJES MAX_PERSONAJES_INVENTARIO

// Todo lo que forma una partida en curso.
typedef struct {
    ModoJuego modo;
    Personaje_t equipo[CANT_PERSONAJES];  // en orden de union: [0] es el protagonista
    int cantDesbloqueados;                // los primeros N del equipo ya se unieron
    Inventario_t inventario;
    ProgresoPartida_t progreso;
    Estadisticas_t estadisticas;
} Partida_t;

// ============================================================
// Equipo
// ============================================================

// Los tres personajes en su orden fijo (INDICE_ISAAC, INDICE_DANTE, INDICE_FRIEREN).
static void crearEquipo(Personaje_t equipo_ptr[]) {
    equipo_ptr[INDICE_ISAAC] = crearPersonaje("Isaac", "Dagas de Excabilur", HABILIDAD_CRITICO_GRUPAL, 320, 40, 80);
    equipo_ptr[INDICE_DANTE] = crearPersonaje("Dante", "Espadon de Rebellion", HABILIDAD_DEFENSA_GRUPAL, 320, 40, 80);
    equipo_ptr[INDICE_FRIEREN] = crearPersonaje("Frieren", "Baston de la Perdida", HABILIDAD_CURACION_GRUPAL, 320, 40, 80);
}

// Arma el equipo en el orden en que se une a la historia: el protagonista y
// despues los aliados de las Zonas 1 y 2. Asi los que ya se unieron quedan
// siempre al principio del arreglo y se pasan directo al combate.
static void crearEquipoEnOrden(int protagonista, Personaje_t equipo_ptr[]) {
    Personaje_t todos[CANT_PERSONAJES];
    crearEquipo(todos);
    equipo_ptr[0] = todos[protagonista];
    equipo_ptr[1] = todos[aliadoQueSeUne(protagonista, 1)];
    equipo_ptr[2] = todos[aliadoQueSeUne(protagonista, 2)];
}

// guardarPartida y aplicarMejoraEquipo reciben un 1/0 por personaje.
static void marcarDesbloqueados(const Partida_t *partida_ptr, int desbloqueados_ptr[]) {
    for (int i = 0; i < CANT_PERSONAJES; i++) {
        desbloqueados_ptr[i] = (i < partida_ptr->cantDesbloqueados);
    }
}

static void mostrarEquipo(const Partida_t *partida_ptr) {
    printf("\nTu equipo:\n");
    for (int i = 0; i < partida_ptr->cantDesbloqueados; i++) {
        mostrarPersonaje(&partida_ptr->equipo[i]);
    }
    mostrarInventario(&partida_ptr->inventario);
}

// ============================================================
// Menus de eleccion
// ============================================================

static ModoJuego seleccionarModoJuego(void) {
    printf("\nElegi el modo de juego:\n");
    const char *opcionesModo_ptr[] = {
        "Normal (podes guardar tu progreso)",
        "Dificil (si el equipo cae en combate, se borra la partida)"
    };
    return (leerOpcionMenu(opcionesModo_ptr, 2) == 0) ? MODO_NORMAL : MODO_DIFICIL;
}

// Devuelve el indice del personaje elegido (INDICE_ISAAC..INDICE_FRIEREN).
static int seleccionarPersonajeInicial(void) {
    Personaje_t todos[CANT_PERSONAJES];
    crearEquipo(todos);

    printf("\nElegi con que personaje queres empezar la historia:\n");
    char opcionesPersonajes[CANT_PERSONAJES][100];
    const char *opcionesPersonajes_ptr[CANT_PERSONAJES];
    for (int i = 0; i < CANT_PERSONAJES; i++) {
        snprintf(opcionesPersonajes[i], sizeof(opcionesPersonajes[i]), "%s (arma: %s)",
                 todos[i].nombre, todos[i].arma);
        opcionesPersonajes_ptr[i] = opcionesPersonajes[i];
    }
    return leerOpcionMenu(opcionesPersonajes_ptr, CANT_PERSONAJES);
}

static TipoMejora seleccionarMejora(void) {
    printf("\nElegí una mejora para tu equipo:\n");
    char opcionesMejora[3][120];
    const char *opcionesMejora_ptr[3];
    snprintf(opcionesMejora[0], sizeof(opcionesMejora[0]),
             "Vida maxima +%d y vida actual +%d si sigue vivo (enemigos: +%d ataque)",
             aumentoMejora(MEJORA_VIDA), aumentoMejora(MEJORA_VIDA),
             aumentoContrabonus(MEJORA_VIDA));
    snprintf(opcionesMejora[1], sizeof(opcionesMejora[1]),
             "Ataque +%d (los enemigos ganan +%d defensa)",
             aumentoMejora(MEJORA_ATAQUE), aumentoContrabonus(MEJORA_ATAQUE));
    snprintf(opcionesMejora[2], sizeof(opcionesMejora[2]),
             "Defensa +%d (los enemigos ganan +%d vida)",
             aumentoMejora(MEJORA_DEFENSA), aumentoContrabonus(MEJORA_DEFENSA));
    for (int i = 0; i < 3; i++) {
        opcionesMejora_ptr[i] = opcionesMejora[i];
    }
    return (TipoMejora) (leerOpcionMenu(opcionesMejora_ptr, 3) + MEJORA_VIDA);
}

// ============================================================
// Zonas
// ============================================================

// Vida al maximo y sin estados alterados (los caidos se levantan).
static void recuperarEquipo(Personaje_t luchadores_ptr[], int cantLuchadores) {
    for (int i = 0; i < cantLuchadores; i++) {
        luchadores_ptr[i].vida = luchadores_ptr[i].vidaMax;
        luchadores_ptr[i].estadoActivo.tipo = ESTADO_NINGUNO;
        luchadores_ptr[i].estadoActivo.duracionTurnos = 0;
        luchadores_ptr[i].estadoActivo.magnitud = 0;
    }
}

// Antes de una pelea dificil.
static void descansarEnFogata(Personaje_t luchadores_ptr[], int cantLuchadores) {
    mostrarTextoEnCaja(NULL,
        "Encontras los restos de una fogata todavia tibia. Descansas un rato y "
        "recuperas todas tus fuerzas.",
        COLOR_BLANCO);
    recuperarEquipo(luchadores_ptr, cantLuchadores);
    pausar();
}

// Despues de ganar un combate queda al menos un luchador en pie.
static Personaje_t *elegirLuchadorVivo(Partida_t *partida_ptr) {
    int vivos[CANT_PERSONAJES];
    int cantVivos = 0;
    for (int i = 0; i < partida_ptr->cantDesbloqueados; i++) {
        if (estaVivoPersonaje(&partida_ptr->equipo[i])) {
            vivos[cantVivos++] = i;
        }
    }
    return &partida_ptr->equipo[vivos[numeroAleatorio(0, cantVivos - 1)]];
}

// Juega todas las fases de la zona en orden: encuentro, combate y escena
// post batalla. Pelean los primeros cantDesbloqueados del equipo, y lo que
// les pasa en una fase (vida, estados) se arrastra a la siguiente.
// Devuelve 1 si superan la zona, 0 si caen en alguna fase.
static int iniciarZona(const Zona_t *zona_ptr, Partida_t *partida_ptr) {
    Personaje_t *luchadores_ptr = partida_ptr->equipo;
    int cantLuchadores = partida_ptr->cantDesbloqueados;

    // Entre una zona y la siguiente el equipo descansa.
    recuperarEquipo(luchadores_ptr, cantLuchadores);
    mostrarEntradaZona(zona_ptr);
    for (int i = 0; i < zona_ptr->cantFases; i++) {
        const Fase_t *fase_ptr = &zona_ptr->fases[i];

        if (fase_ptr->fogataAntes) {
            descansarEnFogata(luchadores_ptr, cantLuchadores);
            mostrarDescansoZona(&luchadores_ptr[0], zona_ptr->numero);
        }
        mostrarEncuentroEnemigos(&luchadores_ptr[0], zona_ptr->recorrido, fase_ptr->amenaza);

        // iniciarCombate les baja la vida a los enemigos, asi que peleamos
        // contra una copia y la zona queda intacta.
        Batalla_t batalla = fase_ptr->batalla;
        if (!aplicarContrabonusEnemigos(batalla.enemigos, batalla.cantEnemigos, &partida_ptr->progreso) ||
            !aplicarContrabonusEnemigos(batalla.refuerzos, batalla.cantRefuerzos, &partida_ptr->progreso)) {
            printf("No se pudieron aplicar las mejoras de los enemigos.\n");
            pausar();
            return 0;
        }

        if (!iniciarCombate(luchadores_ptr, cantLuchadores, &batalla, &partida_ptr->inventario,
                            &partida_ptr->estadisticas, zona_ptr->numero, i + 1)) {
            printf("\n%s cae derrotado en %s...\n",
                   cantLuchadores == 1 ? luchadores_ptr[0].nombre : "El equipo", zona_ptr->nombre);
            if (partida_ptr->modo == MODO_DIFICIL) {
                eliminarPartida();
                printf("Estabas en Modo Dificil: la partida fue eliminada.\n");
            }
            pausar();
            return 0;
        }

        if (i + 1 < zona_ptr->cantFases && !zona_ptr->fases[i + 1].fogataAntes) {
            mostrarPostBatalla(&luchadores_ptr[0]);
            eventoEntreFases(elegirLuchadorVivo(partida_ptr), &partida_ptr->inventario);
        }
    }
    return 1;
}

// El siguiente personaje del equipo se suma: recibe las mejoras que el resto
// ya tenia y la bolsa compartida gana lugar.
static int unirAliado(Partida_t *partida_ptr, int numeroZona) {
    Personaje_t *aliado_ptr = &partida_ptr->equipo[partida_ptr->cantDesbloqueados];
    if (!aplicarMejorasAcumuladas(aliado_ptr, &partida_ptr->progreso) ||
        !actualizarCapacidadInventario(&partida_ptr->inventario, partida_ptr->cantDesbloqueados + 1)) {
        printf("No se pudo sumar al nuevo aliado.\n");
        pausar();
        return 0;
    }
    partida_ptr->cantDesbloqueados++;
    mostrarLiberacionAliado(&partida_ptr->equipo[0], aliado_ptr, numeroZona);
    return 1;
}

// Despues de vencer al jefe de una zona: el equipo elige una mejora (y los
// enemigos ganan su contrabonus) y, si todavia falta alguien, se une un aliado.
static int recompensarJefe(Partida_t *partida_ptr, int numeroZona) {
    int desbloqueados[CANT_PERSONAJES];
    marcarDesbloqueados(partida_ptr, desbloqueados);

    mostrarHallazgoMejora();
    TipoMejora mejora = seleccionarMejora();
    if (!aplicarMejoraEquipo(partida_ptr->equipo, desbloqueados, CANT_PERSONAJES,
                             mejora, &partida_ptr->progreso)) {
        printf("No se pudo aplicar la mejora elegida.\n");
        pausar();
        return 0;
    }
    printf("\nTu %s aumenta en %d puntos.\n", nombreMejora(mejora), aumentoMejora(mejora));
    mostrarReaccionSefirot(nombreContrabonus(mejora),
                           contrabonusAcumulado(mejora, &partida_ptr->progreso));

    if (partida_ptr->cantDesbloqueados < CANT_PERSONAJES) {
        return unirAliado(partida_ptr, numeroZona);
    }
    return 1;
}

static int guardarProgreso(const Partida_t *partida_ptr) {
    int desbloqueados[CANT_PERSONAJES];
    marcarDesbloqueados(partida_ptr, desbloqueados);
    if (!guardarPartida(ARCHIVO_PARTIDA, partida_ptr->equipo, CANT_PERSONAJES,
                        desbloqueados, &partida_ptr->inventario, &partida_ptr->progreso)) {
        return 0;
    }
    printf("\nProgreso guardado.\n");
    return 1;
}

static void terminarJuego(const Partida_t *partida_ptr) {
    mostrarCaidaSefirot(&partida_ptr->equipo[0]);
    mostrarEpilogo(&partida_ptr->equipo[0]);
    mostrarFinDelJuego();
    mostrarEquipo(partida_ptr);
    registrarLog("Juego completado");
    pausar();
}

// Recorre todas las zonas hasta el final o la derrota.
static void jugarZonas(Partida_t *partida_ptr) {
    for (int numero = 1; numero <= CANT_ZONAS; numero++) {
        Zona_t zona = crearZonaPorNumero(numero);
        if (!iniciarZona(&zona, partida_ptr)) {
            return;
        }
        if (numero == CANT_ZONAS) {
            break;   // despues de Sefirot no hay mejora: sigue el final
        }
        if (!recompensarJefe(partida_ptr, numero)) {
            return;
        }
        mostrarEquipo(partida_ptr);
        if (!guardarProgreso(partida_ptr)) {
            pausar();
            return;
        }
        pausar();
    }
    terminarJuego(partida_ptr);
}

// Fin de la partida, se gane o se pierda: muestra y registra las estadisticas
// y libera su memoria. Es el unico lugar donde se liberan.
static void cerrarPartida(Partida_t *partida_ptr) {
    mostrarEstadisticas(&partida_ptr->estadisticas);
    if (!guardarEstadisticas(&partida_ptr->estadisticas)) {
        printf("No se pudieron guardar las estadisticas.\n");
    }
    liberarEstadisticas(&partida_ptr->estadisticas);
    pausar();
}

// ============================================================
// Funciones publicas
// ============================================================

void jugarPartidaNueva(void) {
    Partida_t partida;
    partida.modo = seleccionarModoJuego();
    if (partida.modo == MODO_NORMAL) {
        printf("\nModo Normal activado: vas a poder guardar tu progreso.\n");
    } else {
        printf("\nModo Dificil activado: si tu equipo cae en combate, la partida se borra.\n");
    }
    mostrarIntroduccionMundo();

    crearEquipoEnOrden(seleccionarPersonajeInicial(), partida.equipo);
    partida.cantDesbloqueados = 1;
    partida.inventario = crearInventario(1);
    partida.progreso = crearProgresoPartida();
    partida.estadisticas = crearEstadisticas();
    mostrarNarrativaInicial(&partida.equipo[0]);
    mostrarSalidaDeRaiz(&partida.equipo[0]);

    jugarZonas(&partida);
    cerrarPartida(&partida);
}

// Por ahora solo muestra lo guardado; retomar la historia se suma con el
// guardado por ranuras.
void mostrarPartidaGuardada(void) {
    Personaje_t equipo[CANT_PERSONAJES];
    int desbloqueados[CANT_PERSONAJES];
    Inventario_t inventario = crearInventario(0);
    ProgresoPartida_t progreso = crearProgresoPartida();
    if (!cargarPartida(ARCHIVO_PARTIDA, equipo, CANT_PERSONAJES,
                       desbloqueados, &inventario, &progreso)) {
        printf("\nNo se pudo cargar la partida (no existe o el archivo es incompatible).\n");
        pausar();
        return;
    }

    printf("\nPartida cargada:\n");
    for (int i = 0; i < CANT_PERSONAJES; i++) {
        if (desbloqueados[i]) {
            mostrarPersonaje(&equipo[i]);
        }
    }
    mostrarInventario(&inventario);
    printf("Mejoras acumuladas: vida +%d, ataque +%d, defensa +%d.\n",
           progreso.personajes.vidaMaxima, progreso.personajes.ataque,
           progreso.personajes.defensa);
    pausar();
}
