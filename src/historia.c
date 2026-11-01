#include <stdio.h>
#include "../include/arte_ascii.h"
#include "../include/historia.h"
#include "../include/utils.h"

static void agregarRecompensa(Inventario_t *inventario_ptr, TipoItem tipo, int valor) {
    Item_t item = { tipo, valor };
    if (!agregarItem(inventario_ptr, item)) {
        printf("La bolsa esta llena; no puedes guardar ese consumible.\n");
    }
}

// Bloques 1 y 2: contexto del mundo y el ataque a Raiz.
// No recibe parametros porque este texto es igual sin importar que
// personaje vaya a elegir el jugador despues.
void mostrarIntroduccionMundo(void) {
    mostrarTextoEnCaja("REBOOT C",
        "Año 2076. La humanidad lleva medio siglo combatiendo contra las maquinas.\n"
        "Medio siglo desde que la superficie dejo de ser un lugar seguro para vivir.\n\n"
        "Los pocos que sobrevivieron encontraron refugio bajo tierra, en los tuneles "
        "de La Raiz, el ultimo asentamiento humano del que se tiene noticia.\n"
        "Alli abajo, creian estar a salvo.",
        COLOR_BLANCO);
    pausar();

    // --- Bloque 2 ---
    mostrarTextoEnCaja("LA CAIDA DE LA RAIZ",
        "Esta noche, esa certeza tambien se rompio.\n\n"
        "Las alarmas de La Raiz, enmudecidas durante decadas, han despertado. "
        "Hoy chillan, y no dejan de sonar. Las maquinas han logrado infiltrarse.\n\n"
        "Comienza una batalla desesperada para defender los tuneles, pero la "
        "resistencia humana es aplastada casi de inmediato.\n"
        "El acero y el fuego arrasan con las defensas.\n"
        "Algunos son masacrados. Los demas... son arrastrados a la oscuridad "
        "como prisioneros.",
        COLOR_BLANCO);
    pausar();
}

// Devuelve el texto de la habilidad especial que desbloquea cada personaje
// al transformar su arma. Recibe directamente el TipoHabilidadEspecial (el
// dato ya vive en la struct Personaje_t, seteado desde crearPersonaje), asi
// que no hace falta comparar nombres de personaje para saber cual es.
static const char *textoHabilidadDesbloqueada(TipoHabilidadEspecial habilidad) {
    switch (habilidad) {
        case HABILIDAD_CRITICO_GRUPAL:
            return "El dagas, gastadas hasta hace un momento, ahora tienen unos bordes\n"
                    "que no parece de este siglo. \n"
                   "Isaac entiende, sin que nadie se lo explique,\n"
                   "que su instinto asesino se ha afilado: su proximo golpe sera letal.\n";
        case HABILIDAD_DEFENSA_GRUPAL:
            return "El espadon viejo, curtido de tantos entrenamientos sin gracia, ahora\n"
                   "responde con un peso distinto."
                  "Dante siente que su guardia es\n"
                   "impenetrable, un escudo absoluto capaz de protegerlo.\n";
        case HABILIDAD_CURACION_GRUPAL:
            return "El baston desgastado, el mismo que cargo siempre sin saber por que,\n"
                   "empieza a vibrar con una calidez que no sabe nombrar todavia, pero que\n"
                   "reconoce. Es magia antigua, pura vida fluyendo por sus manos para\n"
                   "sanar sus propias heridas y sostener a los que aun lo necesiten.\n";

        default:
            // Caso general: por si en el futuro se agrega un personaje sin
            // una habilidad especifica todavia asignada.
            return "Algo cambio. Ahora sabe usarla de verdad.\n";
    }
}

