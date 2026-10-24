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

static void imprimirNombreCombate(const char *color_ptr, const char *nombre_ptr) {
    printf("%s%s%s%s", estiloTerminal(color_ptr), estiloTerminal(ESTILO_NEGRITA),
           nombre_ptr, estiloTerminal(color_ptr));
}

static void imprimirAccionCombate(const char *color_ptr, const char *actor_ptr,
                                  const char *accion_ptr, const char *objetivo_ptr) {
    imprimirNombreCombate(color_ptr, actor_ptr);
    printf(" %s ", accion_ptr);
    imprimirNombreCombate(color_ptr, objetivo_ptr);
    printf(".%s\n", estiloTerminal(COLOR_RESET));
}

static void imprimirMensajeCombate(const char *color_ptr, const char *nombre_ptr,
                                   const char *mensaje_ptr) {
    imprimirNombreCombate(color_ptr, nombre_ptr);
    printf(" %s%s\n", mensaje_ptr, estiloTerminal(COLOR_RESET));
}

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
    imprimirNombreCombate(COLOR_ROJO, objetivo_ptr->nombre);
    printf(" recibe %d de dano (vida %d/%d)%s\n", dano, objetivo_ptr->vida,
           objetivo_ptr->vidaMax, estiloTerminal(COLOR_RESET));
    if (!estaVivoEnemigo(objetivo_ptr)) {
        imprimirMensajeCombate(COLOR_ROJO, objetivo_ptr->nombre, "fue derrotado.");
    }
}

void ataqueIndividual(Personaje_t atacante, Enemigo_t *objetivo_ptr, EfectoGrupal_t efecto) {
    int dano = calcularDano(ataqueEfectivo(atacante), objetivo_ptr->defensa);
    dano = aplicarCriticoGrupal(dano, efecto);
    imprimirAccionCombate(COLOR_VERDE, atacante.nombre, "ataca a", objetivo_ptr->nombre);
    recibirDanoEnemigo(objetivo_ptr, dano);
}

void ataqueArea(Personaje_t atacante, Enemigo_t enemigos_ptr[], int cantEnemigos, EfectoGrupal_t efecto) {
    imprimirMensajeCombate(COLOR_VERDE, atacante.nombre, "lanza un ataque en area.");
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
            imprimirNombreCombate(COLOR_VERDE, equipo_ptr[i].nombre);
            printf(" se cura (vida %d/%d)%s\n", equipo_ptr[i].vida,
                   equipo_ptr[i].vidaMax, estiloTerminal(COLOR_RESET));
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
            imprimirNombreCombate(COLOR_VERDE, usuario.nombre);
            printf(" activa Critico grupal: el equipo pega x1.2 por %d turnos.%s\n",
                   DURACION_HABILIDAD, estiloTerminal(COLOR_RESET));
            break;
        case HABILIDAD_DEFENSA_GRUPAL:
            efecto_ptr->tipo = EFECTO_GRUPAL_DEFENSA;
            imprimirNombreCombate(COLOR_VERDE, usuario.nombre);
            printf(" activa Defensa grupal: el equipo recibe menos dano por %d turnos.%s\n",
                   DURACION_HABILIDAD, estiloTerminal(COLOR_RESET));
            break;
        case HABILIDAD_CURACION_GRUPAL:
            efecto_ptr->tipo = EFECTO_GRUPAL_CURACION;
            imprimirMensajeCombate(COLOR_VERDE, usuario.nombre, "activa Curacion grupal.");
            curarEquipo(equipo_ptr, cantPersonajes, CURACION_GRUPAL);
            break;
    }

    efecto_ptr->turnosRestantes = DURACION_HABILIDAD;
    return 1;
}

