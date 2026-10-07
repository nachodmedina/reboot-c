#include <stdio.h>
#include "../include/personajes.h"
#include "../include/historia.h"
#include "../include/utils.h"
#include "../include/archivos.h"
#include "../include/combate.h"
#include "../include/items.h"

#define CANT_PERSONAJES 3

// ============================================================
// PROTOTIPOS: la "firma" de cada funcion (tipo de retorno, nombre
// y parametros) sin su cuerpo. Le avisan al compilador que estas
// funciones existen y como se usan, antes de llegar a main().
// ============================================================
static ModoJuego seleccionarModoJuego(void);
static int seleccionarPersonajeInicial(const Personaje equipo[], int cantPersonajes);
static void nuevaPartida(void);
static int iniciarZona(const Zona *zona, Personaje luchadores[], int cantLuchadores, ModoJuego modo);
static void descansarEnFogata(Personaje luchadores[], int cantLuchadores);
static void cargarPartidaExistente(void);
static void mostrarMenuPrincipal(void);
static void crearEquipo(Personaje equipo[]);
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

static void crearEquipo(Personaje equipo[]) {
    equipo[0] = crearPersonaje("Isaac", "Espada de Excabilur", HABILIDAD_CRITICO_GRUPAL,  320, 40, 80);
    equipo[1] = crearPersonaje("Dante", "Lanza de Rebellion", HABILIDAD_DEFENSA_GRUPAL, 320, 40, 80);
    equipo[2] = crearPersonaje("Frieren", "Baston de la Perdida", HABILIDAD_CURACION_GRUPAL, 320, 40, 80);
}

