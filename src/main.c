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

    Enemigo centinela = crearEnemigo("Centinela Canino", HABILIDAD_ENEMIGO_NINGUNA, 120, 8, 48);
    Enemigo cazador = crearEnemigo("Cazador de Campo Ceniza", HABILIDAD_ENEMIGO_TRAMPA, 160, 16, 56);
    Enemigo spectre = crearEnemigo("Spectre", HABILIDAD_ENEMIGO_CRITICO, 280, 40, 60);

    printf("\nElegi la fase de Zona 1:\n");
    printf("1. Fase 1: 2 Centinela Canino\n");
    printf("2. Fase 2: 1 Cazador de Campo Ceniza\n");
    printf("3. Fase 3: 1 Cazador + 2 Centinela\n");
    printf("4. Fase 4 (jefe): Cazador + Spectre\n");
    printf("Elija una opcion: ");

    Enemigo enemigos[3];
    int cantEnemigos = 0;
    switch (leerOpcionMenu(1, 4)) {
        case 1:
            enemigos[0] = centinela;
            enemigos[1] = centinela;
            cantEnemigos = 2;
            break;
        case 2:
            enemigos[0] = cazador;
            cantEnemigos = 1;
            break;
        case 3:
            enemigos[0] = cazador;
            enemigos[1] = centinela;
            enemigos[2] = centinela;
            cantEnemigos = 3;
            break;
        case 4:
            enemigos[0] = cazador;
            enemigos[1] = spectre;
            cantEnemigos = 2;
            break;
    }

    Inventario *inventario = crearInventario(2);
    Item pocion = { ITEM_VIDA, 100, 0 };
    agregarItem(inventario, pocion);
    agregarItem(inventario, pocion);

    iniciarCombate(luchadores, cantPersonajes, enemigos, cantEnemigos, inventario);
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

// TODO: reemplazar por el flujo real (eleccion de zona, combates, guardado automatico, etc.).
static void nuevaPartida(void) {
    ModoJuego modo = seleccionarModoJuego();

    if (modo == MODO_NORMAL) {
        printf("\nModo Normal activado: vas a poder guardar tu progreso.\n");
    } else {
        printf("\nModo Dificil activado: si tu equipo cae en combate, la partida se borra.\n");
    }
    // NOTA: por ahora "modo" solo se usa para este mensaje. Cuando programemos
    // el bucle de combate de verdad, vamos a tener que hacerle llegar este
    // valor (por ejemplo, como parametro de iniciarCombate) para poder llamar
    // a eliminarPartida() si el equipo pierde y estamos en MODO_DIFICIL.
    mostrarIntroduccionMundo();

    
    Personaje equipo[CANT_PERSONAJES];
    crearEquipo(equipo);

    int protagonista = seleccionarPersonajeInicial(equipo, CANT_PERSONAJES);
    mostrarNarrativaInicial(&equipo[protagonista]);
    mostrarSalidaDeRaiz(&equipo[protagonista]);
   

    Zona campoCeniza = crearZona(
        "Campo Ceniza",
        "Lo que fue un pueblo agricola es hoy tierra quemada, vigilada por\n"
        "cazadores al servicio de las maquinas y sus perros mecanicos.",
        1
    );
    mostrarEntradaZona(&campoCeniza);

    
    
    printf("Tu personaje:\n");
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