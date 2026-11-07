#include <stdio.h>
#include "../include/juego.h"
#include "../include/menu.h"
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
        opcion = leerOpcionMenu(opcionesMenu_ptr, CANT_OPCIONES_MENU);

        switch (opcion) {
            case MENU_NUEVA_PARTIDA:
                jugarPartidaNueva();
                break;
            case MENU_CARGAR_PARTIDA:
                continuarPartidaGuardada();
                break;
            case MENU_SALIR:
                printf("\nHasta la proxima.\n");
                break;
        }
    } while (opcion != MENU_SALIR);

    return 0;
}