// TEMPORAL: combate de prueba con las fases de Zona 1, hasta conectar la zona real.
static void probarCombate(void) {
    Personaje equipo[CANT_PERSONAJES];
    crearEquipo(equipo);

    printf("\n1. Pelear con un solo personaje\n");
    printf("2. Pelear con el equipo completo\n");
    printf("Elija una opcion: ");
    int cantPersonajes = 1;
    Personaje luchadores[CANT_PERSONAJES];
    if (leerOpcionMenu(1, 2) == 1) {
        luchadores[0] = equipo[seleccionarPersonajeInicial(equipo, CANT_PERSONAJES)];
    } else {
        cantPersonajes = CANT_PERSONAJES;
        for (int i = 0; i < CANT_PERSONAJES; i++) {
            luchadores[i] = equipo[i];
        }
    }

    // "zona" es una copia local: se puede pelear directo contra sus enemigos.
    Zona zona = crearZonaCampoCeniza();
    printf("\nElegi la fase de %s:\n", zona.nombre);
    for (int i = 0; i < zona.cantFases; i++) {
        printf("%d. Fase %d:", i + 1, i + 1);
        for (int j = 0; j < zona.fases[i].cantEnemigos; j++) {
            printf("%s %s", j == 0 ? "" : ",", zona.fases[i].enemigos[j].nombre);
        }
        printf("\n");
    }
    printf("Elija una opcion: ");
    Fase *fase = &zona.fases[leerOpcionMenu(1, zona.cantFases) - 1];

    Inventario *inventario = crearInventario(2);
    Item pocion = { ITEM_VIDA, 100, 0 };
    agregarItem(inventario, pocion);
    agregarItem(inventario, pocion);

    iniciarCombate(luchadores, cantPersonajes, fase->enemigos, fase->cantEnemigos, inventario);
    liberarInventario(inventario);
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

// Muestra los personajes disponibles y deja al jugador elegir con cual
// quiere empezar la historia. Devuelve el INDICE (0..cantPersonajes-1)
// dentro del arreglo "equipo", no el numero de opcion que ve el jugador.
static int seleccionarPersonajeInicial(const Personaje equipo[], int cantPersonajes) {
    printf("\nElegi con que personaje queres empezar la historia:\n");
    for (int i = 0; i < cantPersonajes; i++) {
        printf("%d. %s (arma: %s)\n", i + 1, equipo[i].nombre, equipo[i].arma);
    }
    printf("Elija una opcion: ");

    // leerOpcionMenu ya valida que el numero este dentro de rango.
    int opcion = leerOpcionMenu(1, cantPersonajes);

    return opcion - 1; // el jugador ve "1..n", el arreglo usa "0..n-1"
}

// Antes de una pelea dificil: vida al maximo y sin estados alterados.
static void descansarEnFogata(Personaje luchadores[], int cantLuchadores) {
    printf("\nEncontras los restos de una fogata todavia tibia. Descansas un rato\n");
    printf("y recuperas todas tus fuerzas.\n");
    for (int i = 0; i < cantLuchadores; i++) {
        luchadores[i].vida = luchadores[i].vidaMax;
        luchadores[i].estadoActivo.tipo = ESTADO_NINGUNO;
        luchadores[i].estadoActivo.duracionTurnos = 0;
    }
    pausar();
}

// Juega todas las fases de la zona en orden: encuentro, combate y escena
// post batalla. "luchadores" se modifica directamente (vida, estados), asi
// lo que pasa en una fase se arrastra a la siguiente.
// Devuelve 1 si superan la zona, 0 si caen en alguna fase.
static int iniciarZona(const Zona *zona, Personaje luchadores[], int cantLuchadores, ModoJuego modo) {
    mostrarEntradaZona(zona);

    for (int i = 0; i < zona->cantFases; i++) {
        const Fase *fase = &zona->fases[i];

        if (fase->fogataAntes) {
            descansarEnFogata(luchadores, cantLuchadores);
        }

        mostrarEncuentroEnemigos(&luchadores[0], fase->amenaza);

        // iniciarCombate les baja la vida a los enemigos, asi que peleamos
        // contra una copia y la zona queda intacta.
        Enemigo enemigos[MAX_ENEMIGOS_FASE];
        for (int j = 0; j < fase->cantEnemigos; j++) {
            enemigos[j] = fase->enemigos[j];
        }

        if (!iniciarCombate(luchadores, cantLuchadores, enemigos, fase->cantEnemigos, NULL)) {
            printf("\n%s cae derrotado en %s...\n",
                   cantLuchadores == 1 ? luchadores[0].nombre : "El equipo", zona->nombre);
            if (modo == MODO_DIFICIL) {
                eliminarPartida();
                printf("Estabas en Modo Dificil: la partida fue eliminada.\n");
            }
            pausar();
            return 0;
        }

        mostrarPostBatalla(&luchadores[0]);
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

    Personaje equipo[CANT_PERSONAJES];
    crearEquipo(equipo);

    int protagonista = seleccionarPersonajeInicial(equipo, CANT_PERSONAJES);
    mostrarNarrativaInicial(&equipo[protagonista]);
    mostrarSalidaDeRaiz(&equipo[protagonista]);

    // Zona 1 se pelea en solitario: &equipo[protagonista] funciona como un
    // arreglo de 1 personaje, y los cambios del combate quedan en "equipo".
    Zona campoCeniza = crearZonaCampoCeniza();
    if (!iniciarZona(&campoCeniza, &equipo[protagonista], 1, modo)) {
        return; // no se guarda partida ni se sigue mostrando nada mas
    }

    // Siguiente zona (cuando exista), por ejemplo con el equipo completo:
    // Zona zona2 = crearZonaXxx();
    // if (!iniciarZona(&zona2, equipo, CANT_PERSONAJES, modo)) {
    //     return;
    // }

    printf("\nTu personaje:\n");
    mostrarPersonaje(&equipo[protagonista]);

    guardarPartida(equipo, CANT_PERSONAJES);
    printf("\nPartida nueva creada y guardada (modulo de juego en construccion).\n");
    pausar();
}




// TODO: una vez cargado el equipo, retomar la zona/historia donde quedo.
static void cargarPartidaExistente(void) {
    Personaje equipo[CANT_PERSONAJES];
    if (!cargarPartida(equipo, CANT_PERSONAJES)) {
        printf("\nNo hay ninguna partida guardada todavia.\n");
        pausar();
        return;
    }

    printf("\nPartida cargada:\n");
    for (int i = 0; i < CANT_PERSONAJES; i++) {
        mostrarPersonaje(&equipo[i]);
    }
    pausar();
}