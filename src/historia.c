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
    printf("Año 2526. La humanidad lleva cincuenta anios combatiendo a las maquinas.\n");
    printf("Cincuenta anios desde que la superficie dejo de ser un lugar seguro para vivir.\n\n");
    printf("Los pocos que sobrevivieron encontraron refugio bajo tierra, en los tuneles\n");
    printf("de Raiz, el ultimo asentamiento humano del que se tiene noticia.\n");
    pausar();

    // --- Bloque 2 ---
    printf("\nEsta noche, esa certeza tambien se rompio.\n\n");
    printf("Las alarmas de Raiz no suenan desde hace anios. Hoy suenan, y no dejan\n");
    printf("de sonar. Las maquinas encontraron la entrada.\n");
    printf("Comienza una batalla para defender raiz, \n");
    printf("pero rapidamente lo humanos pierden\n");
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
                   "que ya no golpea solo por si mismo -- puede hacer que todo el equipo\n"
                   "golpee mas certero.\n";
        case HABILIDAD_DEFENSA_GRUPAL:
            return "La lanza vieja, curtida de tantos entrenamientos sin gracia, ahora\n"
                   "responde con un peso distinto. Dante siente que puede interponerla\n"
                   "no solo entre un golpe y el, sino entre un golpe y todo el que tenga\n"
                   "cerca.\n";
        case HABILIDAD_CURACION_GRUPAL:
            return "El baston desgastado, el mismo que cargo siempre sin saber por que,\n"
                   "empieza a vibrar con algo que no sabe nombrar todavia, pero que\n"
                   "reconoce: ya no es solo un baston. Es algo hecho para sostener a\n"
                   "los demas.\n";
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
    printf("\nEn el caos de la batalla, %s queda separado del resto. No hay\n",
           personaje->nombre);
    printf("tiempo para buscar a nadie: solo para correr, y esperar volver a\n");
    printf("encontrarlos con vida.\n");
    pausar();

    // --- Bloque 4: el hallazgo del arma comun + la baliza ---
    printf("\nLas ruinas cercanas a Raiz no son ruinas cualquiera. %s todavia lleva su\n",
           personaje->nombre);
    printf("%s  la misma que entreno dentro del refugio, gastada, sin nada de\n",
           personaje->arma);
    printf("especial. La unica que tuvo a mano cuando sonaron las alarmas.\n\n");
    printf("Entre los escombros, algo distinto: un dispositivo pequenio, cubierto\n");
    printf("de polvo de siglos. Al tocarlo, se enciende. No dice mucho, todavia.\n");
    printf("Pero es suficiente para saber que esta guerra ya paso antes.\n");
    pausar();

    // --- Bloque 5: la transformacion del arma ---
    printf("\nUn pulso de luz salta de la baliza directo hacia %s. Por un instante,\n",
           personaje->arma);
    printf("%s ve fragmentos que no son suyos: manos distintas, sosteniendo la\n",
           personaje->nombre);
    printf("misma arma -ya transformada- en batallas que no vivio. Cuando la\n");
    printf("vision termina, %s ya no es la misma: el metal comun dejo lugar a\n",
           personaje->arma);
    printf("algo que reconoce sin haberlo visto nunca.\n\n");

    // Texto especifico de la habilidad que desbloquea cada personaje.
    printf("%s", textoHabilidadDesbloqueada(personaje->habilidadEspecial));
    pausar();
}