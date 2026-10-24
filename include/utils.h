#ifndef UTILS_H
#define UTILS_H

#define COLOR_RESET "\033[0m"
#define COLOR_AZUL_CLARO "\033[96m"
#define COLOR_ROJO "\033[31m"
#define COLOR_VERDE "\033[32m"
#define COLOR_AMARILLO "\033[33m"
#define COLOR_AZUL "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_BLANCO "\033[37m"
#define ESTILO_NEGRITA "\033[1m"
#define ESTILO_TENUE "\033[2m"

// Inicializa el soporte ANSI de la terminal (necesario en la consola clasica de Windows).
void inicializarColoresTerminal(void);

// Devuelve el codigo de estilo o una cadena vacia si la terminal no lo admite.
const char *estiloTerminal(const char *codigo_ptr);

// Limpia la pantalla sin afectar el estado del juego.
void limpiarPantallaTerminal(void);

// Muestra una escena con borde doble y ajusta el texto al ancho del cuadro.
// El titulo_ptr puede ser NULL o vacio para omitir el encabezado.
void mostrarTextoEnCaja(const char *titulo_ptr, const char *texto_ptr, const char *color_ptr);

// Muestra una escena con estilo independiente para el encabezado.
void mostrarTextoEnCajaConTituloEstilizado(const char *titulo_ptr, const char *texto_ptr,
                                          const char *color_ptr,
                                          const char *colorTitulo_ptr,
                                          const char *estiloTitulo_ptr);

// Imprime arte ASCII respetando exactamente los espacios de cada linea.
void mostrarArteASCII(const char *const lineas_ptr[], int cantidadLineas);

// Inicializa la semilla del generador de numeros aleatorios (llamar una vez en main).
void inicializarRandom(void);

// Devuelve un numero entero aleatorio entre min y max, ambos incluidos.
int numeroAleatorio(int min, int max);

// Limpia el buffer de entrada estandar (util despues de leer con scanf).
void limpiarBufferEntrada(void);

// Anima puntos suspensivos y espera que el usuario presione Enter para continuar.
void pausar(void);

// Muestra un menu y devuelve el indice elegido (0..cantidadOpciones-1).
// Usa flechas en una consola compatible y entrada numerica como alternativa.
int leerOpcionMenu(const char *opciones_ptr[], int cantidadOpciones);

#endif