void iniciarRonda(Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto) {
    if (efecto.tipo == EFECTO_GRUPAL_CURACION && efecto.turnosRestantes > 0) {
        printf("%sLa curacion grupal sigue actuando.%s\n",
               estiloTerminal(COLOR_VERDE), estiloTerminal(COLOR_RESET));
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

    imprimirAccionCombate(COLOR_ROJO, atacante.nombre, "ataca a", equipo_ptr[indice].nombre);
    if (atacante.habilidad == HABILIDAD_ENEMIGO_CRITICO && numeroAleatorio(1, 100) <= CRITICO_ENEMIGO_CHANCE) {
        dano = dano * CRITICO_ENEMIGO_PORCENTAJE / 100;
        printf("%s  Golpe critico!%s\n", estiloTerminal(COLOR_ROJO), estiloTerminal(COLOR_RESET));
    }

    equipo_ptr[indice].vida -= dano;
    if (equipo_ptr[indice].vida < 0) {
        equipo_ptr[indice].vida = 0;
    }
    imprimirNombreCombate(COLOR_VERDE, equipo_ptr[indice].nombre);
    printf(" recibe %d de dano (vida %d/%d)%s\n", dano, equipo_ptr[indice].vida,
           equipo_ptr[indice].vidaMax, estiloTerminal(COLOR_RESET));

    if (!estaVivoPersonaje(&equipo_ptr[indice])) {
        imprimirMensajeCombate(COLOR_VERDE, equipo_ptr[indice].nombre, "cayo en combate.");
    } else if (atacante.habilidad == HABILIDAD_ENEMIGO_TRAMPA && numeroAleatorio(1, 100) <= TRAMPA_CHANCE) {
        Estado_t debilitado = { ESTADO_DEBILITADO, DEBILITADO_DURACION, DEBILITADO_REDUCCION };
        aplicarEstadoPersonaje(&equipo_ptr[indice], debilitado);
        imprimirMensajeCombate(COLOR_VERDE, equipo_ptr[indice].nombre,
                               "cae en una trampa: queda debilitado.");
    }
}

void cerrarRonda(EfectoGrupal_t *efecto_ptr) {
    if (efecto_ptr->turnosRestantes > 0) {
        efecto_ptr->turnosRestantes--;
        if (efecto_ptr->turnosRestantes == 0) {
            efecto_ptr->tipo = EFECTO_GRUPAL_INACTIVO;
            printf("%sEl efecto de la habilidad especial termino.%s\n",
                   estiloTerminal(COLOR_AMARILLO), estiloTerminal(COLOR_RESET));
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
    printf("\n%s--- Tu equipo ---%s\n", estiloTerminal(COLOR_VERDE), estiloTerminal(COLOR_RESET));
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo_ptr[i])) {
            mostrarPersonaje(&equipo_ptr[i]);
        }
    }
    printf("%s--- Enemigos ---%s\n", estiloTerminal(COLOR_ROJO), estiloTerminal(COLOR_RESET));
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos_ptr[i])) {
            mostrarEnemigo(&enemigos_ptr[i]);
        }
    }
}

// Muestra los enemigos numerados y devuelve el indice de uno vivo.
static int elegirEnemigo(Enemigo_t enemigos_ptr[], int cantEnemigos) {
    if (cantEnemigos <= 0) {
        return -1;
    }
    printf("Elegi a quien atacar:\n");
    char opcionesEnemigos[cantEnemigos][100];
    const char *opcionesEnemigos_ptr[cantEnemigos];
    int indicesEnemigos[cantEnemigos];
    int cantidadOpciones = 0;
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos_ptr[i])) {
            snprintf(opcionesEnemigos[cantidadOpciones], sizeof(opcionesEnemigos[cantidadOpciones]),
                     "%s (vida %d/%d)", enemigos_ptr[i].nombre,
                     enemigos_ptr[i].vida, enemigos_ptr[i].vidaMax);
            opcionesEnemigos_ptr[cantidadOpciones] = opcionesEnemigos[cantidadOpciones];
            indicesEnemigos[cantidadOpciones] = i;
            cantidadOpciones++;
        }
    }
    if (cantidadOpciones == 0) {
        return -1;
    }
    return indicesEnemigos[leerOpcionMenu(opcionesEnemigos_ptr, cantidadOpciones)];
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
    char opcionesItems[MAX_SLOTS_INVENTARIO + 1][100];
    const char *opcionesItems_ptr[MAX_SLOTS_INVENTARIO + 1];
    for (int i = 0; i < inventario_ptr->cantidad; i++) {
        if (inventario_ptr->items[i].tipo == ITEM_VENENO) {
            snprintf(opcionesItems[i], sizeof(opcionesItems[i]), "%s (%d de dano por turno)",
                   nombreTipoItem(inventario_ptr->items[i].tipo), inventario_ptr->items[i].valor);
        } else {
            snprintf(opcionesItems[i], sizeof(opcionesItems[i]), "%s (+%d de vida)",
                   nombreTipoItem(inventario_ptr->items[i].tipo), inventario_ptr->items[i].valor);
        }
        opcionesItems_ptr[i] = opcionesItems[i];
    }
    opcionesItems_ptr[inventario_ptr->cantidad] = "Cancelar y volver al turno";
    int opcion = leerOpcionMenu(opcionesItems_ptr, inventario_ptr->cantidad + 1);
    return opcion == inventario_ptr->cantidad ? -1 : opcion;
}

