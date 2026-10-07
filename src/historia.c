#include <stdio.h>
#include <string.h>
#include "../include/historia.h"
#include "../include/utils.h"

Zona crearZona(const char *nombre, const char *descripcion, int nivel) {
    Zona zona;
    strncpy(zona.nombre, nombre, sizeof(zona.nombre) - 1);
    zona.nombre[sizeof(zona.nombre) - 1] = '\0';
    strncpy(zona.descripcion, descripcion, sizeof(zona.descripcion) - 1);
    zona.descripcion[sizeof(zona.descripcion) - 1] = '\0';
    zona.nivel = nivel;
    return zona;
}

void mostrarNarrativaZona(const Zona *zona) {
    // TODO: sumar mas texto narrativo y transiciones entre zonas.
    printf("== %s (nivel %d) ==\n%s\n", zona->nombre, zona->nivel, zona->descripcion);
}

// Bloques 1 y 2: contexto del mundo y el ataque a Raiz.
// No recibe parametros porque este texto es igual sin importar que
// personaje vaya a elegir el jugador despues.
void mostrarIntroduccionMundo(void) {
    printf("\n===================================\n");
    printf("              REBOOT C\n");
    printf("===================================\n\n");

    // --- Bloque 1 ---
    printf("Año 2076. La humanidad lleva medio siglo combatiendo contra  las maquinas.\n");
    printf("Medio siglo desde que la superficie dejo de ser un lugar seguro para vivir.\n\n");

    printf("Los pocos que sobrevivieron encontraron refugio bajo tierra, en los tuneles\n");
    printf("de La Raiz, el ultimo asentamiento humano del que se tiene noticia.\n");
    printf("Alli abajo, creian estar a salvo.\n");
    pausar();

    // --- Bloque 2 ---
    printf("\nEsta noche, esa certeza tambien se rompio.\n\n");
    printf("Las alarmas de La Raiz, enmudecidas durante decadas, han despertado.\n");
    printf("Hoy chillan, y no dejan de sonar. Las maquinas han logrado infiltrarse.\n\n");;
    printf("Comienza una batalla desesperada para defender los tuneles,\n");
    printf("pero la resistencia humana es aplastada casi de inmediato.\n");
    printf("El acero y el fuego arrasan con las defensas. \n");
    printf("Algunos son masacrados. Los demas... son arrastrados a la oscuridad como prisioneros.\n");
    pausar();
}

// Devuelve el texto de la habilidad especial que desbloquea cada personaje
// al transformar su arma. Recibe directamente el TipoHabilidadEspecial (el
// dato ya vive en la struct Personaje, seteado desde crearPersonaje), asi
// que no hace falta comparar nombres de personaje para saber cual es.
static const char *textoHabilidadDesbloqueada(TipoHabilidadEspecial habilidad) {
    switch (habilidad) {
        case HABILIDAD_CRITICO_GRUPAL:
            return "El filo, gastado hasta hace un momento, ahora tiene un borde que no\n"
                   "parece de este siglo. Isaac entiende, sin que nadie se lo explique,\n"
                   "que su instinto asesino se ha afilado: su proximo golpe sera letal,\n"
                   "y cuando encuentre a los demas, podra guiarlos para hacer lo mismo.\n";
        case HABILIDAD_DEFENSA_GRUPAL:
            return "La lanza vieja, curtida de tantos entrenamientos sin gracia, ahora\n"
                   "responde con un peso distinto. Dante siente que su guardia es\n"
                   "impenetrable, un escudo absoluto capaz de protegerlo a el hoy,\n"
                   "y a todo su equipo mañana.\n";
        case HABILIDAD_CURACION_GRUPAL:
            return "El baston desgastado, el mismo que cargo siempre sin saber por que,\n"
                   "empieza a vibrar con una calidez que no sabe nombrar todavia, pero que\n"
                   "reconoce. Es magia antigua, pura vida fluyendo por sus manos para\n"
                   "sanar sus propias heridas y sostener a los que aun lo necesiten.\n";
        case HABILIDAD_NINGUNA:
        default:
            // Caso general: por si en el futuro se agrega un personaje sin
            // una habilidad especifica todavia asignada.
            return "Algo cambio. Ahora sabe usarla de verdad.\n";
    }
}

