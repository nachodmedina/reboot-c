#include <stdio.h>
#include "../include/personajes.h"
#include "../include/historia.h"
#include "../include/utils.h"
#include "../include/archivos.h"
#include "../include/combate.h"
#include "../include/items.h"
#include "../include/progresion.h"

#define CANT_PERSONAJES MAX_PERSONAJES_INVENTARIO

// ============================================================
// PROTOTIPOS: la "firma" de cada funcion (tipo de retorno, nombre
// y parametros) sin su cuerpo. Le avisan al compilador que estas
// funciones existen y como se usan, antes de llegar a main().
// ============================================================
static ModoJuego seleccionarModoJuego(void);
static TipoMejora seleccionarMejora(void);
static int seleccionarPersonajeInicial(const Personaje_t equipo_ptr[], int cantPersonajes);
static void nuevaPartida(void);
static int iniciarZona(const Zona_t *zona_ptr, Personaje_t luchadores_ptr[], int cantLuchadores,
                       ModoJuego modo, const ProgresoPartida_t *progreso_ptr,
                       Inventario_t *inventario_ptr);
static void descansarEnFogata(Personaje_t luchadores_ptr[], int cantLuchadores);
static void cargarPartidaExistente(void);
static void mostrarMenuPrincipal(void);
static void crearEquipo(Personaje_t equipo_ptr[]);
static void probarCombate(void);

// ============================================================
// FUNCION PRINCIPAL: al leer el archivo de arriba hacia abajo,
// esta es la primera funcion completa que aparece, asi que sirve
// como punto de entrada y como "indice" del programa.
// ============================================================
int main(void) {
    inicializarRandom();

    int opcion;
    do {
        mostrarMenuPrincipal();
        opcion = leerOpcionMenu(1, 4);

        switch (opcion) {
            case 1:
                nuevaPartida();
                break;
            case 2:
                cargarPartidaExistente();
                break;
            case 3:
                printf("\nHasta la proxima.\n");
                break;
            case 4:
                probarCombate();
                break;
        }
    } while (opcion != 3);

    return 0;
}

// ============================================================
// DEFINICIONES: el cuerpo completo de cada funcion declarada
// arriba en los prototipos.
// ============================================================

static void mostrarMenuPrincipal(void) {
    printf("\n===== Reboot C =====\n");
    printf("1. Nueva partida\n");
    printf("2. Cargar partida\n");
    printf("3. Salir\n");
    printf("4. Probar combate (temporal)\n");
    printf("Elija una opcion: ");
}

static void crearEquipo(Personaje_t equipo_ptr[]) {
    equipo_ptr[0] = crearPersonaje("Isaac", "Espada de Excabilur", HABILIDAD_CRITICO_GRUPAL,  320, 40, 80);
    equipo_ptr[1] = crearPersonaje("Dante", "Lanza de Rebellion", HABILIDAD_DEFENSA_GRUPAL, 320, 40, 80);
    equipo_ptr[2] = crearPersonaje("Frieren", "Baston de la Perdida", HABILIDAD_CURACION_GRUPAL, 320, 40, 80);
}

// TEMPORAL: combate de prueba con las fases de Zona 1, hasta conectar la zona real.
static void probarCombate(void) {
    Personaje_t equipo[CANT_PERSONAJES];
    crearEquipo(equipo);

    printf("\n1. Pelear con un solo personaje\n");
    printf("2. Pelear con el equipo completo\n");
    printf("Elija una opcion: ");
    int cantPersonajes = 1;
    Personaje_t luchadores[CANT_PERSONAJES];
    if (leerOpcionMenu(1, 2) == 1) {
        luchadores[0] = equipo[seleccionarPersonajeInicial(equipo, CANT_PERSONAJES)];
    } else {
        cantPersonajes = CANT_PERSONAJES;
        for (int i = 0; i < CANT_PERSONAJES; i++) {
            luchadores[i] = equipo[i];
        }
    }

    // "zona" es una copia local: se puede pelear directo contra sus enemigos.
    Zona_t zona = crearZonaCampoCeniza();
    printf("\nElegi la fase de %s:\n", zona.nombre);
    for (int i = 0; i < zona.cantFases; i++) {
        printf("%d. Fase %d:", i + 1, i + 1);
        for (int j = 0; j < zona.fases[i].cantEnemigos; j++) {
            printf("%s %s", j == 0 ? "" : ",", zona.fases[i].enemigos[j].nombre);
        }
        printf("\n");
    }
    printf("Elija una opcion: ");
    Fase_t *fase_ptr = &zona.fases[leerOpcionMenu(1, zona.cantFases) - 1];

    Inventario_t inventario = crearInventario(cantPersonajes);
    Item_t pocionVida = { ITEM_VIDA, 100 };
    Item_t pocionVeneno = { ITEM_VENENO, 5 };
    agregarItem(&inventario, pocionVida);
    agregarItem(&inventario, pocionVeneno);

    iniciarCombate(luchadores, cantPersonajes, fase_ptr->enemigos, fase_ptr->cantEnemigos, &inventario);
    pausar();
}

