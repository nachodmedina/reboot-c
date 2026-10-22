#include <stdio.h>
#include <string.h>
#include "../include/historia.h"
#include "../include/utils.h"

static void agregarRecompensa(Inventario_t *inventario_ptr, TipoItem tipo, int valor) {
    Item_t item = { tipo, valor };
    if (!agregarItem(inventario_ptr, item)) {
        printf("La bolsa esta llena; no puedes guardar ese consumible.\n");
    }
}

Zona_t crearZona(const char *nombre_ptr, const char *descripcion_ptr) {
    Zona_t zona;
    strncpy(zona.nombre, nombre_ptr, sizeof(zona.nombre) - 1);
    zona.nombre[sizeof(zona.nombre) - 1] = '\0';
    strncpy(zona.descripcion, descripcion_ptr, sizeof(zona.descripcion) - 1);
    zona.descripcion[sizeof(zona.descripcion) - 1] = '\0';
    zona.cantFases = 0;
    return zona;
}

int agregarFase(Zona_t *zona_ptr, const char *amenaza_ptr, const Enemigo_t enemigos_ptr[], int cantEnemigos, int fogataAntes) {
    if (zona_ptr->cantFases >= MAX_FASES_ZONA || cantEnemigos > MAX_ENEMIGOS_FASE) {
        return 0;
    }
    Fase_t *fase_ptr = &zona_ptr->fases[zona_ptr->cantFases];
    strncpy(fase_ptr->amenaza, amenaza_ptr, sizeof(fase_ptr->amenaza) - 1);
    fase_ptr->amenaza[sizeof(fase_ptr->amenaza) - 1] = '\0';
    for (int i = 0; i < cantEnemigos; i++) {
        fase_ptr->enemigos[i] = enemigos_ptr[i];
    }
    fase_ptr->cantEnemigos = cantEnemigos;
    fase_ptr->fogataAntes = fogataAntes;
    zona_ptr->cantFases++;
    return 1;
}