// Turno de un personaje: menu de acciones. Repite el menu si la opcion no esta disponible.
static void turnoPersonaje(Personaje_t equipo_ptr[], int cantPersonajes, int actual,
                           Enemigo_t enemigos_ptr[], int cantEnemigos,
                           Inventario_t *inventario_ptr, EfectoGrupal_t *efecto_ptr) {
    int pocionUsada = 0;
    const char *opcionesAccion_ptr[] = {
        "Atacar a un enemigo",
        "Ataque en area",
        "Habilidad especial",
        "Usar pocion"
    };

    while (1) {
        printf("\nTurno de ");
        imprimirNombreCombate(COLOR_VERDE, equipo_ptr[actual].nombre);
        printf(":\n");
        char opcionHabilidad[64];
        char opcionPocion[80];
        snprintf(opcionHabilidad, sizeof(opcionHabilidad), "Habilidad especial%s",
                 efecto_ptr->tipo != EFECTO_GRUPAL_INACTIVO ? " (activa)" : "");
        snprintf(opcionPocion, sizeof(opcionPocion), "Usar pocion%s",
                 pocionUsada ? " (ya usada este turno)" : "");
        const char *opcionesActuales_ptr[] = {
            opcionesAccion_ptr[0],
            opcionesAccion_ptr[1],
            opcionHabilidad,
            opcionPocion
        };
        
        switch (leerOpcionMenu(opcionesActuales_ptr, 4)) {
            case 0: {
                int objetivo = elegirEnemigo(enemigos_ptr, cantEnemigos);
                if (objetivo >= 0) {
                    ataqueIndividual(equipo_ptr[actual], &enemigos_ptr[objetivo], *efecto_ptr);
                }
                return;
            }
            case 1:
                ataqueArea(equipo_ptr[actual], enemigos_ptr, cantEnemigos, *efecto_ptr);
                return;
            case 2:
                if (activarHabilidad(equipo_ptr[actual], equipo_ptr, cantPersonajes, efecto_ptr)) {
                    return;
                }
                printf("La habilidad especial ya esta activa.\n");
                break;
            case 3:
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
                    if (indiceItem < 0) {
                        break;
                    }
                    Enemigo_t *objetivoEnemigo_ptr = NULL;
                    if (inventario_ptr->items[indiceItem].tipo == ITEM_VENENO) {
                        int indiceEnemigo = elegirEnemigo(enemigos_ptr, cantEnemigos);
                        if (indiceEnemigo < 0) {
                            break;
                        }
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

    while (1) {
        limpiarPantallaTerminal();
        printf("%s%s===== COMBATE | RONDA %d =====%s\n",
               estiloTerminal(COLOR_AZUL_CLARO), estiloTerminal(ESTILO_NEGRITA),
               ronda, estiloTerminal(COLOR_RESET));
        iniciarRonda(equipo_ptr, cantPersonajes, efecto);
        mostrarEstadoCombate(equipo_ptr, cantPersonajes, enemigos_ptr, cantEnemigos);

        for (int i = 0; i < cantPersonajes && quedanEnemigosVivos(enemigos_ptr, cantEnemigos); i++) {
            if (!estaVivoPersonaje(&equipo_ptr[i])) {
                continue;
            }
            if (equipo_ptr[i].estadoActivo.tipo == ESTADO_ATURDIDO) {
                imprimirMensajeCombate(COLOR_VERDE, equipo_ptr[i].nombre,
                                       "esta aturdido y pierde el turno.");
            } else {
                turnoPersonaje(equipo_ptr, cantPersonajes, i, enemigos_ptr, cantEnemigos, inventario_ptr, &efecto);
            }
            actualizarEstadoPersonaje(&equipo_ptr[i]);
        }

        if (!quedanEnemigosVivos(enemigos_ptr, cantEnemigos)) {
            printf("\n%sVictoria! Todos los enemigos fueron derrotados.%s\n",
                   estiloTerminal(COLOR_VERDE), estiloTerminal(COLOR_RESET));
            registrarLog("Combate ganado");
            pausar();
            return 1;
        }

        printf("\n");
        for (int j = 0; j < cantEnemigos; j++) {
            if (!estaVivoEnemigo(&enemigos_ptr[j])) {
                continue;
            }
            if (enemigos_ptr[j].estadoActivo.tipo == ESTADO_ATURDIDO) {
                imprimirMensajeCombate(COLOR_ROJO, enemigos_ptr[j].nombre,
                                       "esta aturdido y pierde el turno.");
            } else {
                turnoEnemigo(enemigos_ptr[j], equipo_ptr, cantPersonajes, efecto);
            }
            actualizarEstadoEnemigo(&enemigos_ptr[j]);
        }

        if (!quedanPersonajesVivos(equipo_ptr, cantPersonajes)) {
            printf("\n%sDerrota. Tu equipo cayo en combate.%s\n",
                   estiloTerminal(COLOR_ROJO), estiloTerminal(COLOR_RESET));
            registrarLog("Combate perdido");
            pausar();
            return 0;
        }

        if (!quedanEnemigosVivos(enemigos_ptr, cantEnemigos)) {
            printf("\n%sVictoria! Todos los enemigos fueron derrotados.%s\n",
                   estiloTerminal(COLOR_VERDE), estiloTerminal(COLOR_RESET));
            registrarLog("Combate ganado");
            pausar();
            return 1;
        }

        cerrarRonda(&efecto);
        pausar();
        ronda++;
    }
}
