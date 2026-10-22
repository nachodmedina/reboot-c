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

EfectoGrupal_t crearEfectoGrupal(void) {
    EfectoGrupal_t efecto;
    efecto.tipo = EFECTO_GRUPAL_INACTIVO;
    efecto.turnosRestantes = 0;
    return efecto;
}

int calcularDano(int ataque, int defensa) {
    int dano = ataque - defensa;
    return dano > 0 ? dano : 0;
}

int calcularDanoArea(int ataque, int defensa) {
    return calcularDano(ataque / 2 + AREA_BONUS, defensa);
}

int ataqueEfectivo(Personaje_t personaje) {
    if (personaje.estadoActivo.tipo == ESTADO_DEBILITADO) {
        return personaje.ataque * (100 - personaje.estadoActivo.magnitud) / 100;
    }
    return personaje.ataque;
}

// Aplica el critico grupal de Isaac si esta activo.
static int aplicarCriticoGrupal(int dano, EfectoGrupal_t efecto) {
    if (efecto.tipo == EFECTO_GRUPAL_CRITICO) {
        return dano * CRITICO_GRUPAL_PORCENTAJE / 100;
    }
    return dano;
}

static void recibirDanoEnemigo(Enemigo_t *objetivo_ptr, int dano) {
    objetivo_ptr->vida -= dano;
    if (objetivo_ptr->vida < 0) {
        objetivo_ptr->vida = 0;
    }
    printf("  %s recibe %d de dano (vida %d/%d)\n",
           objetivo_ptr->nombre, dano, objetivo_ptr->vida, objetivo_ptr->vidaMax);
    if (!estaVivoEnemigo(objetivo_ptr)) {
        printf("  %s fue derrotado.\n", objetivo_ptr->nombre);
    }
}

void ataqueIndividual(Personaje_t atacante, Enemigo_t *objetivo_ptr, EfectoGrupal_t efecto) {
    int dano = calcularDano(ataqueEfectivo(atacante), objetivo_ptr->defensa);
    dano = aplicarCriticoGrupal(dano, efecto);
    printf("%s ataca a %s.\n", atacante.nombre, objetivo_ptr->nombre);
    recibirDanoEnemigo(objetivo_ptr, dano);
}

void ataqueArea(Personaje_t atacante, Enemigo_t enemigos_ptr[], int cantEnemigos, EfectoGrupal_t efecto) {
    printf("%s lanza un ataque en area.\n", atacante.nombre);
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos_ptr[i])) {
            int dano = calcularDanoArea(ataqueEfectivo(atacante), enemigos_ptr[i].defensa);
            dano = aplicarCriticoGrupal(dano, efecto);
            recibirDanoEnemigo(&enemigos_ptr[i], dano);
        }
    }
}

void curarEquipo(Personaje_t equipo_ptr[], int cantPersonajes, int cantidad) {
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo_ptr[i])) {
            equipo_ptr[i].vida += cantidad;
            if (equipo_ptr[i].vida > equipo_ptr[i].vidaMax) {
                equipo_ptr[i].vida = equipo_ptr[i].vidaMax;
            }
            printf("  %s se cura (vida %d/%d)\n", equipo_ptr[i].nombre, equipo_ptr[i].vida, equipo_ptr[i].vidaMax);
        }
    }
}

int activarHabilidad(Personaje_t usuario, Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t *efecto_ptr) {
    if (efecto_ptr->tipo != EFECTO_GRUPAL_INACTIVO) {
        return 0;
    }

    switch (usuario.habilidadEspecial) {
        case HABILIDAD_CRITICO_GRUPAL:
            efecto_ptr->tipo = EFECTO_GRUPAL_CRITICO;
            printf("%s activa Critico grupal: el equipo pega x1.2 por %d turnos.\n",
                   usuario.nombre, DURACION_HABILIDAD);
            break;
        case HABILIDAD_DEFENSA_GRUPAL:
            efecto_ptr->tipo = EFECTO_GRUPAL_DEFENSA;
            printf("%s activa Defensa grupal: el equipo recibe menos dano por %d turnos.\n",
                   usuario.nombre, DURACION_HABILIDAD);
            break;
        case HABILIDAD_CURACION_GRUPAL:
            efecto_ptr->tipo = EFECTO_GRUPAL_CURACION;
            printf("%s activa Curacion grupal.\n", usuario.nombre);
            curarEquipo(equipo_ptr, cantPersonajes, CURACION_GRUPAL);
            break;
    }

    efecto_ptr->turnosRestantes = DURACION_HABILIDAD;
    return 1;
}

