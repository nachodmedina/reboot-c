#include <stdio.h>
#include "../include/combate.h"
#include "../include/utils.h"
#include "../include/archivos.h"

// Constantes de balance: ajustarlas aca sin tocar la logica.
#define AREA_BONUS                 15
#define DURACION_HABILIDAD          2
#define CRITICO_GRUPAL_PORCENTAJE 120   // Isaac: x1.2
#define DEFENSA_GRUPAL_PORCENTAJE 150   // Dante: +50% de defensa
#define CURACION_GRUPAL            40   // Frieren: HP por curacion
#define DEBILITADO_REDUCCION       25   // % de ataque que se pierde
#define DEBILITADO_DURACION         1
#define TRAMPA_CHANCE              40   // % de probabilidad
#define CRITICO_ENEMIGO_CHANCE     30   // % de probabilidad
#define CRITICO_ENEMIGO_PORCENTAJE 150  // x1.5

// ============================================================
// Piezas del motor (publicas, reutilizables y testeables)
// ============================================================

EfectoGrupal crearEfectoGrupal(void) {
    EfectoGrupal efecto;
    efecto.tipo = HABILIDAD_NINGUNA;
    efecto.turnosRestantes = 0;
    efecto.yaUsada = 0;
    return efecto;
}

int calcularDano(int ataque, int defensa) {
    int dano = ataque - defensa;
    return dano > 0 ? dano : 0;
}

int calcularDanoArea(int ataque, int defensa) {
    return calcularDano(ataque / 2 + AREA_BONUS, defensa);
}

int ataqueEfectivo(Personaje personaje) {
    if (personaje.estadoActivo.tipo == ESTADO_DEBILITADO) {
        return personaje.ataque * (100 - personaje.estadoActivo.magnitud) / 100;
    }
    return personaje.ataque;
}

// Aplica el critico grupal de Isaac si esta activo.
static int aplicarCriticoGrupal(int dano, EfectoGrupal efecto) {
    if (efecto.tipo == HABILIDAD_CRITICO_GRUPAL) {
        return dano * CRITICO_GRUPAL_PORCENTAJE / 100;
    }
    return dano;
}

static void recibirDanoEnemigo(Enemigo *objetivo, int dano) {
    objetivo->vida -= dano;
    if (objetivo->vida < 0) {
        objetivo->vida = 0;
    }
    printf("  %s recibe %d de dano (vida %d/%d)\n",
           objetivo->nombre, dano, objetivo->vida, objetivo->vidaMax);
    if (!estaVivoEnemigo(objetivo)) {
        printf("  %s fue derrotado.\n", objetivo->nombre);
    }
}

void ataqueIndividual(Personaje atacante, Enemigo *objetivo, EfectoGrupal efecto) {
    int dano = calcularDano(ataqueEfectivo(atacante), objetivo->defensa);
    dano = aplicarCriticoGrupal(dano, efecto);
    printf("%s ataca a %s.\n", atacante.nombre, objetivo->nombre);
    recibirDanoEnemigo(objetivo, dano);
}

void ataqueArea(Personaje atacante, Enemigo enemigos[], int cantEnemigos, EfectoGrupal efecto) {
    printf("%s lanza un ataque en area.\n", atacante.nombre);
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos[i])) {
            int dano = calcularDanoArea(ataqueEfectivo(atacante), enemigos[i].defensa);
            dano = aplicarCriticoGrupal(dano, efecto);
            recibirDanoEnemigo(&enemigos[i], dano);
        }
    }
}

void curarEquipo(Personaje equipo[], int cantPersonajes, int cantidad) {
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo[i])) {
            equipo[i].vida += cantidad;
            if (equipo[i].vida > equipo[i].vidaMax) {
                equipo[i].vida = equipo[i].vidaMax;
            }
            printf("  %s se cura (vida %d/%d)\n", equipo[i].nombre, equipo[i].vida, equipo[i].vidaMax);
        }
    }
}

int activarHabilidad(Personaje usuario, Personaje equipo[], int cantPersonajes, EfectoGrupal *efecto) {
    if (efecto->yaUsada || usuario.habilidadEspecial == HABILIDAD_NINGUNA) {
        return 0;
    }
    efecto->tipo = usuario.habilidadEspecial;
    efecto->turnosRestantes = DURACION_HABILIDAD;
    efecto->yaUsada = 1;

    switch (usuario.habilidadEspecial) {
        case HABILIDAD_CRITICO_GRUPAL:
            printf("%s activa Critico grupal: el equipo pega x1.2 por %d turnos.\n",
                   usuario.nombre, DURACION_HABILIDAD);
            break;
        case HABILIDAD_DEFENSA_GRUPAL:
            printf("%s activa Defensa grupal: el equipo recibe menos dano por %d turnos.\n",
                   usuario.nombre, DURACION_HABILIDAD);
            break;
        case HABILIDAD_CURACION_GRUPAL:
            printf("%s activa Curacion grupal.\n", usuario.nombre);
            curarEquipo(equipo, cantPersonajes, CURACION_GRUPAL);
            break;
        case HABILIDAD_NINGUNA:
            break;
    }
    return 1;
}

