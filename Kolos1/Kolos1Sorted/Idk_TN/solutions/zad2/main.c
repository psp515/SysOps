#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/*
 * Funkcja 'read_end' powinna:
 *  - otworzyc plik o nazwie przekazanej w argumencie
 *    'file_name' w trybie tylko do odczytu,
 *  - przeczytac ostatnie 8 bajtow tego pliku i zapisac
 *    wynik w argumencie 'result'.
 */
void read_end(char *file_name, char *result){
    // Uzupelnij cialo funkcji read_end zgodnie z
    // komentarzem powyzej

    FILE * file = fopen(file_name, "r");

    fseek(file, -9, SEEK_END);

    char data[8];

    for(int i = 0; i < 8; i++)
        data[i] = getc(file);

    result = data;

    fclose(file);
}


int main(int argc, char *argv[]) {
    int result[2];

    if (argc < 2) return -1;
    read_end(argv[1], (char *) result);
    printf("magic number: %d\n", (result[0] ^ result[1]) % 1000);
    return 0;
}