void iniciarRonda(Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto) {
    if (efecto.tipo == EFECTO_GRUPAL_CURACION && efecto.turnosRestantes > 0) {
        printf("La curacion grupal sigue actuando.\n");
        curarEquipo(equipo_ptr, cantPersonajes, CURACION_GRUPAL);
    }
}

// Elige al azar un personaje vivo. Devuelve su indice, o -1 si no queda ninguno.
static int elegirObjetivoAleatorio(Personaje_t equipo_ptr[], int cantPersonajes) {
    int vivos = quedanPersonajesVivos(equipo_ptr, cantPersonajes);
    if (vivos == 0) {
        return -1;
    }
    int elegido = numeroAleatorio(1, vivos);
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo_ptr[i])) {
            elegido--;
            if (elegido == 0) {
                return i;
            }
        }
    }
    return -1;
}

void turnoEnemigo(Enemigo_t atacante, Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto) {
    int indice = elegirObjetivoAleatorio(equipo_ptr, cantPersonajes);
    if (indice == -1) {
        return;
    }

    int defensa = equipo_ptr[indice].defensa;
    if (efecto.tipo == EFECTO_GRUPAL_DEFENSA) {
        defensa = defensa * DEFENSA_GRUPAL_PORCENTAJE / 100;
    }
    int dano = calcularDano(atacante.ataque, defensa);

    printf("%s ataca a %s.\n", atacante.nombre, equipo_ptr[indice].nombre);
    if (atacante.habilidad == HABILIDAD_ENEMIGO_CRITICO && numeroAleatorio(1, 100) <= CRITICO_ENEMIGO_CHANCE) {
        dano = dano * CRITICO_ENEMIGO_PORCENTAJE / 100;
        printf("  Golpe critico!\n");
    }

    equipo_ptr[indice].vida -= dano;
    if (equipo_ptr[indice].vida < 0) {
        equipo_ptr[indice].vida = 0;
    }
    printf("  %s recibe %d de dano (vida %d/%d)\n",
           equipo_ptr[indice].nombre, dano, equipo_ptr[indice].vida, equipo_ptr[indice].vidaMax);

    if (!estaVivoPersonaje(&equipo_ptr[indice])) {
        printf("  %s cayo en combate.\n", equipo_ptr[indice].nombre);
    } else if (atacante.habilidad == HABILIDAD_ENEMIGO_TRAMPA && numeroAleatorio(1, 100) <= TRAMPA_CHANCE) {
        Estado_t debilitado = { ESTADO_DEBILITADO, DEBILITADO_DURACION, DEBILITADO_REDUCCION };
        aplicarEstadoPersonaje(&equipo_ptr[indice], debilitado);
        printf("  %s cae en una trampa: queda debilitado.\n", equipo_ptr[indice].nombre);
    }
}

void cerrarRonda(EfectoGrupal_t *efecto_ptr) {
    if (efecto_ptr->turnosRestantes > 0) {
        efecto_ptr->turnosRestantes--;
        if (efecto_ptr->turnosRestantes == 0) {
            efecto_ptr->tipo = EFECTO_GRUPAL_INACTIVO;
            printf("El efecto de la habilidad especial termino.\n");
        }
    }
}

int quedanPersonajesVivos(Personaje_t equipo_ptr[], int cantPersonajes) {
    int vivos = 0;
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo_ptr[i])) {
            vivos++;
        }
    }
    return vivos;
}