void iniciarRonda(Personaje equipo[], int cantPersonajes, EfectoGrupal efecto) {
    if (efecto.tipo == HABILIDAD_CURACION_GRUPAL && efecto.turnosRestantes > 0) {
        printf("La curacion grupal sigue actuando.\n");
        curarEquipo(equipo, cantPersonajes, CURACION_GRUPAL);
    }
}

// Elige al azar un personaje vivo. Devuelve su indice, o -1 si no queda ninguno.
static int elegirObjetivoAleatorio(Personaje equipo[], int cantPersonajes) {
    int vivos = quedanPersonajesVivos(equipo, cantPersonajes);
    if (vivos == 0) {
        return -1;
    }
    int elegido = numeroAleatorio(1, vivos);
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo[i])) {
            elegido--;
            if (elegido == 0) {
                return i;
            }
        }
    }
    return -1;
}

void turnoEnemigo(Enemigo atacante, Personaje equipo[], int cantPersonajes, EfectoGrupal efecto) {
    int indice = elegirObjetivoAleatorio(equipo, cantPersonajes);
    if (indice == -1) {
        return;
    }

    int defensa = equipo[indice].defensa;
    if (efecto.tipo == HABILIDAD_DEFENSA_GRUPAL) {
        defensa = defensa * DEFENSA_GRUPAL_PORCENTAJE / 100;
    }
    int dano = calcularDano(atacante.ataque, defensa);

    printf("%s ataca a %s.\n", atacante.nombre, equipo[indice].nombre);
    if (atacante.habilidad == HABILIDAD_ENEMIGO_CRITICO && numeroAleatorio(1, 100) <= CRITICO_ENEMIGO_CHANCE) {
        dano = dano * CRITICO_ENEMIGO_PORCENTAJE / 100;
        printf("  Golpe critico!\n");
    }

    equipo[indice].vida -= dano;
    if (equipo[indice].vida < 0) {
        equipo[indice].vida = 0;
    }
    printf("  %s recibe %d de dano (vida %d/%d)\n",
           equipo[indice].nombre, dano, equipo[indice].vida, equipo[indice].vidaMax);

    if (!estaVivoPersonaje(&equipo[indice])) {
        printf("  %s cayo en combate.\n", equipo[indice].nombre);
    } else if (atacante.habilidad == HABILIDAD_ENEMIGO_TRAMPA && numeroAleatorio(1, 100) <= TRAMPA_CHANCE) {
        Estado debilitado = { ESTADO_DEBILITADO, DEBILITADO_DURACION, DEBILITADO_REDUCCION };
        aplicarEstadoPersonaje(&equipo[indice], debilitado);
        printf("  %s cae en una trampa: queda debilitado.\n", equipo[indice].nombre);
    }
}

void cerrarRonda(EfectoGrupal *efecto) {
    if (efecto->turnosRestantes > 0) {
        efecto->turnosRestantes--;
        if (efecto->turnosRestantes == 0) {
            efecto->tipo = HABILIDAD_NINGUNA;
            printf("El efecto de la habilidad especial termino.\n");
        }
    }
}

int quedanPersonajesVivos(Personaje equipo[], int cantPersonajes) {
    int vivos = 0;
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo[i])) {
            vivos++;
        }
    }
    return vivos;
}

int quedanEnemigosVivos(Enemigo enemigos[], int cantEnemigos) {
    int vivos = 0;
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos[i])) {
            vivos++;
        }
    }
    return vivos;
}

// ============================================================
// Bucle interactivo
// ============================================================

static void mostrarEstadoCombate(Personaje equipo[], int cantPersonajes, Enemigo enemigos[], int cantEnemigos) {
    printf("\n--- Tu equipo ---\n");
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo[i])) {
            mostrarPersonaje(&equipo[i]);
        }
    }
    printf("--- Enemigos ---\n");
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos[i])) {
            mostrarEnemigo(&enemigos[i]);
        }
    }
}