// Muestra el menu de dificultad y devuelve el modo que eligio el jugador.
// El tipo de retorno es ModoJuego (el enum que definimos en archivos.h),
// no un int suelto: asi el resto del programa sabe con certeza que este
// valor solo puede ser MODO_NORMAL o MODO_DIFICIL, nada mas.
static ModoJuego seleccionarModoJuego(void) {
    printf("\nElegi el modo de juego:\n");
    printf("1. Normal (podes guardar tu progreso)\n");
    printf("2. Dificil (si el equipo cae en combate, se borra la partida)\n");
    printf("Elija una opcion: ");

    int opcion = leerOpcionMenu(1, 2);

    // opcion 1 (lo que ve el jugador) -> MODO_NORMAL
    // opcion 2 (lo que ve el jugador) -> MODO_DIFICIL
    // Usamos el operador ternario (condicion ? valorSiVerdadero : valorSiFalso)
    // porque es una traduccion directa de un solo valor, sin necesitar un if
    // completo con llaves.
    return (opcion == 1) ? MODO_NORMAL : MODO_DIFICIL;
}

static TipoMejora seleccionarMejora(void) {
    printf("\nElegí una mejora para tu equipo:\n");
    printf("1. Vida maxima +%d y vida actual +%d si sigue vivo (enemigos: +%d ataque)\n",
           aumentoMejora(MEJORA_VIDA),
           aumentoMejora(MEJORA_VIDA), aumentoContrabonus(MEJORA_VIDA));
    printf("2. Ataque +%d (los enemigos ganan +%d defensa)\n",
           aumentoMejora(MEJORA_ATAQUE), aumentoContrabonus(MEJORA_ATAQUE));
    printf("3. Defensa +%d (los enemigos ganan +%d vida)\n",
           aumentoMejora(MEJORA_DEFENSA), aumentoContrabonus(MEJORA_DEFENSA));
    printf("Elija una opcion: ");
    return (TipoMejora) leerOpcionMenu(MEJORA_VIDA, MEJORA_DEFENSA);
}

// Muestra los personajes disponibles y deja al jugador elegir con cual
// quiere empezar la historia. Devuelve el INDICE (0..cantPersonajes-1)
// dentro del arreglo "equipo", no el numero de opcion que ve el jugador.
static int seleccionarPersonajeInicial(const Personaje_t equipo_ptr[], int cantPersonajes) {
    printf("\nElegi con que personaje queres empezar la historia:\n");
    for (int i = 0; i < cantPersonajes; i++) {
        printf("%d. %s (arma: %s)\n", i + 1, equipo_ptr[i].nombre, equipo_ptr[i].arma);
    }
    printf("Elija una opcion: ");

    // leerOpcionMenu ya valida que el numero este dentro de rango.
    int opcion = leerOpcionMenu(1, cantPersonajes);

    return opcion - 1; // el jugador ve "1..n", el arreglo usa "0..n-1"
}

// Antes de una pelea dificil: vida al maximo y sin estados alterados.
static void descansarEnFogata(Personaje_t luchadores_ptr[], int cantLuchadores) {
    printf("\nEncontras los restos de una fogata todavia tibia. Descansas un rato\n");
    printf("y recuperas todas tus fuerzas.\n");
    for (int i = 0; i < cantLuchadores; i++) {
        luchadores_ptr[i].vida = luchadores_ptr[i].vidaMax;
        luchadores_ptr[i].estadoActivo.tipo = ESTADO_NINGUNO;
        luchadores_ptr[i].estadoActivo.duracionTurnos = 0;
    }
    pausar();
}