// Bloques 3, 4 y 5: la separacion, el hallazgo del arma comun junto a la
// baliza, y la transformacion del arma segun el personaje elegido.
void mostrarNarrativaInicial(const Personaje_t *personaje_ptr) {
    limpiarPantallaTerminal();
    mostrarTextoEnCaja(NULL, personaje_ptr->nombre, COLOR_BLANCO);
    mostrarArtePersonaje(personaje_ptr->habilidadEspecial);
    pausar();

    // --- Bloque 3: la separacion ---
    char textoNarrativa[2200];
    snprintf(textoNarrativa, sizeof(textoNarrativa),
        "En el caos de la batalla, %s queda separado del resto.\n"
        "No hay tiempo para ayudar a nadie: solo para correr, y esperar a encontrar "
        "otros sobrevivientes\n"
        "el sonido de los engranajes y el fuego enemigo lo obligan a buscar refugio "
        "entre las ruinas.",
        personaje_ptr->nombre);
    mostrarTextoEnCaja(NULL, textoNarrativa, COLOR_BLANCO);
    pausar();

    // --- Bloque 4: el hallazgo del arma comun + la baliza ---
    snprintf(textoNarrativa, sizeof(textoNarrativa),
        "Al adentrarse en la oscuridad, la respiracion de %s se detiene.\n"
        "Frente a el se alza una enorme compuerta de metal oscuro, cubierta de "
        "inscripciones de una era olvidada. La puerta cede con un gemido pesado...\n\n"
        "En el centro de la sala, un pilar monolitico pulsa con una luz violeta.\n"
        "%s Al acercarse y apoyar su mano, el pilar estalla en un relampago "
        "silencioso.\n"
        "Visiones invaden la mente de %s:\n\n"
        "> Ciudades enteras consumidas por el fuego...\n"
        "> Ejercitos antiguos cayendo ante legiones de maquinas...\n"
        "> Y en el centro de la masacre, un guerrero empunando tu %s,\n"
        "pero brillando con un poder absoluto y devastador.\n\n"
        "Las visiones colapsan de golpe dejandote sin aliento.",
        personaje_ptr->nombre, personaje_ptr->nombre, personaje_ptr->nombre,
        personaje_ptr->arma);
    mostrarTextoEnCaja("LA BALIZA", textoNarrativa, COLOR_BLANCO);
    pausar();
    snprintf(textoNarrativa, sizeof(textoNarrativa),
        "El pilar libera un latido de energia pura que impacta de lleno en tu arma.\n"
        "%s observa como su %s comienza a transformarse y a parecerse al arma "
        "que le mostro la baliza.\n"
        "El acero oxidado de la vieja arma temblo.\n"
        "El oxido y la sangre seca se desprendieron en el aire como cenizas "
        "arrastradas por un viento invisible,\n"
        "revelando una superficie que ya no era hierro, sino un cristal oscuro "
        "y perfecto que devoraba la luz del entorno.\n"
        "El peso del arma cambio drasticamente, volviendose ligera como una "
        "pluma pero anclada al brazo como una extension misma de su alma.\n"
        "Las muescas de mil batallas pasadas se fundieron, retorciendose hasta "
        "formar runas grabadas a fuego blanco a lo largo del arma.\n\n"
        "La sala entera vibra por el zumbido del arma despertando. Una leyenda "
        "acaba de renacer.",
        personaje_ptr->nombre, personaje_ptr->arma);
    mostrarTextoEnCaja("EL ARMA DESPIERTA", textoNarrativa, COLOR_BLANCO);
    pausar();

    mostrarTextoEnCaja("ARMA CONSEGUIDA", personaje_ptr->arma, COLOR_BLANCO);
    mostrarArteArma(personaje_ptr->habilidadEspecial);

    // Texto especifico de la habilidad que desbloquea cada personaje.
    mostrarTextoEnCaja(NULL,
                       textoHabilidadDesbloqueada(personaje_ptr->habilidadEspecial),
                       COLOR_BLANCO);
    pausar();
}

// La baliza empieza a guiar el camino: se acaba la posibilidad de volver a
// Raiz, y arranca el viaje hacia la primera zona. Depende del nombre del
// personaje, por eso recibe el puntero.
void mostrarSalidaDeRaiz(const Personaje_t *personaje_ptr) {
    char textoNarrativa[1000];
    snprintf(textoNarrativa, sizeof(textoNarrativa),
        "Con el arma transformada en su mano, %s siente algo mas: la baliza, "
        "antes silenciosa, vibra con un pulso constante, como si supiera "
        "exactamente hacia donde hay que ir.\n\n"
        "un pequeño pedazo de la baliza se desprende y se posa en tu mano,\n"
        "ahora esta pequeña baliza proyecta una luz tenua hacia la salida de las ruinas,\n"
        "marcando un camino que %s no habia visto antes.",
        personaje_ptr->nombre, personaje_ptr->nombre);
    mostrarTextoEnCaja("EL CAMINO", textoNarrativa, COLOR_BLANCO);
    pausar();

    snprintf(textoNarrativa, sizeof(textoNarrativa),
        "Regresar a Raiz ya no es una opcion. Lo que quedaba del refugio esta\n"
        "en manos de las maquinas, y no hay señal de que nadie mas haya sobrevivido...\n"
        "todavia.\n\n"
        "%s respira hondo, aferra el arma, y sigue la luz de la baliza hacia\n"
        "la superficie. El viaje recien empieza.",
        personaje_ptr->nombre);
    mostrarTextoEnCaja("HACIA LA SUPERFICIE", textoNarrativa, COLOR_BLANCO);
    pausar();
}

