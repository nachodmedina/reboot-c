# Reboot C

Videojuego por turnos en C puro (C11), para consola. Proyecto integrador de Programación I (UTN).

Año 2076. Las máquinas se infiltran en La Raíz, el último refugio humano bajo tierra.
En medio del ataque, uno de los tres protagonistas queda separado del resto, encuentra
una baliza antigua y su arma se transforma, despertando una habilidad especial. Siguiendo
la luz de la baliza, sale a la superficie, libera a sus compañeros zona por zona y baja
hasta El Núcleo para terminar con Sefirot.

## Compilar y correr

```bash
make
./rebootc
```

Tests del motor de combate y simulación de balance de la campaña completa:

```bash
make test
./test_combate 101   # opcional: simular con otras semillas (desde la 101)
```

Para limpiar los binarios generados:

```bash
make clean
```

## Estado actual

El juego compila sin warnings (`-Wall -Wextra -std=c11`) y se juega de punta a punta:
introducción, 4 zonas, Sefirot y epílogo, con textos distintos según el protagonista.

### Flujo de una partida nueva

1. **Menú principal**: Nueva partida / Cargar partida / Salir.
2. **Modo de juego**: Normal o Difícil.
3. **Introducción del mundo** y **elección del protagonista** (Isaac, Dante o Frieren).
4. **Narrativa inicial**: la separación, la baliza y la transformación del arma.
5. **Zona 1: Campo Ceniza**, en solitario. Al vencer a Spectre se libera al primer aliado.
6. **Zona 2: Catedral de Engranajes**, de a dos. Al vencer a Vergil se libera al segundo.
7. **Zona 3: La Fundición**, los tres juntos, contra Tiny.
8. **Zona 4: El Núcleo**: los restos de las zonas anteriores y Sefirot.
9. **La caída de Sefirot, epílogo y FIN.**

Aliados según el protagonista (Dante siempre llega en la Zona 2, salvo que sea el
protagonista):

| Protagonista | Aliado Zona 1 | Aliado Zona 2 |
|--------------|---------------|---------------|
| Isaac        | Frieren       | Dante         |
| Dante        | Frieren       | Isaac         |
| Frieren      | Isaac         | Dante         |

Cada zona tiene 4 combates. El equipo empieza cada zona descansado y antes del jefe hay
una fogata que recupera toda la vida (en las Zonas 2 a 4, con un diálogo entre los
personajes). Entre combates ocurre un **evento aleatorio**: nada, una nube tóxica que
envenena, una planta medicinal, pociones o veneno para la bolsa, o un robot de apoyo.

Después de los jefes de las Zonas 1 a 3 el equipo elige una **mejora** (vida, ataque o
defensa) y Sefirot responde dándoles a los enemigos de las zonas siguientes un
**contrabonus**. La partida se guarda en `partida.dat` al terminar cada zona.

Si el equipo cae en combate, la partida termina. En **Modo Difícil**, además, se borra la
partida guardada (permadeath).

### Personajes

Los tres tienen las mismas stats; cambia la habilidad especial.

| Personaje | Arma                  | Vida | Ataque | Defensa | Habilidad especial                          |
|-----------|-----------------------|------|--------|---------|---------------------------------------------|
| Isaac     | Dagas de Excabilur    | 320  | 80     | 40      | Crítico grupal: el equipo pega x1.2         |
| Dante     | Espadón de Rebellion  | 320  | 80     | 40      | Defensa grupal: +50% de defensa al equipo   |
| Frieren   | Bastón de la Pérdida  | 320  | 80     | 40      | Curación grupal: +40 de vida, dos veces     |

Las habilidades duran 2 rondas y solo puede haber una activa a la vez.

### Combate

Por turnos: primero actúan los personajes y después los enemigos (que eligen a quién
atacar al azar). En cada turno el jugador elige:

1. **Atacar a un enemigo**: daño = `ataque - defensa` (mínimo 0).
2. **Ataque en área**: golpea a todos los enemigos vivos con `ataque / 2 + 15 - defensa`.
3. **Habilidad especial** (ver tabla de personajes).
4. **Usar poción**: de vida (para el que la usa) o de veneno (sobre un enemigo). No gasta
   la acción del turno.

Estados alterados:

- **Veneno**: resta vida al final de cada turno mientras dure.
- **Debilitado**: −25% de ataque. Lo aplica el Cazador con *Trampa*.
- **Aturdido**: pierde el turno. Lo aplica el Gólem.

### Zonas y enemigos

| Zona | Enemigo                 | Tipo       | Vida | Defensa | Ataque | Habilidad                                      |
|------|-------------------------|------------|------|---------|--------|------------------------------------------------|
| 1    | Centinela Canino        | Común      | 120  | 8       | 48     | —                                              |
| 1    | Cazador de Campo Ceniza | Común      | 160  | 16      | 56     | Trampa: 40% de dejar debilitado 1 turno        |
| 1    | **Spectre**             | Jefe       | 280  | 40      | 64     | Crítico: 30% de pegar x1.5                     |
| 2    | Escudero                | Común      | 150  | 24      | 58     | —                                              |
| 2    | Guardián                | Común      | 150  | 20      | 64     | —                                              |
| 2    | **Vergil**              | Jefe       | 380  | 30      | 80     | Recibe la mitad del daño                       |
| 3    | Mutante                 | Común      | 190  | 20      | 82     | —                                              |
| 3    | Gólem                   | Común      | 260  | 30      | 74     | Aturdir: 25% de dejar aturdido 1 turno         |
| 3    | **Tiny**                | Jefe       | 500  | 36      | 86     | Regenera 30 de vida al empezar su turno        |
| 4    | **Sefirot**             | Jefe final | 820  | 42      | 100    | Ataque en área, curación de 80 o invocación    |

