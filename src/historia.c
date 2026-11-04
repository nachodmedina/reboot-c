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
        "Mientras %s avanza ,%s un sonido metalico y "
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

// ============================================================
// Textos del documento de diseno que cambian segun el protagonista.
// Cada tabla se indexa con la habilidad especial del protagonista
// (el mismo criterio que textoHabilidadDesbloqueada).
// ============================================================

// Liberacion del aliado al cerrar la Zona 1 [0] y la Zona 2 [1].
static const char *const TEXTOS_LIBERACION[][2] = {
    [HABILIDAD_CRITICO_GRUPAL] = {   // Isaac libera a Frieren y despues a Dante
        "Con Spectre caído, la baliza reacciona por primera vez desde que Isaac la encontró — un "
        "pulso leve, casi un latido, que apunta hacia una de las jaulas del asentamiento.\n\n"
        "Adentro, una chica que no suelta un bastón roto, como si todavía esperara que sirviera para "
        "algo. Isaac no llega a preguntar nada: la baliza responde por los dos, con el mismo brillo "
        "que tuvo en las ruinas. El bastón deja de estar roto. O tal vez nunca lo estuvo del todo.\n\n"
        "— También te esperaba a vos —dice Isaac, sin estar del todo seguro de a quién se lo dice: si "
        "a Frieren, o a la baliza.",

        "Vergil cae, y con él, buena parte del control que la Catedral tenía sobre los suyos. La "
        "baliza vuelve a guiarlos, esta vez hacia las celdas más profundas, donde los conversos "
        "guardaban a quienes todavía se negaban a \"iluminarse\".\n\n"
        "Dante está ahí, con un ojo cubierto por una placa metálica tosca, puesta a la fuerza. El "
        "espadón que llevaba antes de caer prisionero sigue tirado a su lado, sin filo, sin uso. La "
        "baliza lo toca apenas, y el metal responde — no borra el ojo que le impusieron, pero sí le "
        "devuelve el arma que era suya de verdad.\n\n"
        "— No es la primera vez que alguien pierde algo acá adentro —dice Dante, mirándose la placa en "
        "el ojo—. Que sea la última vez que me lo sacan sin que yo elija."
    },
    [HABILIDAD_DEFENSA_GRUPAL] = {   // Dante libera a Frieren y despues a Isaac
        "Con Spectre caído, la baliza reacciona por primera vez desde que Dante la encontró — un pulso "
        "leve que apunta hacia una de las jaulas del asentamiento.\n\n"
        "Adentro, una chica que no suelta un bastón roto, como si todavía esperara que sirviera para "
        "algo. La baliza responde antes de que Dante llegue a preguntar nada: el mismo brillo que tuvo "
        "en las ruinas. El bastón deja de estar roto.\n\n"
        "— También te esperaba a vos —dice Dante, sin estar del todo seguro de a quién se lo dice.",

        "Vergil cae, y con él buena parte del control que la Catedral tenía sobre los suyos. La baliza "
        "guía a Dante y Frieren hacia las celdas más profundas.\n\n"
        "Isaac está ahí, con el brazo metálico que ya llevaba desde Raíz y, ahora, una placa tosca "
        "puesta a la fuerza sobre una pierna. Las dagas que llevaba antes de caer prisionero siguen "
        "tiradas a su lado, sin filo. La baliza las toca apenas, y el metal responde.\n\n"
        "— No es la primera vez que alguien me saca algo sin que yo elija —dice Isaac, mirándose la "
        "pierna—. Que sea la última."
    },
    [HABILIDAD_CURACION_GRUPAL] = {  // Frieren libera a Isaac y despues a Dante
        "Con Spectre caído, la baliza reacciona por primera vez desde que Frieren la encontró — un "
        "pulso leve que apunta hacia una de las jaulas del asentamiento.\n\n"
        "Adentro, un chico con el brazo metálico cubierto de tierra, que no suelta unas dagas sin "
        "filo, como si todavía esperara que sirvieran para algo. La baliza responde antes de que "
        "Frieren llegue a preguntar nada: el mismo brillo que tuvo en las ruinas. El filo vuelve.\n\n"
        "— También te esperaba a vos —dice Frieren, casi para sí misma.",

        "Vergil cae, y con él buena parte del control que la Catedral tenía sobre los suyos. La baliza "
        "guía a Frieren e Isaac hacia las celdas más profundas, donde los conversos guardaban a "
        "quienes todavía se negaban a \"iluminarse\".\n\n"
        "Dante está ahí, con un ojo cubierto por una placa metálica tosca, puesta a la fuerza. El "
        "espadón que llevaba antes de caer prisionero sigue tirado a su lado, sin filo. La baliza lo "
        "toca apenas, y el metal responde — no borra el ojo que le impusieron, pero sí le devuelve el "
        "arma que era suya de verdad.\n\n"
        "— No es la primera vez que alguien pierde algo acá adentro —dice Dante, mirándose la placa en "
        "el ojo—. Que sea la última vez que me lo sacan sin que yo elija."
    }
};