Zona_t crearZonaCampoCeniza(void) {
    Zona_t zona = crearZona(
        "Campo Ceniza",
        "Lo que fue un pueblo agricola es hoy tierra quemada, vigilada por\n"
        "cazadores al servicio de las maquinas y sus perros mecanicos."
    );

    Enemigo_t centinela = crearEnemigo("Centinela Canino", HABILIDAD_ENEMIGO_NINGUNA, 120, 8, 48);
    Enemigo_t cazador = crearEnemigo("Cazador de Campo Ceniza", HABILIDAD_ENEMIGO_TRAMPA, 160, 16, 56);
    Enemigo_t spectre = crearEnemigo("Spectre", HABILIDAD_ENEMIGO_CRITICO, 280, 40, 60);

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
// dato ya vive en la struct Personaje_t, seteado desde crearPersonaje), asi
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
void mostrarNarrativaInicial(const Personaje_t *personaje_ptr) {
    // --- Bloque 3: la separacion ---
    printf("\nEn el caos de la batalla, %s queda separado del resto.\n",
           personaje_ptr->nombre);
    printf("No hay tiempo para ayudar a nadie: solo para correr, y esperar a encontrar otros sobrevivientes\n");
    printf("el sonido de los engranajes y el fuego enemigo lo obligan a buscar refugio entre las ruinas.\n");
    pausar();

    // --- Bloque 4: el hallazgo del arma comun + la baliza ---
    printf("\n\nAl adentrarse en la oscuridad, la respiracion de %s se detiene.\n",
            personaje_ptr->nombre);
    printf("Frente a el se alza una enorme compuerta de metal oscuro, cubierta de\n");
    printf("inscripciones de una era olvidada. La puerta cede con un gemido pesado...\n\n");
    printf("En el centro de la sala, un pilar monolitico pulsa con una luz violeta.\n");
    printf("%s Al acercarse y apoyar su mano, el pilar estalla en un relampago silencioso.\n",
            personaje_ptr->nombre);
    printf("Visiones invaden la mente de %s:\n\n", personaje_ptr->nombre);

    printf("  > Ciudades enteras consumidas por el fuego...\n");
    printf("  > Ejercitos antiguos cayendo ante legiones de maquinas...\n");
    printf("  > Y en el centro de la masacre, un guerrero empunando tu %s,\n", personaje_ptr->arma);
    printf("    pero brillando con un poder absoluto y devastador.\n\n");
    
    printf("Las visiones colapsan de golpe dejandote sin aliento.\n");
    pausar();
    printf("\nEl pilar libera un latido de energia pura que impacta de lleno en tu arma.\n");
    printf("%s observa como su %s comienza a transformarse y a parecerse a el arma que le mostro la baliza\n",
            personaje_ptr->nombre, personaje_ptr->arma);
    printf("El acero oxidado de la vieja arma tembló\n");
    printf("El óxido y la sangre seca se desprendieron en el aire como cenizas arrastradas por un viento invisible, \n");
    printf("revelando una superficie que ya no era hierro, sino un cristal oscuro y perfecto que devoraba la luz del entorno.\n");
    printf("El peso del arma cambió drásticamente, volviéndose ligera como una pluma pero anclada al brazo como una extensión misma de su alma\n");
    printf("Las muescas de mil batallas pasadas se fundieron, retorciéndose hasta formar runas grabadas a fuego blanco a lo largo del arma\n\n");

    printf("La sala entera vibra por el zumbido del arma despertando.\n");
    printf("Una leyenda acaba de renacer.\n");
    pausar();

    printf("Arma conseguida  %s!! \n", personaje_ptr->arma);

    // Texto especifico de la habilidad que desbloquea cada personaje.
    printf("%s", textoHabilidadDesbloqueada(personaje_ptr->habilidadEspecial));
    pausar();
}

// La baliza empieza a guiar el camino: se acaba la posibilidad de volver a
// Raiz, y arranca el viaje hacia la primera zona. Depende del nombre del
// personaje, por eso recibe el puntero.
void mostrarSalidaDeRaiz(const Personaje_t *personaje_ptr) {
    printf("\nCon el arma transformada en su mano, %s siente algo mas: la baliza,\n",
           personaje_ptr->nombre);
    printf("antes silenciosa, ahora vibra con un pulso constante, como si supiera\n");
    printf("exactamente hacia donde hay que ir.\n\n");
    printf("un pequeño pedazo de la baliza se desprende y se posa en tu mano,\n");
    printf("ahora esta pequeña baliza proyecta una luz tenua hacia la salida de las ruinas,\n");
    printf("marcando un camino que %s no habia visto antes.\n", personaje_ptr->nombre);
    pausar();

    printf("\nRegresar a Raiz ya no es una opcion. Lo que quedaba del refugio esta\n");
    printf("en manos de las maquinas, y no hay señal de que nadie mas haya sobrevivido...\n");
    printf("todavia.\n\n");
    printf("%s respira hondo, aferra el arma, y sigue la luz de la baliza hacia\n",
           personaje_ptr->nombre);
    printf("la superficie. El viaje recien empieza.\n");
    pausar();
}

// Muestra el "cartel" de entrada a una zona nueva, usando directamente los
// datos de la struct Zona_t (nombre y descripcion) en vez de textos sueltos
// escritos a mano en cada lugar donde se entra a una zona.
void mostrarEntradaZona(const Zona_t *zona_ptr) {
    printf("\n=======================================\n");
    printf("  ENTRASTE A LA ZONA: %s\n", zona_ptr->nombre);
    printf("=========================================\n\n");
    printf("%s\n", zona_ptr->descripcion);
    pausar();
}



// Escena corta de transicion justo antes de un combate.
void mostrarEncuentroEnemigos(const Personaje_t *personaje_ptr, const char *amenaza_ptr) {
    printf("\nMientras %s avanza entre las ruinas del pueblo, un sonido metalico y extraño\n",
           personaje_ptr->nombre);
    printf("rompe el silencio. No hay tiempo para pensar: %s\n", amenaza_ptr);
    pausar();
}


void mostrarPostBatalla (const Personaje_t *personaje_ptr) {

    printf ("Luego de la batalla, %s toma un descanso y sigue avanzando...\n",
        personaje_ptr->nombre);
    printf ("sabiendo que adelante lo esperan muchas mas amenazas\n");
}

void mostrarReaccionSephirot(const char *atributoEnemigo_ptr, int aumentoAcumulado) {
    printf("\nLa baliza emite un pulso que se extiende por todo el campo de batalla.\n");
    printf("Muy lejos de alli, Sephirot observa una lectura de energia desconocida.\n");
    printf("\"Reconozco esa mejora. Si ellos se fortalecen, mis maquinas tambien.\"\n");
    printf("Los enemigos de las proximas zonas ganaran %d puntos de %s acumulados.\n",
           aumentoAcumulado, atributoEnemigo_ptr);
    pausar();
}

void mostrarLiberacionAliado(const Personaje_t *protagonista_ptr, const Personaje_t *aliado_ptr) {
    printf("\nLa baliza de %s comienza a parpadear y proyecta un camino entre las ruinas.\n",
           protagonista_ptr->nombre);
    printf("Al final del camino, una jaula de energia mantiene prisionero a %s.\n",
           aliado_ptr->nombre);
    printf("%s se acerca y rompe el sello de la jaula.\n", protagonista_ptr->nombre);
    printf("La baliza toca el %s de %s, y su arma empieza a transformarse.\n",
           aliado_ptr->arma, aliado_ptr->nombre);
    printf("%s recupera el aliento: \"Gracias. En la proxima batalla luchare a tu lado.\"\n",
           aliado_ptr->nombre);
    printf("%s se unio a tu aventura.\n", aliado_ptr->nombre);
    pausar();
}

void eventoEntreFases(Personaje_t *personaje_ptr, Inventario_t *inventario_ptr) {
    printf("\n%s sigue avanzando entre las ruinas. Algo llama su atencion...\n",
           personaje_ptr->nombre);

    switch (numeroAleatorio(0, 8)) {
        case 0:
        case 1:
            printf("No era nada. El camino sigue en silencio y continuas avanzando.\n");
            break;
        case 2:
        case 3:
            printf("Entre los restos encuentras una pocion de curacion.\n");
            agregarRecompensa(inventario_ptr, ITEM_VIDA, 100);
            break;
        case 4:
        case 5:
            printf("Descubres un frasco de veneno entre los escombros.\n");
            agregarRecompensa(inventario_ptr, ITEM_VENENO, 5);
            break;
        case 6: {
            Estado_t veneno = { ESTADO_VENENO, 2, 10 };
            aplicarEstadoPersonaje(personaje_ptr, veneno);
            printf("Una nube toxica se libera al mover los escombros y te alcanza.\n");
            printf("Quedas envenenado: perderas 10 de vida durante 2 turnos.\n");
            break;
        }
        case 7:
            if (personaje_ptr->estadoActivo.tipo == ESTADO_VENENO) {
                personaje_ptr->estadoActivo.tipo = ESTADO_NINGUNO;
                personaje_ptr->estadoActivo.duracionTurnos = 0;
                personaje_ptr->estadoActivo.magnitud = 0;
                printf("Encuentras una planta medicinal y la aplicas sobre la herida.\n");
                printf("El veneno deja de afectarte.\n");
            } else {
                printf("Encuentras una planta medicinal, pero no tienes veneno que curar.\n");
            }
            break;
        case 8:
            printf("Un pequeno robot de apoyo emerge de entre los escombros.\n");
            printf("\"Sobreviviente detectado. Preparando asistencia.\"\n");

            switch (numeroAleatorio(1, 3)) {
                case 1:
                    printf("El robot te ofrece dos pociones de curacion.\n");
                    agregarRecompensa(inventario_ptr, ITEM_VIDA, 100);
                    agregarRecompensa(inventario_ptr, ITEM_VIDA, 100);
                    break;
                case 2:
                    printf("El robot te ofrece dos frascos de veneno para tus armas.\n");
                    agregarRecompensa(inventario_ptr, ITEM_VENENO, 5);
                    agregarRecompensa(inventario_ptr, ITEM_VENENO, 5);
                    break;
                case 3:
                    personaje_ptr->vidaMax += 10;
                    personaje_ptr->vida += 10;
                    personaje_ptr->ataque += 10;
                    personaje_ptr->defensa += 10;
                    printf("El robot calibra tu equipo y refuerza tu cuerpo.\n");
                    printf("Vida maxima +10, ataque +10 y defensa +10.\n");
                    printf("Ahora tienes %d/%d de vida, %d de ataque y %d de defensa.\n",
                           personaje_ptr->vida, personaje_ptr->vidaMax,
                           personaje_ptr->ataque, personaje_ptr->defensa);
                    break;
            }
            break;
    }

    pausar();

}