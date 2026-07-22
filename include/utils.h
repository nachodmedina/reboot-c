#ifndef UTILS_H
#define UTILS_H

// Inicializa la semilla del generador de numeros aleatorios (llamar una vez en main).
void inicializarRandom(void);

// Devuelve un numero entero aleatorio entre min y max, ambos incluidos.
int numeroAleatorio(int min, int max);

// Limpia el buffer de entrada estandar (util despues de leer con scanf).
void limpiarBufferEntrada(void);

// Muestra un mensaje y espera que el usuario presione Enter para continuar.
void pausar(void);

// Lee una opcion numerica de menu, validando que este dentro de un rango.
int leerOpcionMenu(int min, int max);

#endif
