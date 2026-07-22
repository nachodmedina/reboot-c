# Reboot C

Videojuego por turnos en C puro (C11), para consola. Proyecto integrador de Programación I (UTN).

## Compilar y correr

```bash
make
./rebootc
```

Para limpiar los binarios generados:

```bash
make clean
```

## Estado actual

Esqueleto inicial del proyecto: estructura de módulos, structs principales y un menú
de consola (Nueva partida / Cargar partida / Salir) que ya compila y corre. La lógica
de combate, historia y guardado completo se van a ir sumando módulo por módulo.

## Estructura

- `src/` y `include/` — un `.c`/`.h` por módulo:
  - `personajes` — personajes jugables (Isaac, Dante, Frieren).
  - `enemigos` — enemigos por zona.
  - `combate` — turnos, cálculo de daño, estados alterados.
  - `items` — ítems e inventario dinámico.
  - `archivos` — guardado, carga y log en archivo.
  - `historia` — zonas y narrativa.
  - `utils` — funciones auxiliares (random, menús, entrada por consola).
  - `main.c` — flujo principal del juego.