int quedanEnemigosVivos(Enemigo_t enemigos_ptr[], int cantEnemigos) {
    int vivos = 0;
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos_ptr[i])) {
            vivos++;
        }
    }
    return vivos;
}

// ============================================================
// Bucle interactivo
// ============================================================

static void mostrarEstadoCombate(Personaje_t equipo_ptr[], int cantPersonajes, Enemigo_t enemigos_ptr[], int cantEnemigos) {
    printf("\n--- Tu equipo ---\n");
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo_ptr[i])) {
            mostrarPersonaje(&equipo_ptr[i]);
        }
    }
    printf("--- Enemigos ---\n");
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos_ptr[i])) {
            mostrarEnemigo(&enemigos_ptr[i]);
        }
    }
}

// Muestra los enemigos numerados y devuelve el indice de uno vivo.
static int elegirEnemigo(Enemigo_t enemigos_ptr[], int cantEnemigos) {
    printf("Elegi a quien atacar:\n");
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos_ptr[i])) {
            printf("%d. %s (vida %d/%d)\n", i + 1, enemigos_ptr[i].nombre, enemigos_ptr[i].vida, enemigos_ptr[i].vidaMax);
        } else {
            printf("%d. %s (derrotado)\n", i + 1, enemigos_ptr[i].nombre);
        }
    }
    printf("Elija una opcion: ");
    while (1) {
        int indice = leerOpcionMenu(1, cantEnemigos) - 1;
        if (estaVivoEnemigo(&enemigos_ptr[indice])) {
            return indice;
        }
        printf("Ese enemigo ya fue derrotado. Elegi otro: ");
    }
}

static const char *nombreTipoItem(TipoItem tipo) {
    switch (tipo) {
        case ITEM_VIDA:    return "Pocion de vida";
        case ITEM_VENENO:  return "Pocion de veneno";
    }
    return "?";
}

// Muestra el inventario y devuelve el indice del item elegido.
static int elegirItem(const Inventario_t *inventario_ptr) {
    printf("Elegi un item:\n");
    for (int i = 0; i < inventario_ptr->cantidad; i++) {
        if (inventario_ptr->items[i].tipo == ITEM_VENENO) {
            printf("%d. %s (%d de dano por turno)\n", i + 1,
                   nombreTipoItem(inventario_ptr->items[i].tipo), inventario_ptr->items[i].valor);
        } else {
            printf("%d. %s (+%d de vida)\n", i + 1,
                   nombreTipoItem(inventario_ptr->items[i].tipo), inventario_ptr->items[i].valor);
        }
    }
    printf("Elija una opcion: ");
    return leerOpcionMenu(1, inventario_ptr->cantidad) - 1;
}

