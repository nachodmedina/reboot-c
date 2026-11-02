#include <stdio.h>
#include "../include/combate.h"
#include "../include/utils.h"
#include "../include/archivos.h"

// Constantes de balance: ajustarlas aca sin tocar la logica.
#define AREA_BONUS                 15
#define DURACION_HABILIDAD          2
#define CRITICO_GRUPAL_PORCENTAJE 120   // Isaac: x1.2
#define DEFENSA_GRUPAL_PORCENTAJE 140   // Dante: +40% de defensa
#define CURACION_GRUPAL            40   // Frieren: HP por curacion
#define DEBILITADO_REDUCCION       25   // % de ataque que se pierde
#define DEBILITADO_DURACION         1
#define TRAMPA_CHANCE              40   // % de probabilidad
#define CRITICO_ENEMIGO_CHANCE     30   // % de probabilidad
#define CRITICO_ENEMIGO_PORCENTAJE 150  // x1.5
#define MITAD_DANO_PORCENTAJE      50   // Vergil: recibe la mitad
#define REGENERACION_VIDA          30   // Tiny: vida por turno
#define ATURDIR_CHANCE             25   // Golem: % de probabilidad
#define ATURDIR_DURACION            1
#define SEFIROT_AREA_CHANCE        25   // Sefirot: % de cada accion especial
#define SEFIROT_CURACION_CHANCE    20
#define SEFIROT_INVOCAR_CHANCE     20
#define SEFIROT_AREA_PORCENTAJE    75   // el area pega el 75% de su ataque
#define SEFIROT_CURACION           80

// Lo que va pasando en la ronda actual, para registrarlo cuando termina.
static int danoCausadoRonda = 0;
static int danoRecibidoRonda = 0;
static int enemigosDerrotadosRonda = 0;

static void imprimirNombreCombate(const char *color_ptr, const char *nombre_ptr) {
    printf("%s%s%s%s", estiloTerminal(color_ptr), estiloTerminal(ESTILO_NEGRITA),
           nombre_ptr, estiloTerminal(COLOR_RESET));
}