// Muestra los enemigos numerados y devuelve el indice de uno vivo.
static int elegirEnemigo(Enemigo enemigos[], int cantEnemigos) {
    printf("Elegi a quien atacar:\n");
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos[i])) {
            printf("%d. %s (vida %d/%d)\n", i + 1, enemigos[i].nombre, enemigos[i].vida, enemigos[i].vidaMax);
        } else {
            printf("%d. %s (derrotado)\n", i + 1, enemigos[i].nombre);
        }
    }
    printf("Elija una opcion: ");
    while (1) {
        int indice = leerOpcionMenu(1, cantEnemigos) - 1;
        if (estaVivoEnemigo(&enemigos[indice])) {
            return indice;
        }
        printf("Ese enemigo ya fue derrotado. Elegi otro: ");
    }
}

static const char *nombreTipoItem(TipoItem tipo) {
    switch (tipo) {
        case ITEM_VIDA:    return "Vida";
        case ITEM_ATAQUE:  return "Ataque";
        case ITEM_DEFENSA: return "Defensa";
    }
    return "?";
}

// Muestra el inventario y devuelve el indice del item elegido.
static int elegirItem(const Inventario *inventario) {
    printf("Elegi un item:\n");
    for (int i = 0; i < inventario->cantidad; i++) {
        printf("%d. %s +%d\n", i + 1, nombreTipoItem(inventario->items[i].tipo), inventario->items[i].valor);
    }
    printf("Elija una opcion: ");
    return leerOpcionMenu(1, inventario->cantidad) - 1;
}

// Turno de un personaje: menu de acciones. Repite el menu si la opcion no esta disponible.
static void turnoPersonaje(Personaje equipo[], int cantPersonajes, int actual,
                           Enemigo enemigos[], int cantEnemigos,
                           Inventario *inventario, EfectoGrupal *efecto) {
    while (1) {
        printf("\nTurno de %s:\n", equipo[actual].nombre);
        printf("1. Atacar a un enemigo\n");
        printf("2. Ataque en area\n");
        printf("3. Habilidad especial%s\n", efecto->yaUsada ? " (ya usada)" : "");
        printf("4. Usar item\n");
        printf("Elija una opcion: ");

        switch (leerOpcionMenu(1, 4)) {
            case 1: {
                int objetivo = elegirEnemigo(enemigos, cantEnemigos);
                ataqueIndividual(equipo[actual], &enemigos[objetivo], *efecto);
                return;
            }
            case 2:
                ataqueArea(equipo[actual], enemigos, cantEnemigos, *efecto);
                return;
            case 3:
                if (activarHabilidad(equipo[actual], equipo, cantPersonajes, efecto)) {
                    return;
                }
                printf("La habilidad especial ya se uso en este combate.\n");
                break;
            case 4:
                if (inventario != NULL && inventario->cantidad > 0) {
                    usarItem(inventario, elegirItem(inventario), &equipo[actual]);
                    return;
                }
                printf("No tenes items.\n");
                break;
        }
    }
}

int iniciarCombate(Personaje equipo[], int cantPersonajes, Enemigo enemigos[], int cantEnemigos, Inventario *inventario) {
    EfectoGrupal efecto = crearEfectoGrupal();
    int ronda = 1;

    printf("\n===== COMBATE =====\n");
    while (1) {
        printf("\n===== Ronda %d =====\n", ronda);
        iniciarRonda(equipo, cantPersonajes, efecto);
        mostrarEstadoCombate(equipo, cantPersonajes, enemigos, cantEnemigos);

        for (int i = 0; i < cantPersonajes && quedanEnemigosVivos(enemigos, cantEnemigos); i++) {
            if (!estaVivoPersonaje(&equipo[i])) {
                continue;
            }
            if (equipo[i].estadoActivo.tipo == ESTADO_ATURDIDO) {
                printf("\n%s esta aturdido y pierde el turno.\n", equipo[i].nombre);
            } else {
                turnoPersonaje(equipo, cantPersonajes, i, enemigos, cantEnemigos, inventario, &efecto);
            }
            actualizarEstadoPersonaje(&equipo[i]);
        }

        if (!quedanEnemigosVivos(enemigos, cantEnemigos)) {
            printf("\nVictoria! Todos los enemigos fueron derrotados.\n");
            registrarLog("Combate ganado");
            return 1;
        }

        printf("\n");
        for (int j = 0; j < cantEnemigos; j++) {
            if (!estaVivoEnemigo(&enemigos[j])) {
                continue;
            }
            if (enemigos[j].estadoActivo.tipo == ESTADO_ATURDIDO) {
                printf("%s esta aturdido y pierde el turno.\n", enemigos[j].nombre);
            } else {
                turnoEnemigo(enemigos[j], equipo, cantPersonajes, efecto);
            }
            actualizarEstadoEnemigo(&enemigos[j]);
        }

        if (!quedanPersonajesVivos(equipo, cantPersonajes)) {
            printf("\nDerrota. Tu equipo cayo en combate.\n");
            registrarLog("Combate perdido");
            return 0;
        }

        cerrarRonda(&efecto);
        ronda++;
    }
}