// Muestra el "cartel" de entrada a una zona nueva, usando directamente los
// datos de la struct Zona_t (nombre y descripcion) en vez de textos sueltos
// escritos a mano en cada lugar donde se entra a una zona.
void mostrarEntradaZona(const Zona_t *zona_ptr) {
    mostrarTextoEnCajaConTituloEstilizado(zona_ptr->nombre, zona_ptr->descripcion,
                                          COLOR_BLANCO, COLOR_AMARILLO,
                                          ESTILO_NEGRITA);
    pausar();
}

// Escena corta de transicion justo antes de un combate.
void mostrarEncuentroEnemigos(const Personaje_t *personaje_ptr, const char *recorrido_ptr,
                              const char *amenaza_ptr) {
    char textoNarrativa[600];
    snprintf(textoNarrativa, sizeof(textoNarrativa),
        "Mientras %s avanza %s, un sonido metalico y "
        "extraño rompe el silencio. No hay tiempo para pensar: %s",
        personaje_ptr->nombre, recorrido_ptr, amenaza_ptr);
    mostrarTextoEnCaja(NULL, textoNarrativa, COLOR_BLANCO);
    pausar();
}

void mostrarPostBatalla(const Personaje_t *personaje_ptr) {
    char textoNarrativa[300];
    snprintf(textoNarrativa, sizeof(textoNarrativa),
        "Luego de la batalla, %s toma un descanso y sigue avanzando, sabiendo "
        "que adelante lo esperan muchas mas amenazas.",
        personaje_ptr->nombre);
    mostrarTextoEnCaja(NULL, textoNarrativa, COLOR_BLANCO);
    pausar();
}

void mostrarHallazgoMejora(void) {
    mostrarTextoEnCaja(NULL,
        "Entre los restos del jefe encuentras una mejora para tu equipamiento.",
        COLOR_BLANCO);
    pausar();
}

void mostrarReaccionSefirot(const char *atributoEnemigo_ptr, int aumentoAcumulado) {
    char textoNarrativa[500];
    snprintf(textoNarrativa, sizeof(textoNarrativa),
        "La baliza emite un pulso que se extiende por todo el campo de batalla.\n"
        "Muy lejos de alli, Sefirot observa una lectura de energia desconocida.\n"
        "\"Reconozco esa mejora. Si ellos se fortalecen, mis maquinas tambien.\"\n"
        "Los enemigos de las proximas zonas ganaran %d puntos de %s acumulados.",
        aumentoAcumulado, atributoEnemigo_ptr);
    mostrarTextoEnCaja("SEFIROT", textoNarrativa, COLOR_BLANCO);
    pausar();
}

void mostrarLiberacionAliado(const Personaje_t *protagonista_ptr, const Personaje_t *aliado_ptr) {
    char textoNarrativa[700];
    snprintf(textoNarrativa, sizeof(textoNarrativa),
        "La baliza de %s comienza a parpadear y proyecta un camino entre las "
        "ruinas. Al final, una jaula de energia mantiene prisionero a %s.\n"
        "%s se acerca y rompe el sello de la jaula. La baliza toca el %s de %s, "
        "y su arma empieza a transformarse.\n"
        "%s recupera el aliento: \"Gracias. En la proxima batalla luchare a tu "
        "lado.\"\n%s se unio a tu aventura.",
        protagonista_ptr->nombre, aliado_ptr->nombre, protagonista_ptr->nombre,
        aliado_ptr->arma, aliado_ptr->nombre, aliado_ptr->nombre, aliado_ptr->nombre);
    mostrarTextoEnCaja(NULL, textoNarrativa, COLOR_BLANCO);
    pausar();

    limpiarPantallaTerminal();
    mostrarTextoEnCaja(NULL, aliado_ptr->nombre, COLOR_BLANCO);
    mostrarArtePersonaje(aliado_ptr->habilidadEspecial);
    pausar();

    snprintf(textoNarrativa, sizeof(textoNarrativa),
             "La baliza completa la transformacion del arma de %s.\n"
             "%s ya esta lista para luchar a tu lado.",
             aliado_ptr->nombre, aliado_ptr->arma);
    limpiarPantallaTerminal();
    mostrarTextoEnCaja("ARMA CONSEGUIDA", textoNarrativa, COLOR_BLANCO);
    mostrarArteArma(aliado_ptr->habilidadEspecial);
    pausar();
}