// Turno de un personaje: menu de acciones. Repite el menu si la opcion no esta disponible.
static void turnoPersonaje(Personaje_t equipo_ptr[], int cantPersonajes, int actual,
                           Enemigo_t enemigos_ptr[], int cantEnemigos,
                           Inventario_t *inventario_ptr, EfectoGrupal_t *efecto_ptr) {
    int pocionUsada = 0;

    while (1) {
        printf("\nTurno de %s:\n", equipo_ptr[actual].nombre);
        printf("1. Atacar a un enemigo\n");
        printf("2. Ataque en area\n");
        printf("3. Habilidad especial%s\n",
               efecto_ptr->tipo != EFECTO_GRUPAL_INACTIVO ? " (activa)" : "");
        printf("4. Usar pocion%s\n", pocionUsada ? " (ya usada este turno)" : "");
        printf("Elija una opcion: ");
        
        switch (leerOpcionMenu(1, 4)) {
            case 1: {
                int objetivo = elegirEnemigo(enemigos_ptr, cantEnemigos);
                ataqueIndividual(equipo_ptr[actual], &enemigos_ptr[objetivo], *efecto_ptr);
                return;
            }
            case 2:
                ataqueArea(equipo_ptr[actual], enemigos_ptr, cantEnemigos, *efecto_ptr);
                return;
            case 3:
                if (activarHabilidad(equipo_ptr[actual], equipo_ptr, cantPersonajes, efecto_ptr)) {
                    return;
                }
                printf("La habilidad especial ya esta activa.\n");
                break;
            case 4:
                if (pocionUsada) {
                    printf("Ya usaste una pocion este turno; todavia puedes realizar una accion normal.\n");
                    break;
                }
                if (inventario_ptr == NULL || inventario_ptr->cantidad == 0) {
                    printf("No tenes pociones.\n");
                    break;
                }
                {
                    int indiceItem = elegirItem(inventario_ptr);
                    Enemigo_t *objetivoEnemigo_ptr = NULL;
                    if (inventario_ptr->items[indiceItem].tipo == ITEM_VENENO) {
                        int indiceEnemigo = elegirEnemigo(enemigos_ptr, cantEnemigos);
                        objetivoEnemigo_ptr = &enemigos_ptr[indiceEnemigo];
                    }
                    if (usarItem(inventario_ptr, indiceItem, &equipo_ptr[actual], objetivoEnemigo_ptr)) {
                        pocionUsada = 1;
                        printf("La pocion no consume tu accion: aun podes atacar o usar tu habilidad.\n");
                    } else if (inventario_ptr->items[indiceItem].tipo == ITEM_VIDA) {
                        printf("No podes usar esa pocion ahora; se conserva en la bolsa.\n");
                    } else {
                        printf("Ese enemigo ya tiene otro estado alterado; la pocion se conserva.\n");
                    }
                }
                break;
        }
    }
}

int iniciarCombate(Personaje_t equipo_ptr[], int cantPersonajes, Enemigo_t enemigos_ptr[], int cantEnemigos, Inventario_t *inventario_ptr) {
    EfectoGrupal_t efecto = crearEfectoGrupal();
    int ronda = 1;

    printf("\n===== COMBATE =====\n");
    while (1) {
        printf("\n===== Ronda %d =====\n", ronda);
        iniciarRonda(equipo_ptr, cantPersonajes, efecto);
        mostrarEstadoCombate(equipo_ptr, cantPersonajes, enemigos_ptr, cantEnemigos);

        for (int i = 0; i < cantPersonajes && quedanEnemigosVivos(enemigos_ptr, cantEnemigos); i++) {
            if (!estaVivoPersonaje(&equipo_ptr[i])) {
                continue;
            }
            if (equipo_ptr[i].estadoActivo.tipo == ESTADO_ATURDIDO) {
                printf("\n%s esta aturdido y pierde el turno.\n", equipo_ptr[i].nombre);
            } else {
                turnoPersonaje(equipo_ptr, cantPersonajes, i, enemigos_ptr, cantEnemigos, inventario_ptr, &efecto);
            }
            actualizarEstadoPersonaje(&equipo_ptr[i]);
        }

        if (!quedanEnemigosVivos(enemigos_ptr, cantEnemigos)) {
            printf("\nVictoria! Todos los enemigos fueron derrotados.\n");
            registrarLog("Combate ganado");
            return 1;
        }

        printf("\n");
        for (int j = 0; j < cantEnemigos; j++) {
            if (!estaVivoEnemigo(&enemigos_ptr[j])) {
                continue;
            }
            if (enemigos_ptr[j].estadoActivo.tipo == ESTADO_ATURDIDO) {
                printf("%s esta aturdido y pierde el turno.\n", enemigos_ptr[j].nombre);
            } else {
                turnoEnemigo(enemigos_ptr[j], equipo_ptr, cantPersonajes, efecto);
            }
            actualizarEstadoEnemigo(&enemigos_ptr[j]);
        }

        if (!quedanPersonajesVivos(equipo_ptr, cantPersonajes)) {
            printf("\nDerrota. Tu equipo cayo en combate.\n");
            registrarLog("Combate perdido");
            return 0;
        }

        cerrarRonda(&efecto);
        ronda++;
    }
}
