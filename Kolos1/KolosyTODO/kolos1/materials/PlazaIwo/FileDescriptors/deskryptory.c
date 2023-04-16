#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* 
Wstaw we wskazanych miejscach odpowienie deskryptory, sposrod 
fd1, fd2, fd3, by uzyskac na terminalu nastepujacy efekt dzialania 
programu:

Hello,
Hello, 12345678
HELLO, 12345678
WITAM! 12345678


Prosze nie wprowadzac innych zmian do programu niz wskazane powyzej w miejscach oznaczonych komentarzami
*/


int main(int argc, char *argv[])
{
    int fd1, fd2, fd3;
    #define file "a"
    char cmd[] = "cat " file "; echo";

    // OPENING FILES

   

    // WRITING TO FILES

    
    // CLOSING OPEN FILES.

    

    exit(EXIT_SUCCESS);
}