static const char *colorNombreEnemigo(const Enemigo_t *enemigo_ptr) {
    return esJefeEnemigo(enemigo_ptr) ? COLOR_AMARILLO : COLOR_ROJO;
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

int ataqueEfectivo(const Personaje_t *personaje_ptr) {
    if (personaje_ptr->estadoActivo.tipo == ESTADO_DEBILITADO) {
        return personaje_ptr->ataque * (100 - personaje_ptr->estadoActivo.magnitud) / 100;
    }
    return personaje_ptr->ataque;
}

// Aplica el critico grupal de Isaac si esta activo.
static int aplicarCriticoGrupal(int dano, EfectoGrupal_t efecto) {
    if (efecto.tipo == EFECTO_GRUPAL_CRITICO) {
        return dano * CRITICO_GRUPAL_PORCENTAJE / 100;
    }
    return dano;
}

static void recibirDanoEnemigo(Enemigo_t *objetivo_ptr, int dano) {
    if (objetivo_ptr->habilidad == HABILIDAD_ENEMIGO_MITAD_DANO) {
        dano = dano * MITAD_DANO_PORCENTAJE / 100;
        imprimirMensajeCombate(colorNombreEnemigo(objetivo_ptr), objetivo_ptr->nombre,
                               "absorbe la mitad del golpe.");
    }
    int vidaAntes = objetivo_ptr->vida;
    objetivo_ptr->vida -= dano;
    if (objetivo_ptr->vida < 0) {
        objetivo_ptr->vida = 0;
    }
    danoCausadoRonda += vidaAntes - objetivo_ptr->vida;
    imprimirNombreCombate(colorNombreEnemigo(objetivo_ptr), objetivo_ptr->nombre);
    printf(" recibe %d de dano (vida %d/%d)%s\n", dano, objetivo_ptr->vida,
           objetivo_ptr->vidaMax, estiloTerminal(COLOR_RESET));
    if (!estaVivoEnemigo(objetivo_ptr)) {
        enemigosDerrotadosRonda++;
        imprimirMensajeCombate(colorNombreEnemigo(objetivo_ptr), objetivo_ptr->nombre,
                               "fue derrotado.");
    }
}

void ataqueIndividual(const Personaje_t *atacante_ptr, Enemigo_t *objetivo_ptr, EfectoGrupal_t efecto) {
    int dano = calcularDano(ataqueEfectivo(atacante_ptr), objetivo_ptr->defensa);
    dano = aplicarCriticoGrupal(dano, efecto);
    imprimirNombreCombate(COLOR_VERDE, atacante_ptr->nombre);
    printf(" ataca a ");
    imprimirNombreCombate(colorNombreEnemigo(objetivo_ptr), objetivo_ptr->nombre);
    printf(".%s\n", estiloTerminal(COLOR_RESET));
    recibirDanoEnemigo(objetivo_ptr, dano);
}

void ataqueArea(const Personaje_t *atacante_ptr, Enemigo_t enemigos_ptr[], int cantEnemigos, EfectoGrupal_t efecto) {
    imprimirMensajeCombate(COLOR_VERDE, atacante_ptr->nombre, "lanza un ataque en area.");
    for (int i = 0; i < cantEnemigos; i++) {
        if (estaVivoEnemigo(&enemigos_ptr[i])) {
            int dano = calcularDanoArea(ataqueEfectivo(atacante_ptr), enemigos_ptr[i].defensa);
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

int activarHabilidad(const Personaje_t *usuario_ptr, Personaje_t equipo_ptr[], int cantPersonajes,
                     EfectoGrupal_t *efecto_ptr) {
    if (efecto_ptr->tipo != EFECTO_GRUPAL_INACTIVO) {
        return 0;
    }

    switch (usuario_ptr->habilidadEspecial) {
        case HABILIDAD_CRITICO_GRUPAL:
            efecto_ptr->tipo = EFECTO_GRUPAL_CRITICO;
            imprimirNombreCombate(COLOR_VERDE, usuario_ptr->nombre);
            printf(" activa Critico grupal: el equipo pega x1.2 por %d turnos.%s\n",
                   DURACION_HABILIDAD, estiloTerminal(COLOR_RESET));
            break;
        case HABILIDAD_DEFENSA_GRUPAL:
            efecto_ptr->tipo = EFECTO_GRUPAL_DEFENSA;
            imprimirNombreCombate(COLOR_VERDE, usuario_ptr->nombre);
            printf(" activa Defensa grupal: el equipo recibe menos dano por %d turnos.%s\n",
                   DURACION_HABILIDAD, estiloTerminal(COLOR_RESET));
            break;
        case HABILIDAD_CURACION_GRUPAL:
            efecto_ptr->tipo = EFECTO_GRUPAL_CURACION;
            imprimirMensajeCombate(COLOR_VERDE, usuario_ptr->nombre, "activa Curacion grupal.");
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
static int elegirObjetivoAleatorio(const Personaje_t equipo_ptr[], int cantPersonajes) {
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

// Defensa del personaje teniendo en cuenta la defensa grupal de Dante.
static int defensaEfectiva(const Personaje_t *personaje_ptr, EfectoGrupal_t efecto) {
    if (efecto.tipo == EFECTO_GRUPAL_DEFENSA) {
        return personaje_ptr->defensa * DEFENSA_GRUPAL_PORCENTAJE / 100;
    }
    return personaje_ptr->defensa;
}

static void recibirDanoPersonaje(Personaje_t *objetivo_ptr, int dano) {
    int vidaAntes = objetivo_ptr->vida;
    objetivo_ptr->vida -= dano;
    if (objetivo_ptr->vida < 0) {
        objetivo_ptr->vida = 0;
    }
    danoRecibidoRonda += vidaAntes - objetivo_ptr->vida;
    imprimirNombreCombate(COLOR_VERDE, objetivo_ptr->nombre);
    printf(" recibe %d de dano (vida %d/%d)%s\n", dano, objetivo_ptr->vida,
           objetivo_ptr->vidaMax, estiloTerminal(COLOR_RESET));
    if (!estaVivoPersonaje(objetivo_ptr)) {
        imprimirMensajeCombate(COLOR_VERDE, objetivo_ptr->nombre, "cayo en combate.");
    }
}

void turnoEnemigo(const Enemigo_t *atacante_ptr, Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto) {
    int indice = elegirObjetivoAleatorio(equipo_ptr, cantPersonajes);
    if (indice == -1) {
        return;
    }
    Personaje_t *objetivo_ptr = &equipo_ptr[indice];
    int dano = calcularDano(atacante_ptr->ataque, defensaEfectiva(objetivo_ptr, efecto));

    imprimirNombreCombate(colorNombreEnemigo(atacante_ptr), atacante_ptr->nombre);
    printf(" ataca a ");
    imprimirNombreCombate(COLOR_VERDE, objetivo_ptr->nombre);
    printf(".%s\n", estiloTerminal(COLOR_RESET));
    if (atacante_ptr->habilidad == HABILIDAD_ENEMIGO_CRITICO && numeroAleatorio(1, 100) <= CRITICO_ENEMIGO_CHANCE) {
        dano = dano * CRITICO_ENEMIGO_PORCENTAJE / 100;
        printf("%s  Golpe critico!%s\n", estiloTerminal(COLOR_ROJO), estiloTerminal(COLOR_RESET));
    }
    recibirDanoPersonaje(objetivo_ptr, dano);
    if (!estaVivoPersonaje(objetivo_ptr)) {
        return;
    }

    if (atacante_ptr->habilidad == HABILIDAD_ENEMIGO_TRAMPA && numeroAleatorio(1, 100) <= TRAMPA_CHANCE) {
        Estado_t debilitado = { ESTADO_DEBILITADO, DEBILITADO_DURACION, DEBILITADO_REDUCCION };
        aplicarEstadoPersonaje(objetivo_ptr, debilitado);
        imprimirMensajeCombate(COLOR_VERDE, objetivo_ptr->nombre, "cae en una trampa: queda debilitado.");
    } else if (atacante_ptr->habilidad == HABILIDAD_ENEMIGO_ATURDIR && numeroAleatorio(1, 100) <= ATURDIR_CHANCE) {
        Estado_t aturdido = { ESTADO_ATURDIDO, ATURDIR_DURACION, 0 };
        aplicarEstadoPersonaje(objetivo_ptr, aturdido);
        imprimirMensajeCombate(COLOR_VERDE, objetivo_ptr->nombre, "recibe un golpe aplastante: queda aturdido.");
    }
}

// Tiny: se repara al empezar su turno, sin pasar su vida maxima.
void regenerarEnemigo(Enemigo_t *enemigo_ptr) {
    if (enemigo_ptr->vida >= enemigo_ptr->vidaMax) {
        return;
    }
    enemigo_ptr->vida += REGENERACION_VIDA;
    if (enemigo_ptr->vida > enemigo_ptr->vidaMax) {
        enemigo_ptr->vida = enemigo_ptr->vidaMax;
    }
    imprimirNombreCombate(colorNombreEnemigo(enemigo_ptr), enemigo_ptr->nombre);
    printf(" se autorrepara (vida %d/%d)%s\n", enemigo_ptr->vida, enemigo_ptr->vidaMax,
           estiloTerminal(COLOR_RESET));
}

static void mostrarLineaSefirot(const char *linea_ptr) {
    printf("%s%s%s%s\n", estiloTerminal(COLOR_AMARILLO), estiloTerminal(ESTILO_NEGRITA),
           linea_ptr, estiloTerminal(COLOR_RESET));
}

static void ataqueAreaSefirot(const Enemigo_t *sefirot_ptr, Personaje_t equipo_ptr[], int cantPersonajes,
                              EfectoGrupal_t efecto) {
    mostrarLineaSefirot("Sefirot no elige un blanco. Elige a los tres a la vez.");
    int ataque = sefirot_ptr->ataque * SEFIROT_AREA_PORCENTAJE / 100;
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo_ptr[i])) {
            recibirDanoPersonaje(&equipo_ptr[i], calcularDano(ataque, defensaEfectiva(&equipo_ptr[i], efecto)));
        }
    }
}

static void curarSefirot(Enemigo_t *sefirot_ptr) {
    mostrarLineaSefirot("El dano que le hicieron se cierra solo, como si el Nucleo entero lo estuviera reparando.");
    sefirot_ptr->vida += SEFIROT_CURACION;
    if (sefirot_ptr->vida > sefirot_ptr->vidaMax) {
        sefirot_ptr->vida = sefirot_ptr->vidaMax;
    }
    imprimirNombreCombate(colorNombreEnemigo(sefirot_ptr), sefirot_ptr->nombre);
    printf(" se cura (vida %d/%d)%s\n", sefirot_ptr->vida, sefirot_ptr->vidaMax, estiloTerminal(COLOR_RESET));
}

int invocarRefuerzo(Batalla_t *batalla_ptr) {
    if (batalla_ptr->cantRefuerzos == 0 ||
        quedanEnemigosVivos(batalla_ptr->enemigos, batalla_ptr->cantEnemigos) >= MAX_ENEMIGOS_COMBATE) {
        return 0;
    }
    // Ocupa el lugar de un enemigo caido o, si no hay, uno libre al final.
    int lugar = 0;
    while (lugar < batalla_ptr->cantEnemigos && estaVivoEnemigo(&batalla_ptr->enemigos[lugar])) {
        lugar++;
    }
    if (lugar == batalla_ptr->cantEnemigos) {
        batalla_ptr->cantEnemigos++;
    }
    int elegido = numeroAleatorio(0, batalla_ptr->cantRefuerzos - 1);
    batalla_ptr->enemigos[lugar] = batalla_ptr->refuerzos[elegido];

    mostrarLineaSefirot("Algo conocido se materializa junto a Sefirot. Ya lo vencieron una vez. "
                        "Van a tener que hacerlo de nuevo.");
    imprimirMensajeCombate(colorNombreEnemigo(&batalla_ptr->enemigos[lugar]),
                           batalla_ptr->enemigos[lugar].nombre, "se une al combate.");
    return 1;
}

// Sefirot elige al azar entre ataque en area, curarse, invocar o atacar normal.
// Si la accion elegida no se puede hacer (vida llena, sin lugar), ataca normal.
static void turnoSefirot(Batalla_t *batalla_ptr, int indice, Personaje_t equipo_ptr[], int cantPersonajes,
                         EfectoGrupal_t efecto) {
    Enemigo_t *sefirot_ptr = &batalla_ptr->enemigos[indice];
    int tirada = numeroAleatorio(1, 100);
    if (tirada <= SEFIROT_AREA_CHANCE) {
        ataqueAreaSefirot(sefirot_ptr, equipo_ptr, cantPersonajes, efecto);
        return;
    }
    tirada -= SEFIROT_AREA_CHANCE;
    if (tirada <= SEFIROT_CURACION_CHANCE && sefirot_ptr->vida < sefirot_ptr->vidaMax) {
        curarSefirot(sefirot_ptr);
        return;
    }
    tirada -= SEFIROT_CURACION_CHANCE;
    if (tirada > 0 && tirada <= SEFIROT_INVOCAR_CHANCE && invocarRefuerzo(batalla_ptr)) {
        return;
    }
    turnoEnemigo(sefirot_ptr, equipo_ptr, cantPersonajes, efecto);
}

void faseEnemigos(Batalla_t *batalla_ptr, Personaje_t equipo_ptr[], int cantPersonajes, EfectoGrupal_t efecto) {
    // Los invocados en esta fase recien actuan en la ronda siguiente.
    int actuaEstaRonda[MAX_ENEMIGOS_COMBATE];
    for (int j = 0; j < MAX_ENEMIGOS_COMBATE; j++) {
        actuaEstaRonda[j] = j < batalla_ptr->cantEnemigos && estaVivoEnemigo(&batalla_ptr->enemigos[j]);
    }

    for (int j = 0; j < MAX_ENEMIGOS_COMBATE && quedanPersonajesVivos(equipo_ptr, cantPersonajes); j++) {
        Enemigo_t *enemigo_ptr = &batalla_ptr->enemigos[j];
        if (!actuaEstaRonda[j]) {
            continue;
        }
        if (enemigo_ptr->habilidad == HABILIDAD_ENEMIGO_REGENERACION) {
            regenerarEnemigo(enemigo_ptr);
        }
        if (enemigo_ptr->estadoActivo.tipo == ESTADO_ATURDIDO) {
            imprimirMensajeCombate(colorNombreEnemigo(enemigo_ptr), enemigo_ptr->nombre,
                                   "esta aturdido y pierde el turno.");
        } else if (enemigo_ptr->habilidad == HABILIDAD_ENEMIGO_SEFIROT) {
            turnoSefirot(batalla_ptr, j, equipo_ptr, cantPersonajes, efecto);
        } else {
            turnoEnemigo(enemigo_ptr, equipo_ptr, cantPersonajes, efecto);
        }
        int vidaAntes = enemigo_ptr->vida;
        actualizarEstadoEnemigo(enemigo_ptr);
        danoCausadoRonda += vidaAntes - enemigo_ptr->vida;
        if (vidaAntes > 0 && !estaVivoEnemigo(enemigo_ptr)) {
            enemigosDerrotadosRonda++;
        }
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

int quedanPersonajesVivos(const Personaje_t equipo_ptr[], int cantPersonajes) {
    int vivos = 0;
    for (int i = 0; i < cantPersonajes; i++) {
        if (estaVivoPersonaje(&equipo_ptr[i])) {
            vivos++;
        }
    }
    return vivos;
}

int quedanEnemigosVivos(const Enemigo_t enemigos_ptr[], int cantEnemigos) {
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

static void mostrarEstadoCombate(const Personaje_t equipo_ptr[], int cantPersonajes,
                                 const Enemigo_t enemigos_ptr[], int cantEnemigos) {
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

// Muestra el inventario y devuelve el indice del item elegido.
static int elegirItem(const Inventario_t *inventario_ptr) {
    printf("Elegi un item:\n");
    char opcionesItems[MAX_SLOTS_INVENTARIO + 1][100];
    const char *opcionesItems_ptr[MAX_SLOTS_INVENTARIO + 1];
    for (int i = 0; i < inventario_ptr->cantidad; i++) {
        describirItem(&inventario_ptr->items[i], opcionesItems[i], sizeof(opcionesItems[i]));
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
                    ataqueIndividual(&equipo_ptr[actual], &enemigos_ptr[objetivo], *efecto_ptr);
                }
                return;
            }
            case 1:
                ataqueArea(&equipo_ptr[actual], enemigos_ptr, cantEnemigos, *efecto_ptr);
                return;
            case 2:
                if (activarHabilidad(&equipo_ptr[actual], equipo_ptr, cantPersonajes, efecto_ptr)) {
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

static int terminarCombate(int gano) {
    if (gano) {
        printf("\n%sVictoria! Todos los enemigos fueron derrotados.%s\n",
               estiloTerminal(COLOR_VERDE), estiloTerminal(COLOR_RESET));
        registrarLog("Combate ganado");
    } else {
        printf("\n%sDerrota. Tu equipo cayo en combate.%s\n",
               estiloTerminal(COLOR_ROJO), estiloTerminal(COLOR_RESET));
        registrarLog("Combate perdido");
    }
    pausar();
    return gano;
}

// Guarda lo que paso en la ronda y deja los contadores listos para la siguiente.
static void registrarRonda(Estadisticas_t *estadisticas_ptr, int zona, int fase, int ronda) {
    if (estadisticas_ptr != NULL) {
        RegistroRonda_t registro = { zona, fase, ronda, danoCausadoRonda, danoRecibidoRonda, enemigosDerrotadosRonda };
        if (!agregarRegistroRonda(estadisticas_ptr, registro)) {
            printf("No hay memoria para registrar la ronda; las estadisticas quedan incompletas.\n");
        }
    }
    danoCausadoRonda = 0;
    danoRecibidoRonda = 0;
    enemigosDerrotadosRonda = 0;
}

int iniciarCombate(Personaje_t equipo_ptr[], int cantPersonajes, Batalla_t *batalla_ptr, Inventario_t *inventario_ptr,
                   Estadisticas_t *estadisticas_ptr, int zona, int fase) {
    EfectoGrupal_t efecto = crearEfectoGrupal();
    int ronda = 1;

    danoCausadoRonda = 0;
    danoRecibidoRonda = 0;
    enemigosDerrotadosRonda = 0;

    while (1) {
        limpiarPantallaTerminal();
        printf("%s%s===== COMBATE | RONDA %d =====%s\n",
               estiloTerminal(COLOR_AZUL_CLARO), estiloTerminal(ESTILO_NEGRITA),
               ronda, estiloTerminal(COLOR_RESET));
        iniciarRonda(equipo_ptr, cantPersonajes, efecto);
        mostrarEstadoCombate(equipo_ptr, cantPersonajes, batalla_ptr->enemigos, batalla_ptr->cantEnemigos);

        for (int i = 0; i < cantPersonajes && quedanEnemigosVivos(batalla_ptr->enemigos, batalla_ptr->cantEnemigos); i++) {
            if (!estaVivoPersonaje(&equipo_ptr[i])) {
                continue;
            }
            if (equipo_ptr[i].estadoActivo.tipo == ESTADO_ATURDIDO) {
                imprimirMensajeCombate(COLOR_VERDE, equipo_ptr[i].nombre,
                                       "esta aturdido y pierde el turno.");
            } else {
                turnoPersonaje(equipo_ptr, cantPersonajes, i, batalla_ptr->enemigos, batalla_ptr->cantEnemigos,
                               inventario_ptr, &efecto);
            }
            int vidaAntes = equipo_ptr[i].vida;
            actualizarEstadoPersonaje(&equipo_ptr[i]);
            danoRecibidoRonda += vidaAntes - equipo_ptr[i].vida;
        }
        if (!quedanEnemigosVivos(batalla_ptr->enemigos, batalla_ptr->cantEnemigos)) {
            registrarRonda(estadisticas_ptr, zona, fase, ronda);
            return terminarCombate(1);
        }

        printf("\n");
        faseEnemigos(batalla_ptr, equipo_ptr, cantPersonajes, efecto);
        registrarRonda(estadisticas_ptr, zona, fase, ronda);
        if (!quedanPersonajesVivos(equipo_ptr, cantPersonajes)) {
            return terminarCombate(0);
        }
        if (!quedanEnemigosVivos(batalla_ptr->enemigos, batalla_ptr->cantEnemigos)) {
            return terminarCombate(1);   // el ultimo enemigo murio por veneno
        }

        cerrarRonda(&efecto);
        pausar();
        ronda++;
    }
}