// Bloques 3, 4 y 5: la separacion, el hallazgo del arma comun junto a la
// baliza, y la transformacion del arma segun el personaje elegido.
void mostrarNarrativaInicial(const Personaje *personaje) {
    // --- Bloque 3: la separacion ---
    printf("\nEn el caos de la batalla, %s queda separado del resto.\n",
           personaje->nombre);
    printf("No hay tiempo para ayudar a nadie: solo para correr, y esperar a encontrar otros sobrevivientes\n");
    printf("el sonido de los engranajes y el fuego enemigo lo obligan a buscar refugio entre las ruinas.\n");
    pausar();

    // --- Bloque 4: el hallazgo del arma comun + la baliza ---
    printf("\n\nAl adentrarse en la oscuridad, la respiracion de %s se detiene.\n",
            personaje->nombre);
    printf("Frente a el se alza una enorme compuerta de metal oscuro, cubierta de\n");
    printf("inscripciones de una era olvidada. La puerta cede con un gemido pesado...\n\n");
    printf("En el centro de la sala, un pilar monolitico pulsa con una luz violeta.\n");
    printf("%s Al acercarse y apoyar su mano, el pilar estalla en un relampago silencioso.\n",
            personaje->nombre);
    printf("Visiones invaden la mente de %s:\n\n", personaje->nombre);

    printf("  > Ciudades enteras consumidas por el fuego...\n");
    printf("  > Ejercitos antiguos cayendo ante legiones de maquinas...\n");
    printf("  > Y en el centro de la masacre, un guerrero empunando tu %s,\n", personaje->arma);
    printf("    pero brillando con un poder absoluto y devastador.\n\n");
    
    printf("Las visiones colapsan de golpe dejandote sin aliento.\n");
    pausar();
    printf("\nEl pilar libera un latido de energia pura que impacta de lleno en tu arma.\n");
    printf("%s observa como su %s comienza a transformarse y a parecerse a el arma que le mostro la baliza\n",
            personaje->nombre, personaje->arma);
    printf("El acero oxidado de la vieja arma tembló\n");
    printf("El óxido y la sangre seca se desprendieron en el aire como cenizas arrastradas por un viento invisible, \n");
    printf("revelando una superficie que ya no era hierro, sino un cristal oscuro y perfecto que devoraba la luz del entorno.\n");
    printf("El peso del arma cambió drásticamente, volviéndose ligera como una pluma pero anclada al brazo como una extensión misma de su alma\n");
    printf("Las muescas de mil batallas pasadas se fundieron, retorciéndose hasta formar runas grabadas a fuego blanco a lo largo del arma\n\n");

    printf("La sala entera vibra por el zumbido del arma despertando.\n");
    printf("Una leyenda acaba de renacer.\n");
    pausar();

    printf("Arma conseguida  %s!! \n", personaje->arma);

    // Texto especifico de la habilidad que desbloquea cada personaje.
    printf("%s", textoHabilidadDesbloqueada(personaje->habilidadEspecial));
    pausar();
}

// La baliza empieza a guiar el camino: se acaba la posibilidad de volver a
// Raiz, y arranca el viaje hacia la primera zona. Depende del nombre del
// personaje, por eso recibe el puntero.
void mostrarSalidaDeRaiz(const Personaje *personaje) {
    printf("\nCon el arma transformada en su mano, %s siente algo mas: la baliza,\n",
           personaje->nombre);
    printf("antes silenciosa, ahora vibra con un pulso constante, como si supiera\n");
    printf("exactamente hacia donde hay que ir.\n\n");
    printf("un pequeño pedazo de la baliza se desprende y se posa en tu mano,\n");
    printf("ahora esta pequeña baliza proyecta una luz tenua hacia la salida de las ruinas,\n");
    printf("marcando un camino que %s no habia visto antes.\n", personaje->nombre);
    pausar();

    printf("\nRegresar a Raiz ya no es una opcion. Lo que quedaba del refugio esta\n");
    printf("en manos de las maquinas, y no hay señal de que nadie mas haya sobrevivido...\n");
    printf("todavia.\n\n");
    printf("%s respira hondo, aferra el arma, y sigue la luz de la baliza hacia\n",
           personaje->nombre);
    printf("la superficie. El viaje recien empieza.\n");
    pausar();
}

// Muestra el "cartel" de entrada a una zona nueva, usando directamente los
// datos de la struct Zona (nombre y descripcion) en vez de textos sueltos
// escritos a mano en cada lugar donde se entra a una zona.
void mostrarEntradaZona(const Zona *zona) {
    printf("\n=======================================\n");
    printf("  ENTRASTE A LA ZONA: %s\n", zona->nombre);
    printf("=========================================\n\n");
    printf("%s\n", zona->descripcion);
    pausar();
}



// Escena corta de transicion justo antes de un combate.
void mostrarEncuentroEnemigos(const Personaje *personaje, const char *amenaza) {
    printf("\nMientras %s avanza entre las ruinas del pueblo, un sonido metalico y extraño\n",
           personaje->nombre);
    printf("rompe el silencio. No hay tiempo para pensar: %s\n", amenaza);
    pausar();
}


void mostrarPostBatalla (const Personaje *personaje) {

    printf ("Luego de la batalla, %s topma un descanso y sigue avanzando...\n",
        personaje->nombre);
    printf ("sabiendo que adelante lo esperan muchas mas amenazas\n");
}