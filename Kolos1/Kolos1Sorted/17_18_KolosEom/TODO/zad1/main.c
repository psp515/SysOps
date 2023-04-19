#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    DIR* directory;
    if (argc != 2) {
        printf("Call %s path", argv[0]);
        return 1;
    }
    struct dirent* position;

    /*
     * Otwórz katalog, w przypadku błędu otwarcia zwróć błąd funkcji
     otwierającej i wartość 1. Wyświetl zawartość katalogu katalog, pomijając
     "." i ".." Jeśli podczas tej operacji wartość errno zostanie ustawiona,
     zwróć błąd funkcji czytającej oraz wartość 2*/

    /* ADDED BY ME - FROM HERE */

    

    /* TILL HERE */

    closedir(directory);
    return (0);
}