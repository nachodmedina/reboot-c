#include <string.h>
#include "../include/zonas.h"

static Zona_t crearZona(int numero, const char *nombre_ptr, const char *descripcion_ptr,
                        const char *recorrido_ptr) {
    Zona_t zona;
    zona.numero = numero;
    strncpy(zona.nombre, nombre_ptr, sizeof(zona.nombre) - 1);
    zona.nombre[sizeof(zona.nombre) - 1] = '\0';
    strncpy(zona.descripcion, descripcion_ptr, sizeof(zona.descripcion) - 1);
    zona.descripcion[sizeof(zona.descripcion) - 1] = '\0';
    strncpy(zona.recorrido, recorrido_ptr, sizeof(zona.recorrido) - 1);
    zona.recorrido[sizeof(zona.recorrido) - 1] = '\0';
    zona.cantFases = 0;
    return zona;
}

// Agrega una fase al final de la zona. Los enemigos se copian.
// Devuelve 1 si la agrego, 0 si no hay lugar.
static int agregarFase(Zona_t *zona_ptr, const char *amenaza_ptr, const Enemigo_t enemigos_ptr[],
                       int cantEnemigos, int fogataAntes) {
    if (zona_ptr->cantFases >= MAX_FASES_ZONA || cantEnemigos > MAX_ENEMIGOS_COMBATE) {
        return 0;
    }
    Fase_t *fase_ptr = &zona_ptr->fases[zona_ptr->cantFases];
    strncpy(fase_ptr->amenaza, amenaza_ptr, sizeof(fase_ptr->amenaza) - 1);
    fase_ptr->amenaza[sizeof(fase_ptr->amenaza) - 1] = '\0';
    for (int i = 0; i < cantEnemigos; i++) {
        fase_ptr->batalla.enemigos[i] = enemigos_ptr[i];
    }
    fase_ptr->batalla.cantEnemigos = cantEnemigos;
    fase_ptr->batalla.cantRefuerzos = 0;
    fase_ptr->fogataAntes = fogataAntes;
    zona_ptr->cantFases++;
    return 1;
}

// ============================================================
// Zona 1: Campo Ceniza (en solitario)
// ============================================================

static Enemigo_t crearCentinela(void) {
    return crearEnemigo("Centinela Canino", HABILIDAD_ENEMIGO_NINGUNA, 120, 10, 48);
}

static Enemigo_t crearCazador(void) {
    return crearEnemigo("Cazador de Campo Ceniza", HABILIDAD_ENEMIGO_TRAMPA, 160, 16, 56);
}

static Zona_t crearZonaCampoCeniza(void) {
    Zona_t zona = crearZona(1, "Campo Ceniza",
        "Lo que fue un pueblo agricola es hoy tierra quemada, vigilada por\n"
        "cazadores al servicio de las maquinas y sus perros mecanicos.",
        "entre las ruinas del pueblo");

    Enemigo_t centinela = crearCentinela();
    Enemigo_t cazador = crearCazador();
    Enemigo_t spectre = crearJefe("Spectre", HABILIDAD_ENEMIGO_CRITICO, 280, 40, 64);

    Enemigo_t fase1[] = { centinela, centinela };
    agregarFase(&zona, "dos perros mecanicos emergen entre la ceniza y corren directo hacia vos.",
                fase1, 2, 0);

    Enemigo_t fase2[] = { cazador };
    agregarFase(&zona, "un cazador sale de entre los restos de un granero y te apunta.",
                fase2, 1, 0);

    Enemigo_t fase3[] = { cazador, centinela, centinela };
    agregarFase(&zona, "otro cazador aparece, esta vez acompañado por sus perros.",
                fase3, 3, 0);

    // Fase del jefe: antes se descansa en una fogata.
    Enemigo_t fase4[] = { cazador, spectre };
    agregarFase(&zona, "En el centro del pueblo esta su lider: es Spectre.",
                fase4, 2, 1);

    return zona;
}

// ============================================================
// Zona 2: Catedral de Engranajes (de a dos)
// ============================================================

static Enemigo_t crearEscudero(void) {
    return crearEnemigo("Escudero", HABILIDAD_ENEMIGO_NINGUNA, 150, 20, 50);
}

static Enemigo_t crearGuardian(void) {
    return crearEnemigo("Guardián", HABILIDAD_ENEMIGO_NINGUNA, 200, 26, 64);
}

