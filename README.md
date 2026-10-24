# Reboot C

Videojuego por turnos en C puro (C11), para consola. Proyecto integrador de Programación I (UTN).

Año 2076. Las máquinas se infiltran en La Raíz, el último refugio humano bajo tierra.
En medio del ataque, uno de los tres protagonistas queda separado del resto, encuentra
una baliza antigua y su arma se transforma, despertando una habilidad especial. Siguiendo
la luz de la baliza, sale a la superficie y se enfrenta solo a la primera zona.

## Compilar y correr

```bash
make
./rebootc
```

Tests del motor de combate y simulación automática de la Zona 1:

```bash
make test
./test_combate 123   # opcional: correr la simulacion con otra semilla
```

Para limpiar los binarios generados:

```bash
make clean
```

## Estado actual

El juego compila sin warnings (`-Wall -Wextra -std=c11`) y se puede jugar de punta a
punta la **introducción y la Zona 1 (Campo Ceniza)** con el protagonista elegido.
Todavía no hay una segunda zona ni momento en que se junte el equipo completo.

### Flujo de una partida nueva

1. **Menú principal**: Nueva partida / Cargar partida / Salir / Probar combate (temporal).
2. **Modo de juego**: Normal o Difícil.
3. **Introducción del mundo**: el contexto del año 2076 y el ataque a La Raíz.
4. **Elegir protagonista** entre Isaac, Dante y Frieren.
5. **Narrativa inicial**: la separación, la baliza, la transformación del arma y la
   habilidad que desbloquea cada personaje.
6. **Salida de La Raíz**: un fragmento de la baliza marca el camino a la superficie.
7. **Zona 1: Campo Ceniza**, en solitario: 4 fases de combate, con eventos aleatorios
   entre fases y una fogata antes del jefe.
8. **Guardado** del equipo en `partida.dat` al superar la zona.

Si el protagonista cae en combate, la partida termina. En **Modo Difícil**, además, se
borra la partida guardada (permadeath).

### Personajes

Por ahora los tres tienen las mismas stats; solo cambia la habilidad especial.

| Personaje | Arma                  | Vida | Ataque | Defensa | Habilidad especial                          |
|-----------|-----------------------|------|--------|---------|---------------------------------------------|
| Isaac     | Espada de Excabilur   | 320  | 80     | 40      | Crítico grupal: el equipo pega x1.2         |
| Dante     | Lanza de Rebellion    | 320  | 80     | 40      | Defensa grupal: +50% de defensa al equipo   |
| Frieren   | Bastón de la Pérdida  | 320  | 80     | 40      | Curación grupal: +40 de vida, dos veces     |

Las habilidades duran 2 rondas y solo puede haber una activa a la vez.

### Combate

Por turnos: primero actúan los personajes y después los enemigos (que eligen a quién
atacar al azar). En cada turno el jugador elige:

1. **Atacar a un enemigo**: daño = `ataque - defensa` (mínimo 0).
2. **Ataque en área**: golpea a todos los enemigos vivos con `ataque / 2 + 15 - defensa`.
3. **Habilidad especial** (ver tabla de personajes).
4. **Usar ítem**: vida, ataque o defensa.

Estados alterados:

- **Veneno**: resta vida al final de cada turno mientras dure.
- **Debilitado**: −25% de ataque. Lo aplican los enemigos con *Trampa*.
- **Aturdido**: pierde el turno (el combate lo respeta, pero todavía nada lo aplica).

### Zona 1: Campo Ceniza

| Fase | Enemigos                                     | Notas                       |
|------|----------------------------------------------|-----------------------------|
| 1    | 2 Centinela Canino                           |                             |
| 2    | 1 Cazador de Campo Ceniza                    |                             |
| 3    | 1 Cazador + 2 Centinela Canino               |                             |
| 4    | 1 Cazador + **Spectre** (jefe)               | Fogata antes: vida al 100%  |

| Enemigo                  | Vida | Ataque | Defensa | Habilidad                                        |
|--------------------------|------|--------|---------|--------------------------------------------------|
| Centinela Canino         | 120  | 48     | 8       | —                                                |
| Cazador de Campo Ceniza  | 160  | 56     | 16      | Trampa: 40% de dejar debilitado 1 turno          |
| Spectre                  | 280  | 60     | 40      | Crítico: 30% de pegar x1.5                       |