// Dialogo junto a la fogata antes del jefe: Catedral [0], Fundicion [1] y Nucleo [2].
static const char *const TEXTOS_DESCANSO[][3] = {
    [HABILIDAD_CRITICO_GRUPAL] = {
        "La Catedral de Engranajes impone silencio incluso en el descanso. Frieren mira el bastón, "
        "todavía extrañándose de su propio peso.\n\n"
        "— En Raíz no llegué a nadie a tiempo —dice Isaac, casi sin querer decirlo en voz alta—. Salí "
        "corriendo y no elegí bien a quién ayudar primero.\n\n"
        "Frieren no responde enseguida. Cuando lo hace, es corto:\n\n"
        "— Yo elegí. No alcanzó igual.\n\n"
        "No hay mucho más que agregar todavía. Pero algo quedó dicho.",

        "Saben lo que viene: Tiny los espera al fondo, regenerándose de cada golpe que le den. Y "
        "después de Tiny, lo que queda es pelear contra todo lo que ya vencieron antes, camino a "
        "Sefirot.\n\n"
        "— Va a saber cómo peleamos —dice Frieren, sin que haga falta explicar a quién se refiere.\n\n"
        "— Que sepa —responde Dante—. La última vez que alguien nos sorprendió, fue porque estábamos "
        "solos. Ya no lo estamos.\n\n"
        "Isaac mira la baliza, todavía tibia entre sus manos. No tiene respuestas nuevas que dar esta "
        "noche. Pero por primera vez, tampoco las necesita para saber que van a llegar hasta el final.",

        "La baliza, por primera vez desde Raíz, se enciende sin que nadie la toque. No es una visión "
        "fragmentada esta vez — es casi una voz, o lo más parecido a una voz que un objeto de siglos "
        "puede tener.\n\n"
        "No muestra el pasado como antes. Muestra un patrón: cada vez que Sefirot cayó, algo de él "
        "quedó. Un núcleo, un resto, una semilla dormida en algún lugar como este — esperando otro "
        "ciclo, otros diez años, otra Raíz.\n\n"
        "— Por eso vuelve —dice Frieren, entendiendo antes que los demás—. Nunca lo destruyeron del "
        "todo. Solo lo hicieron dormir.\n\n"
        "Isaac mira el túnel que sigue bajando, hacia el Núcleo. — Entonces hoy no alcanza con ganar.\n\n"
        "— Hoy hay que asegurarse de que no quede nada para el próximo ciclo —dice Dante."
    },
    [HABILIDAD_DEFENSA_GRUPAL] = {
        "La Catedral impone silencio incluso en el descanso. Dante se mira las manos, otra vez.\n\n"
        "— Yo estaba en la puerta —dice, sin que nadie se lo pregunte—. Se suponía que esa puerta no "
        "cedía nunca.\n\n"
        "— No cediste vos —responde Frieren—. Cedió la puerta.\n\n"
        "— Da lo mismo, si yo estaba ahí para sostenerla.\n\n"
        "Frieren no insiste. Después de un silencio, agrega algo propio: — Yo elegí a quién curar "
        "primero esa noche. No alcanzó igual.\n\n"
        "No hay mucho más que agregar todavía. Pero algo quedó dicho.",

        "— Va a saber cómo peleamos —dice Frieren.\n\n"
        "— Que sepa —responde Isaac—. La última vez que alguien nos sorprendió, fue porque estábamos "
        "solos. Ya no lo estamos.\n\n"
        "Dante mira la baliza en sus manos, todavía tibia. No tiene respuestas nuevas esta noche. Pero "
        "tampoco las necesita para saber que van a llegar hasta el final.",

        "La baliza, por primera vez desde Raíz, se enciende sin que nadie la toque — esta vez en manos "
        "de Dante. No muestra el pasado como antes. Muestra un patrón: cada vez que Sefirot cayó, algo "
        "de él quedó. Un núcleo, un resto, una semilla dormida en algún lugar como este.\n\n"
        "— Por eso vuelve —dice Frieren, entendiendo antes que los demás—. Nunca lo destruyeron del "
        "todo. Solo lo hicieron dormir.\n\n"
        "Isaac mira el túnel que sigue bajando. — Entonces hoy no alcanza con ganar.\n\n"
        "— Hoy hay que asegurarse de que no quede nada para el próximo ciclo —dice Dante, cerrando la "
        "mano sobre la baliza."
    },
    [HABILIDAD_CURACION_GRUPAL] = {
        "La Catedral impone silencio incluso en el descanso. Isaac aprieta las dagas, otra vez.\n\n"
        "— En Raíz se me fue gente de al lado corriendo. No elegí bien a quién ayudar primero.\n\n"
        "— No elegiste mal —responde Frieren—. Nadie elige bien esa noche.\n\n"
        "— Yo elegí a quién curar primero esa noche —agrega ella, después de un silencio—. No alcanzó "
        "igual.\n\n"
        "No hay mucho más que agregar todavía. Pero algo quedó dicho.",

        "— Va a saber cómo peleamos —dice Isaac.\n\n"
        "— Que sepa —responde Dante—. La última vez que alguien nos sorprendió, fue porque estábamos "
        "solos. Ya no lo estamos.\n\n"
        "Frieren mira la baliza en sus manos, todavía tibia. No tiene respuestas nuevas esta noche. "
        "Pero tampoco las necesita para saber que van a llegar hasta el final.",

        "La baliza, por primera vez desde Raíz, se enciende sin que nadie la toque — esta vez en manos "
        "de Frieren. No muestra el pasado como antes. Muestra un patrón: cada vez que Sefirot cayó, "
        "algo de él quedó. Un núcleo, un resto, una semilla dormida en algún lugar como este.\n\n"
        "— Por eso vuelve —dice Frieren, casi para sí misma—. Nunca lo destruyeron del todo. Solo lo "
        "hicieron dormir.\n\n"
        "Isaac mira el túnel que sigue bajando. — Entonces hoy no alcanza con ganar.\n\n"
        "— Hoy hay que asegurarse de que no quede nada para el próximo ciclo —dice Dante."
    }
};

