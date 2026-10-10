#include <stdio.h>
#include <string.h>
#include "../include/historia.h"
#include "../include/utils.h"

Zona crearZona(const char *nombre, const char *descripcion) {
    Zona zona;
    strncpy(zona.nombre, nombre, sizeof(zona.nombre) - 1);
    zona.nombre[sizeof(zona.nombre) - 1] = '\0';
    strncpy(zona.descripcion, descripcion, sizeof(zona.descripcion) - 1);
    zona.descripcion[sizeof(zona.descripcion) - 1] = '\0';
    zona.cantFases = 0;
    return zona;
}

int agregarFase(Zona *zona, const char *amenaza, const Enemigo enemigos[], int cantEnemigos, int fogataAntes) {
    if (zona->cantFases >= MAX_FASES_ZONA || cantEnemigos > MAX_ENEMIGOS_FASE) {
        return 0;
    }
    Fase *fase = &zona->fases[zona->cantFases];
    strncpy(fase->amenaza, amenaza, sizeof(fase->amenaza) - 1);
    fase->amenaza[sizeof(fase->amenaza) - 1] = '\0';
    for (int i = 0; i < cantEnemigos; i++) {
        fase->enemigos[i] = enemigos[i];
    }
    fase->cantEnemigos = cantEnemigos;
    fase->fogataAntes = fogataAntes;
    zona->cantFases++;
    return 1;
}

Zona crearZonaCampoCeniza(void) {
    Zona zona = crearZona(
        "Campo Ceniza",
        "Lo que fue un pueblo agricola es hoy tierra quemada, vigilada por\n"
        "cazadores al servicio de las maquinas y sus perros mecanicos."
    );

    Enemigo centinela = crearEnemigo("Centinela Canino", HABILIDAD_ENEMIGO_NINGUNA, 120, 8, 48);
    Enemigo cazador = crearEnemigo("Cazador de Campo Ceniza", HABILIDAD_ENEMIGO_TRAMPA, 160, 16, 56);
    Enemigo spectre = crearEnemigo("Spectre", HABILIDAD_ENEMIGO_CRITICO, 280, 40, 60);

    Enemigo fase1[] = { centinela, centinela };
    agregarFase(&zona, "dos perros mecanicos emergen entre la ceniza y corren directo hacia vos.",
                fase1, 2, 0);

    Enemigo fase2[] = { cazador };
    agregarFase(&zona, "un cazador sale de entre los restos de un granero y te apunta.",
                fase2, 1, 0);

    Enemigo fase3[] = { cazador, centinela, centinela };
    agregarFase(&zona, "otro cazador aparece, esta vez acompañado por sus perros.",
                fase3, 3, 0);

    // Fase del jefe: antes se descansa en una fogata.
    Enemigo fase4[] = { cazador, spectre };
    agregarFase(&zona, "una figura enorme avanza detras de un cazador: es Spectre.",
                fase4, 2, 1);

    return zona;
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

void eventoEntreFases(Personaje *personaje) {
    printf("\n%s sigue avanzando entre las ruinas. Algo llama su atencion...\n",
           personaje->nombre);

    switch (numeroAleatorio(0, 10)) {
        case 0:
        case 1:
            printf("No era nada. El camino sigue en silencio y continuas avanzando.\n");
            break;
        case 2:
        case 3:
            printf("Entre los restos encuentras una pocion de curacion.\n");
            printf("La guardas mentalmente para mas adelante; por ahora no puedes llevarla.\n");
            break;
        case 4:
        case 5:
            printf("Descubres un frasco de veneno entre los escombros.\n");
            printf("Podria servir contra una maquina, aunque todavia no puedes guardarlo.\n");
            break;
        case 6: {
            Estado veneno = { ESTADO_VENENO, 2, 10 };
            aplicarEstadoPersonaje(personaje, veneno);
            printf("Una nube toxica se libera al mover los escombros y te alcanza.\n");
            printf("Quedas envenenado: perderas 10 de vida durante 2 turnos.\n");
            break;
        }
        case 7:
            if (personaje->estadoActivo.tipo == ESTADO_VENENO) {
                personaje->estadoActivo.tipo = ESTADO_NINGUNO;
                personaje->estadoActivo.duracionTurnos = 0;
                personaje->estadoActivo.magnitud = 0;
                printf("Encuentras una planta medicinal y la aplicas sobre la herida.\n");
                printf("El veneno deja de afectarte.\n");
            } else {
                printf("Encuentras una planta medicinal, pero no tienes veneno que curar.\n");
            }
            break;
        case 8:
        case 9:
        case 10:
            printf("Un pequeno robot de apoyo emerge de entre los escombros.\n");
            printf("\"Sobreviviente detectado. Preparando asistencia.\"\n");

            switch (numeroAleatorio(1, 3)) {
                case 1:
                    printf("El robot te ofrece dos pociones de curacion.\n");
                    printf("Aun no puedes guardarlas, pero sabes donde encontrarlas.\n");
                    break;
                case 2:
                    printf("El robot te ofrece dos frascos de veneno para tus armas.\n");
                    printf("Aun no puedes guardarlos, pero sabes donde encontrarlos.\n");
                    break;
                case 3:
                    personaje->vidaMax += 10;
                    personaje->vida += 10;
                    personaje->ataque += 10;
                    personaje->defensa += 10;
                    printf("El robot calibra tu equipo y refuerza tu cuerpo.\n");
                    printf("Vida maxima +10, ataque +10 y defensa +10.\n");
                    printf("Ahora tienes %d/%d de vida, %d de ataque y %d de defensa.\n",
                           personaje->vida, personaje->vidaMax,
                           personaje->ataque, personaje->defensa);
                    break;
            }
            break;
    }

    pausar();

}