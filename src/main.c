#include <stdio.h>
#include "../include/juego.h"
#include "../include/utils.h"

int main(void) {
    inicializarRandom();
    inicializarColoresTerminal();

    const char *opcionesMenu_ptr[] = {
        "Nueva partida",
        "Cargar partida",
        "Salir"
    };
    int opcion;
    do {
        printf("\n%s%s===== Reboot C =====%s\n\n",
               estiloTerminal(COLOR_MAGENTA), estiloTerminal(ESTILO_NEGRITA),
               estiloTerminal(COLOR_RESET));
        opcion = leerOpcionMenu(opcionesMenu_ptr, 3);

        switch (opcion) {
            case 0:
                jugarPartidaNueva();
                break;
            case 1:
                continuarPartidaGuardada();
                break;
            case 2:
                printf("\nHasta la proxima.\n");
                break;
        }
    } while (opcion != 2);

    return 0;
}