// La caida de Sefirot y la destruccion del Nucleo.
static const char *const TEXTOS_CAIDA_SEFIROT[] = {
    [HABILIDAD_CRITICO_GRUPAL] =
        "Sefirot no explota, no grita, no tiene un último discurso. Simplemente se apaga — como algo "
        "que finalmente se queda sin energía para seguir fingiendo que estaba vivo.\n\n"
        "El silencio que sigue dura poco. Esta vez no hay tiempo para preguntarse qué se hace después "
        "de una guerra.\n\n"
        "— Hay que terminar esto de verdad —dice Isaac, ya moviéndose hacia los núcleos secundarios "
        "que todavía zumban alrededor del cuerpo caído de Sefirot.\n\n"
        "No pelean para salir. Pelean para que no quede nada — cada núcleo secundario, cada sistema que "
        "todavía respira, cada resto que en otro ciclo hubiera bastado para volver a empezar. La "
        "Fundición entera empieza a temblar antes de que lleguen a la salida.\n\n"
        "Corren los tres juntos, con el techo del Núcleo cayendo detrás de ellos como si la Tierra "
        "misma quisiera asegurarse de que esta vez sí fuera definitivo.",
    [HABILIDAD_DEFENSA_GRUPAL] =
        "Sefirot se apaga sin discursos, sin gritos, como algo que se queda sin energía para seguir "
        "fingiendo que estaba vivo.\n\n"
        "— Hay que terminar esto de verdad —dice Dante, ya moviéndose hacia los núcleos secundarios que "
        "todavía zumban alrededor del cuerpo caído.\n\n"
        "No pelean para salir. Pelean para que no quede nada — cada núcleo, cada sistema que todavía "
        "respira. La Fundición entera empieza a temblar antes de que lleguen a la salida. Corren los "
        "tres juntos, con el techo cayendo detrás de ellos.",
    [HABILIDAD_CURACION_GRUPAL] =
        "Sefirot se apaga sin discursos, sin gritos, como algo que se queda sin energía para seguir "
        "fingiendo que estaba vivo.\n\n"
        "— Hay que terminar esto de verdad —dice Frieren, ya moviéndose hacia los núcleos secundarios "
        "que todavía zumban alrededor del cuerpo caído.\n\n"
        "No pelean para salir. Pelean para que no quede nada — cada núcleo, cada sistema que todavía "
        "respira. La Fundición entera empieza a temblar antes de que lleguen a la salida. Corren los "
        "tres juntos, con el techo cayendo detrás de ellos."
};