// Juega todas las fases de la zona en orden: encuentro, combate y escena
// post batalla. "luchadores" se modifica directamente (vida, estados), asi
// lo que pasa en una fase se arrastra a la siguiente.
// Devuelve 1 si superan la zona, 0 si caen en alguna fase.
static int iniciarZona(const Zona_t *zona_ptr, Personaje_t luchadores_ptr[], int cantLuchadores,
                       ModoJuego modo, const ProgresoPartida_t *progreso_ptr,
                       Inventario_t *inventario_ptr) {
    mostrarEntradaZona(zona_ptr);

    for (int i = 0; i < zona_ptr->cantFases; i++) {
        const Fase_t *fase_ptr = &zona_ptr->fases[i];

        if (fase_ptr->fogataAntes) {
            descansarEnFogata(luchadores_ptr, cantLuchadores);
        }

        mostrarEncuentroEnemigos(&luchadores_ptr[0], fase_ptr->amenaza);

        // iniciarCombate les baja la vida a los enemigos, asi que peleamos
        // contra una copia y la zona queda intacta.
        Enemigo_t enemigos[MAX_ENEMIGOS_FASE];
        for (int j = 0; j < fase_ptr->cantEnemigos; j++) {
            enemigos[j] = fase_ptr->enemigos[j];
        }
        if (!aplicarContrabonusEnemigos(enemigos, fase_ptr->cantEnemigos, progreso_ptr)) {
            printf("No se pudieron aplicar las mejoras de los enemigos.\n");
            pausar();
            return 0;
        }

        if (!iniciarCombate(luchadores_ptr, cantLuchadores, enemigos, fase_ptr->cantEnemigos, inventario_ptr)) {
            printf("\n%s cae derrotado en %s...\n",
                   cantLuchadores == 1 ? luchadores_ptr[0].nombre : "El equipo", zona_ptr->nombre);
            if (modo == MODO_DIFICIL) {
                eliminarPartida();
                printf("Estabas en Modo Dificil: la partida fue eliminada.\n");
            }
            pausar();
            return 0;
        }

        mostrarPostBatalla(&luchadores_ptr[0]);
        if (i + 1 < zona_ptr->cantFases && !zona_ptr->fases[i + 1].fogataAntes) {
            eventoEntreFases(&luchadores_ptr[0], inventario_ptr);
        }
    }
    return 1;
}



static void nuevaPartida(void) {
    ModoJuego modo = seleccionarModoJuego();

    if (modo == MODO_NORMAL) {
        printf("\nModo Normal activado: vas a poder guardar tu progreso.\n");
    } else {
        printf("\nModo Dificil activado: si tu equipo cae en combate, la partida se borra.\n");
    }
    mostrarIntroduccionMundo();

    Personaje_t equipo[CANT_PERSONAJES];
    crearEquipo(equipo);

    int protagonista = seleccionarPersonajeInicial(equipo, CANT_PERSONAJES);
    mostrarNarrativaInicial(&equipo[protagonista]);
    mostrarSalidaDeRaiz(&equipo[protagonista]);

    ProgresoPartida_t progreso = crearProgresoPartida();
    int desbloqueados[CANT_PERSONAJES] = { 0, 0, 0 };
    desbloqueados[protagonista] = 1;
    Inventario_t inventario = crearInventario(1);

    // Zona 1 se pelea en solitario: &equipo[protagonista] funciona como un
    // arreglo de 1 personaje, y los cambios del combate quedan en "equipo".
    Zona_t campoCeniza = crearZonaCampoCeniza();
    if (!iniciarZona(&campoCeniza, &equipo[protagonista], 1, modo, &progreso, &inventario)) {
        return; // no se guarda partida ni se sigue mostrando nada mas
    }

    TipoMejora mejora = seleccionarMejora();
    if (!aplicarMejoraEquipo(equipo, desbloqueados, CANT_PERSONAJES, mejora, &progreso)) {
        printf("No se pudo aplicar la mejora elegida.\n");
        pausar();
        return;
    }
    printf("\nTu %s aumenta en %d puntos.\n", nombreMejora(mejora), aumentoMejora(mejora));
    mostrarReaccionSephirot(nombreContrabonus(mejora),
                            contrabonusAcumulado(mejora, &progreso));

    int aliado = aliadoQueSeUne(protagonista, 1);
    if (aliado < 0 || desbloqueados[aliado]) {
        printf("No se pudo determinar el aliado que debe unirse.\n");
        pausar();
        return;
    }
    if (!aplicarMejorasAcumuladas(&equipo[aliado], &progreso)) {
        printf("No se pudieron aplicar las mejoras al nuevo aliado.\n");
        pausar();
        return;
    }
    desbloqueados[aliado] = 1;
    if (!actualizarCapacidadInventario(&inventario, 2)) {
        printf("No se pudo ampliar la capacidad del inventario.\n");
        pausar();
        return;
    }
    mostrarLiberacionAliado(&equipo[protagonista], &equipo[aliado]);

    printf("\nTu equipo:\n");
    for (int i = 0; i < CANT_PERSONAJES; i++) {
        if (desbloqueados[i]) {
            mostrarPersonaje(&equipo[i]);
        }
    }
    mostrarInventario(&inventario);

    if (!guardarPartida(ARCHIVO_PARTIDA, equipo, CANT_PERSONAJES,
                        desbloqueados, &inventario, &progreso)) {
        pausar();
        return;
    }
    printf("\nProgreso guardado. La siguiente zona se incorporara mas adelante.\n");
    pausar();
}




// TODO: una vez cargado el equipo, retomar la zona/historia donde quedo.
static void cargarPartidaExistente(void) {
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