Entre fases (salvo antes de la fogata) ocurre un **evento aleatorio**: nada, una nube
tóxica que envenena, una planta medicinal que cura el veneno, o un robot de apoyo que
puede mejorar vida máxima, ataque y defensa (+10 cada uno). Algunos eventos encuentran
pociones o veneno, pero todavía no se pueden guardar.

### Pendiente

- **Más zonas** y el reencuentro con el resto del equipo (en `main.c` ya está marcado
  dónde se conecta la siguiente zona).
- **Inventario en la partida real**: el inventario dinámico y `usarItem` funcionan, pero
  en la Zona 1 el combate corre sin ítems. Solo se pueden usar en "Probar combate".
- **Guardar lo que se encuentra** en los eventos (pociones, veneno) y usar el veneno
  contra los enemigos.
- **Aturdido**: falta algo que lo aplique.
- **Stats propias** para cada personaje.
- **Guardado completo**: hoy solo se guarda el equipo y recién al terminar la zona.
  Falta guardar la zona actual, el inventario, el modo y el protagonista, y que
  "Cargar partida" retome la historia (hoy solo muestra el equipo guardado).
- Sacar la opción temporal **"Probar combate"** del menú.

## Presentacion en terminal

En una consola compatible, los menus de seleccion se recorren con las flechas y
se confirman con Enter. Si la entrada no es una consola interactiva, el juego
muestra las mismas opciones numeradas para poder elegirlas escribiendo un numero.
Los colores usan secuencias ANSI: aliados en verde, enemigos en rojo y el numero
de ronda en celeste. Los mensajes de combate resaltan en negrita a quienes actuan.
La pantalla se limpia al comenzar cada ronda.

Las escenas narrativas aparecen en cuadros de doble borde blanco. Los dibujos
ASCII son espacios reservados para que se peguen manualmente:

- En `src/main.c`, reemplaza el texto de `arteIsaac_ptr`, `arteDante_ptr` y
  `arteFrieren_ptr`. Esos dibujos aparecen despues de elegir al heroe.
- En `src/historia.c`, reemplaza el texto de `arteArmaIsaac_ptr`,
  `arteArmaDante_ptr` y `arteArmaFrieren_ptr`. El dibujo correspondiente se
  muestra junto al anuncio de que se consiguio el arma.
- Mantene cada linea entre comillas y separada por comas. En una cadena C, una
  barra invertida se escribe `\\` y una comilla doble se escribe `\"`. Respeta
  los espacios iniciales y finales de cada linea para conservar la alineacion.

## Convenciones de nombres C

- Los alias de tipos declarados con `typedef struct` terminan en `_t`
  (`Personaje_t`, `Inventario_t`).
- Los nombres de variables y parametros puntero terminan en `_ptr`
  (`personaje_ptr`, `archivo_ptr`). En parametros, los arreglos tambien se
  escriben con este sufijo porque C los recibe como punteros.

## Estructura

- `src/` y `include/` — un `.c`/`.h` por módulo:
  - `personajes` — personajes jugables, habilidades especiales y estados alterados.
  - `enemigos` — enemigos y sus habilidades (Trampa, Crítico).
  - `combate` — motor de combate por turnos: daño, ataque en área, habilidades
    grupales, turnos de enemigos y bucle interactivo.
  - `items` — ítems e inventario dinámico (`malloc` / `realloc`).
  - `archivos` — guardado, carga, borrado de partida (modo difícil) y log en archivo.
  - `historia` — narrativa, zonas y fases (`Zona_t`, `Fase_t`), Campo Ceniza y eventos
    aleatorios entre fases.
  - `utils` — random, pausas, lectura validada de menús y helpers de colores ANSI.
  - `main.c` — menú principal, flujo de nueva partida, recorrido de zonas y fogata.
- `tests/test_combate.c` — tests unitarios del motor de combate y simulación de la
  Zona 1 sin teclado (`make test`).

Archivos que genera el juego al correr (ignorados por git):

- `partida.dat` — partida guardada (binario).
- `juego.log` — registro de eventos (combates ganados/perdidos, partida guardada, etc.).