static const char *const TEXTOS_EPILOGO[] = {
    [HABILIDAD_CRITICO_GRUPAL] =
        "Arriba, la explosión se ve incluso desde lejos — una columna de humo donde antes había una "
        "montaña. Isaac no la mira con miedo. La mira como quien por fin ve algo terminado.\n\n"
        "— ¿Vos creés que esto ya se acabó de verdad? —pregunta Dante.\n\n"
        "Isaac saca la baliza del bolsillo. Ya no brilla. Por primera vez desde que la encontró, se "
        "siente como lo que es: un objeto viejo, agotado, sin nada más que mostrar.\n\n"
        "— Antes, alguien solo lo venció —dice—. Nosotros no dejamos nada para que vuelva.\n\n"
        "Frieren mira el cielo, todavía gris, pero sin nada que los esté cazando mientras caminan de "
        "vuelta hacia Raíz.\n\n"
        "— Entonces esta vez, el ciclo se rompe acá.",
    [HABILIDAD_DEFENSA_GRUPAL] =
        "Arriba, la explosión se ve incluso desde lejos. Isaac la mira sin miedo, como quien por fin ve "
        "algo terminado.\n\n"
        "— ¿Vos creés que esto ya se acabó de verdad? —pregunta.\n\n"
        "Dante saca la baliza del bolsillo. Ya no brilla.\n\n"
        "— Antes, alguien solo lo venció —dice—. Nosotros no dejamos nada para que vuelva.\n\n"
        "Frieren mira el cielo, todavía gris, pero sin nada que los esté cazando mientras caminan de "
        "vuelta hacia Raíz.\n\n"
        "— Entonces esta vez, el ciclo se rompe acá.",
    [HABILIDAD_CURACION_GRUPAL] =
        "Arriba, la explosión se ve incluso desde lejos. Dante la mira sin miedo, como quien por fin ve "
        "algo terminado.\n\n"
        "— ¿Vos creés que esto ya se acabó de verdad? —pregunta.\n\n"
        "Frieren saca la baliza del bolsillo. Ya no brilla.\n\n"
        "— Antes, alguien solo lo venció —dice—. Nosotros no dejamos nada para que vuelva.\n\n"
        "Isaac mira el cielo, todavía gris, pero sin nada que los esté cazando mientras caminan de "
        "vuelta hacia Raíz.\n\n"
        "— Entonces esta vez, el ciclo se rompe acá."
};

void mostrarLiberacionAliado(const Personaje_t *protagonista_ptr, const Personaje_t *aliado_ptr,
                             int numeroZona) {
    if (numeroZona < 1 || numeroZona > 2) {
        return;
    }
    mostrarTextoEnCaja(NULL, TEXTOS_LIBERACION[protagonista_ptr->habilidadEspecial][numeroZona - 1],
                       COLOR_BLANCO);
    pausar();

    char textoNarrativa[300];
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

void mostrarDescansoZona(const Personaje_t *protagonista_ptr, int numeroZona) {
    if (numeroZona < 2 || numeroZona > 4) {
        return;   // Campo Ceniza no tiene dialogo: el protagonista esta solo
    }
    mostrarTextoEnCaja(numeroZona == 4 ? "EL ULTIMO RESPIRO" : "JUNTO AL FUEGO",
                       TEXTOS_DESCANSO[protagonista_ptr->habilidadEspecial][numeroZona - 2],
                       COLOR_BLANCO);
    pausar();
}

void mostrarCaidaSefirot(const Personaje_t *protagonista_ptr) {
    limpiarPantallaTerminal();
    mostrarTextoEnCaja("LA CAIDA DE SEFIROT", TEXTOS_CAIDA_SEFIROT[protagonista_ptr->habilidadEspecial],
                       COLOR_BLANCO);
    pausar();
}

void mostrarEpilogo(const Personaje_t *protagonista_ptr) {
    limpiarPantallaTerminal();
    mostrarTextoEnCaja("EPILOGO", TEXTOS_EPILOGO[protagonista_ptr->habilidadEspecial], COLOR_BLANCO);
    pausar();
}

void mostrarFinDelJuego(void) {
    limpiarPantallaTerminal();
    mostrarTextoEnCajaConTituloEstilizado("FIN",
        "El ciclo se rompio. Gracias por jugar Reboot C.",
        COLOR_BLANCO, COLOR_AMARILLO, ESTILO_NEGRITA);
}

void eventoEntreFases(Personaje_t equipo_ptr[], int cantPersonajes, Personaje_t *personaje_ptr,
                      Inventario_t *inventario_ptr) {
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
            agregarRecompensa(inventario_ptr, ITEM_VENENO, 10);
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
                case 3: {
                    for (int i = 0; i < cantPersonajes; i++) {
                        equipo_ptr[i].vidaMax += 10;
                        if (equipo_ptr[i].vida > 0) {
                            equipo_ptr[i].vida += 10;
                        }
                        equipo_ptr[i].ataque += 10;
                        equipo_ptr[i].defensa += 10;
                    }
                    snprintf(textoEvento, sizeof(textoEvento),
                             "Un pequeno robot de apoyo emerge de entre los "
                             "escombros. \"Sobreviviente detectado. Preparando "
                             "asistencia.\" Calibra y refuerza a todo el equipo.\n"
                             "Cada integrante recibe +10 de vida maxima, ataque y defensa. "
                             "Los que siguen en pie recuperan ademas 10 de vida.");
                    break;
                }
            }
            mostrarTextoEnCaja(NULL, textoEvento, COLOR_BLANCO);
            break;
    }

    pausar();

}