void eventoEntreFases(Personaje_t *personaje_ptr, Inventario_t *inventario_ptr) {
    char textoEvento[500];

    switch (numeroAleatorio(0, 8)) {
        case 0:
        case 1:
            snprintf(textoEvento, sizeof(textoEvento),
                     "%s sigue avanzando entre las ruinas. Algo llama su atencion, "
                     "pero no era nada. El camino sigue en silencio.",
                     personaje_ptr->nombre);
            mostrarTextoEnCaja(NULL, textoEvento, COLOR_BLANCO);
            break;
        case 2:
        case 3:
            agregarRecompensa(inventario_ptr, ITEM_VIDA, 100);
            snprintf(textoEvento, sizeof(textoEvento),
                     "%s encuentra una pocion de curacion entre los restos.",
                     personaje_ptr->nombre);
            mostrarTextoEnCaja(NULL, textoEvento, COLOR_BLANCO);
            break;
        case 4:
        case 5:
            agregarRecompensa(inventario_ptr, ITEM_VENENO, 5);
            snprintf(textoEvento, sizeof(textoEvento),
                     "%s descubre un frasco de veneno entre los escombros.",
                     personaje_ptr->nombre);
            mostrarTextoEnCaja(NULL, textoEvento, COLOR_BLANCO);
            break;
        case 6: {
            Estado_t veneno = { ESTADO_VENENO, 2, 10 };
            aplicarEstadoPersonaje(personaje_ptr, veneno);
            snprintf(textoEvento, sizeof(textoEvento),
                     "Una nube toxica se libera al mover los escombros y alcanza a %s. "
                     "Queda envenenado: perdera 10 de vida durante 2 turnos.",
                     personaje_ptr->nombre);
            mostrarTextoEnCaja(NULL, textoEvento, COLOR_BLANCO);
            break;
        }
        case 7:
            if (personaje_ptr->estadoActivo.tipo == ESTADO_VENENO) {
                personaje_ptr->estadoActivo.tipo = ESTADO_NINGUNO;
                personaje_ptr->estadoActivo.duracionTurnos = 0;
                personaje_ptr->estadoActivo.magnitud = 0;
                snprintf(textoEvento, sizeof(textoEvento),
                         "%s encuentra una planta medicinal y la aplica sobre la "
                         "herida. El veneno deja de afectarle.",
                         personaje_ptr->nombre);
            } else {
                snprintf(textoEvento, sizeof(textoEvento),
                         "%s encuentra una planta medicinal, pero no tiene veneno "
                         "que curar.",
                         personaje_ptr->nombre);
            }
            mostrarTextoEnCaja(NULL, textoEvento, COLOR_BLANCO);
            break;
        case 8:
            switch (numeroAleatorio(1, 3)) {
                case 1:
                    agregarRecompensa(inventario_ptr, ITEM_VIDA, 100);
                    agregarRecompensa(inventario_ptr, ITEM_VIDA, 100);
                    snprintf(textoEvento, sizeof(textoEvento),
                             "Un pequeno robot de apoyo emerge de entre los "
                             "escombros. \"Sobreviviente detectado. Preparando "
                             "asistencia.\" El robot ofrece dos pociones de curacion.");
                    break;
                case 2:
                    agregarRecompensa(inventario_ptr, ITEM_VENENO, 5);
                    agregarRecompensa(inventario_ptr, ITEM_VENENO, 5);
                    snprintf(textoEvento, sizeof(textoEvento),
                             "Un pequeno robot de apoyo emerge de entre los "
                             "escombros. \"Sobreviviente detectado. Preparando "
                             "asistencia.\" El robot ofrece dos frascos de veneno.");
                    break;
                case 3:
                    personaje_ptr->vidaMax += 10;
                    personaje_ptr->vida += 10;
                    personaje_ptr->ataque += 10;
                    personaje_ptr->defensa += 10;
                    snprintf(textoEvento, sizeof(textoEvento),
                             "Un pequeno robot de apoyo emerge de entre los "
                             "escombros. \"Sobreviviente detectado. Preparando "
                             "asistencia.\" Calibra el equipo y refuerza el cuerpo "
                             "de %s.\nVida maxima +10, ataque +10 y defensa +10.\n"
                             "Ahora tiene %d/%d de vida, %d de ataque y %d de defensa.",
                             personaje_ptr->nombre, personaje_ptr->vida,
                             personaje_ptr->vidaMax, personaje_ptr->ataque,
                             personaje_ptr->defensa);
                    break;
            }
            mostrarTextoEnCaja(NULL, textoEvento, COLOR_BLANCO);
            break;
    }

    pausar();

}