Combates (la fogata va antes del cuarto):

| Zona | Fase 1               | Fase 2                | Fase 3                 | Fase 4 (jefe)        |
|------|----------------------|-----------------------|------------------------|----------------------|
| 1    | 2 Centinelas         | Cazador               | Cazador + 2 Centinelas | Cazador + Spectre    |
| 2    | 2 Escuderos          | Escudero + Guardián   | Guardián + 2 Escuderos | 2 Guardianes + Vergil|
| 3    | 2 Mutantes           | Mutante + Gólem       | Gólem + 2 Mutantes     | 2 Gólems + Tiny      |
| 4    | 2 Cazadores + Centinela | Guardián + 2 Escuderos | Gólem + 2 Mutantes | Sefirot              |

En cada turno Sefirot elige al azar: ataque en área a todo el equipo (25%), curarse 80 de
vida (20%), invocar un Cazador, un Guardián o un Mutante (20%, hasta 3 enemigos en pie) o
un ataque normal. Los enemigos de El Núcleo usan las stats de su zona más todos los
contrabonus acumulados.

**Balance**: `make test` simula la campaña completa 50 veces por estrategia de mejora,
en el peor caso (sin habilidades ni pociones). Con "siempre vida", la más exigente, las
Zonas 2 a 4 se ganan en alrededor del 90% de las corridas y el equipo termina con 20% a
35% de vida. Las mejoras de ataque y defensa dejan las zonas más fáciles.

### Pendiente

- **Guardado completo**: hoy se guarda el equipo al terminar cada zona, pero "Cargar
  partida" solo muestra lo guardado. El guardado entre fases con ranuras está en la rama
  `feature/guardado-ranuras`.
- **Stats propias** para cada personaje.

## Presentacion en terminal

En una consola compatible, los menus de seleccion se recorren con las flechas y
se confirman con Enter. Si la entrada no es una consola interactiva, el juego
muestra las mismas opciones numeradas para poder elegirlas escribiendo un numero.
Los colores usan secuencias ANSI: aliados en verde, enemigos en rojo, jefes en
amarillo y el numero de ronda en celeste. Los mensajes de combate resaltan en
negrita a quienes actuan. La pantalla se limpia al comenzar cada ronda.

Las escenas narrativas aparecen en cuadros de doble borde blanco. Los dibujos
ASCII de los personajes y sus armas estan en `src/arte_ascii.c` (`arteIsaac_ptr`,
`arteArmaIsaac_ptr`, etc.):

- Mantene cada linea entre comillas y separada por comas. En una cadena C, una
  barra invertida se escribe `\\` y una comilla doble se escribe `\"`. Respeta
  los espacios iniciales y finales de cada linea para conservar la alineacion.

## Convenciones de nombres C

- Los alias de tipos declarados con `typedef struct` terminan en `_t`
  (`Personaje_t`, `Inventario_t`).
- Los nombres de variables y parametros puntero terminan en `_ptr`
  (`personaje_ptr`, `archivo_ptr`). En parametros, los arreglos tambien se
  escriben con este sufijo porque C los recibe como punteros.
- Los structs se pasan por puntero: `const` si la funcion solo los lee.

## Estructura

- `src/` y `include/` — un `.c`/`.h` por módulo:
  - `main.c` — solo el menú principal.
  - `juego` — el desarrollo de una partida: elección de modo y protagonista, recorrido
    de las zonas, fogatas, mejoras, aliados que se suman y final.
  - `zonas` — los datos de cada zona: fases, enemigos y sus stats (`Zona_t`, `Fase_t`).
  - `historia` — textos narrativos (incluidos los que cambian según el protagonista) y
    eventos aleatorios entre fases.
  - `combate` — motor de combate por turnos: daño, ataque en área, habilidades grupales,
    habilidades de los enemigos (incluido Sefirot) y bucle interactivo (`Batalla_t`).
  - `personajes` — personajes jugables, habilidades especiales y estados alterados.
  - `enemigos` — enemigos, jefes y sus habilidades.
  - `progresion` — mejoras, contrabonus y qué aliado se une en cada zona.
  - `items` — ítems e inventario compartido.
  - `archivos` — guardado, carga, borrado de partida (modo difícil) y log en archivo.
  - `arte_ascii` — dibujos de personajes y armas.
  - `utils` — random, pausas, lectura validada de menús, cuadros de texto y colores ANSI.
- `tests/test_combate.c` — tests unitarios del combate, los ítems y el guardado, y la
  simulación de balance de la campaña (`make test`).
- `tests/test_progresion.c` — tests de mejoras y contrabonus.

Archivos que genera el juego al correr (ignorados por git):

- `partida.dat` — partida guardada (binario).
- `juego.log` — registro de eventos (combates ganados/perdidos, partida guardada, etc.).