static Zona_t crearZonaCatedral(void) {
    Zona_t zona = crearZona(2, "Catedral de Engranajes",
        "Una catedral levantada con engranajes y chatarra. \n"
        "Adentro, los protectores de la fe que le rezan a las maquinas",
        "y castigan a los que se niegan a iluminarse.");

    Enemigo_t escudero = crearEscudero();
    Enemigo_t guardian = crearGuardian();
    Enemigo_t vergil = crearJefe("Vergil", HABILIDAD_ENEMIGO_MITAD_DANO, 380, 36, 80);

    Enemigo_t fase1[] = { escudero, escudero };
    agregarFase(&zona, "dos escuderos conversos cierran el paso con sus escudos remachados.",
                fase1, 2, 0);

    Enemigo_t fase2[] = { escudero, guardian };
    agregarFase(&zona, "un guardian baja del coro, escoltado por un escudero que reza en voz baja.",
                fase2, 2, 0);

    Enemigo_t fase3[] = { guardian, escudero, escudero };
    agregarFase(&zona, "un guardian y dos escuderos salen de las capillas laterales al mismo tiempo.",
                fase3, 3, 0);

    Enemigo_t fase4[] = { guardian, guardian, vergil };
    agregarFase(&zona, "frente al altar espera Vergil, mitad hombre y mitad maquina, entre dos guardianes.",
                fase4, 3, 1);

    return zona;
}

// ============================================================
// Zona 3: La Fundicion (los tres juntos)
// ============================================================

static Enemigo_t crearMutante(void) {
    return crearEnemigo("Mutante", HABILIDAD_ENEMIGO_NINGUNA, 190, 24, 64);
}

static Enemigo_t crearGolem(void) {
    return crearEnemigo("Gólem", HABILIDAD_ENEMIGO_ATURDIR, 260, 30, 74);
}

static Zona_t crearZonaFundicion(void) {
    Zona_t zona = crearZona(3, "La Fundición",
        "Hornos que nunca se apagan y cintas que arrastran piezas a medio armar.\n"
        "Aca las maquinas se fabrican a si mismas, y lo que sale mal sigue caminando.",
        "entre los hornos de la Fundicion");

    Enemigo_t mutante = crearMutante();
    Enemigo_t golem = crearGolem();
    Enemigo_t tiny = crearJefe("Tiny", HABILIDAD_ENEMIGO_REGENERACION, 500, 36, 86);

    Enemigo_t fase1[] = { mutante, mutante };
    agregarFase(&zona, "dos mutantes, unidades de ensamblaje defectuosas, se arrastran fuera de una cinta.",
                fase1, 2, 0);

    Enemigo_t fase2[] = { mutante, golem };
    agregarFase(&zona, "un golem pesado aparta los escombros y avanza detras de un mutante.",
                fase2, 2, 0);

    Enemigo_t fase3[] = { golem, mutante, mutante };
    agregarFase(&zona, "un golem y dos mutantes bloquean la puerta al centro de la fundicion.",
                fase3, 3, 0);

    Enemigo_t fase4[] = { golem, golem, tiny };
    agregarFase(&zona, "En el centro de la fundicion. Tiny, un robot de 2 metros capaz de destruir todo a su paso",
                fase4, 3, 1);

    return zona;
}

// ============================================================
// Zona 4: El Nucleo (zona final)
// Los "restos" de las zonas anteriores, con sus mismas stats.
// ============================================================

static Zona_t crearZonaNucleo(void) {
    Zona_t zona = crearZona(4, "El Núcleo",
        "El corazon de las maquinas, bajo la montaña. Aca Sefirot junta los restos\n"
        "de todo lo que el equipo ya vencio, para cerrarle el camino una ultima vez.",
        "por los tuneles del Nucleo");

    Enemigo_t restosCampo[] = { crearCazador(), crearCazador(), crearCentinela() };
    agregarFase(&zona, "los restos de Campo Ceniza vuelven a levantarse: dos cazadores y un perro mecanico.",
                restosCampo, 3, 0);

    Enemigo_t restosCatedral[] = { crearGuardian(), crearEscudero(), crearEscudero() };
    agregarFase(&zona, "conversos de la Catedral salen de la oscuridad, todavia rezandole a las maquinas.",
                restosCatedral, 3, 0);

    Enemigo_t restosFundicion[] = { crearGolem(), crearMutante(), crearMutante() };
    agregarFase(&zona, "piezas de la Fundicion se arman solas delante del equipo: un golem y dos mutantes.",
                restosFundicion, 3, 0);

    Enemigo_t jefeFinal[] = { crearJefe("Sefirot", HABILIDAD_ENEMIGO_SEFIROT, 1000, 50, 100) };
    agregarFase(&zona, "en el centro del Nucleo, Sefirot abre los ojos. Los estaba esperando.",
                jefeFinal, 1, 1);

    // Sefirot puede invocar una unidad de cada zona anterior.
    Batalla_t *batallaFinal_ptr = &zona.fases[zona.cantFases - 1].batalla;
    batallaFinal_ptr->refuerzos[0] = crearCazador();
    batallaFinal_ptr->refuerzos[1] = crearGuardian();
    batallaFinal_ptr->refuerzos[2] = crearMutante();
    batallaFinal_ptr->cantRefuerzos = 3;

    return zona;
}

Zona_t crearZonaPorNumero(int numero) {
    switch (numero) {
        case 2:
            return crearZonaCatedral();
        case 3:
            return crearZonaFundicion();
        case 4:
            return crearZonaNucleo();
        case 1:
        default:
            return crearZonaCampoCeniza();
    }
}
