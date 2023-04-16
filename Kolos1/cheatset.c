// 1 - Podstawowe funkcje
// Mierzenie czasu 

#include <time.h>
#include <sys/times.h>

long double calculate_time_clocks(clock_t start, clock_t end) {
    return (long double) (end - start) / CLOCKS_PER_SEC;
}

// dla pól struktury tms z sys/times tms.tms_utime tms.tms_stime
long double calculate_time_tics(clock_t start, clock_t end) {
    return (long double) (end - start) / sysconf(_SC_CLK_TCK);
}

// Zapisywanie startu / stopu zegara
start = clock();
times(&tms_start);

//

#include <stdlib.h>
char * array = malloc(sizeof()); // inicjalizuje miejsce

char * array2 = calloc(n, sizeof()); // inicjalizuje miejsca i ustawia wszyskto na 0

char blok[1024]; // alokowanie tablicy bez calloc malloc 

// zwalnianie miejsca
free(array);
free(array2);

// 2 - Pliki

// Podejscie system

#include <fcntl.h>

int descriptor = open(const char *pathname, int flags ,[mode_t mode]); // flags to O_RDONLY, O_WRONLY, O_RDWR	| O_CREAT, mode to 0666 np.
int creat(const char *pathname, mode_t mode);

// zwracaja -1 gdy jest błąd
// po uzyciu creat i tak trzeba użyc open !

close(descriptor);

ilosc przeczytanych danych = read(deskryptor, buffer, wielkosc_buffa);

write(deskryptor, buffer, wielkosć)

lseek(deskryptor, nnowa pozycja w pliku, wzgledem jakiej pozycji )

// Whence: 
SEEK_SET – początek pliku
SEEK_END – koniec pliku
SEEK_CUR – aktualna pozycja wskaźnika

// Deskryptor po odczytaniu automatycznie sie przezsuwa do przodu

#include <unistd.h>
#include <sys/stat.h>
// PRzydatne includy 


// Podejscie C
#include <stdio.h>

FILE * fopen ( const char * filename, const char * mode ); // r w a r+ w+(tworzy jak nie istniał)

size_t fread ( void * ptr, size_t size, size_t count, FILE * file); // tablica, rozmiar elementu, tablicy, i plik

size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * file); // tablica, rozmiar elementu, tablicy, i plik
// przesuwa wskaznik pliku ! 

int fseek ( FILE * file, long int offset, int mode); // tak samo jak lseek()

// fpos przekazane przez refa 
int fsetpos (FILE* file, fpos_t* pos); 
int fgetpos (FILE* file, fpos_t* pos); 

// zamyklanie pliku
int fclose ( FILE * stream );

// Katalogi 

#include <dirent.h> 

DIR* opendir(const char* dirname); // zwraca NULL gdy nie połaczy 

int closedir(DIR* dirp); 
struct dirent* readdir(DIR* dirp); // zwraca następny plik 
void rewinddir(DIR* dirp); // ustawia katalog na początek 
int stat (const char *path, struct stat *buf); // informacje o pliku przez refa przkasany stat


int mkdir (const char *path, mode_t mode); //- tworzenie katalogu z uprawnieniami podanymi w mode
int rmdir (const char *path); //- usuwanie katalogu
int chdir (const char *path); //- argument path staje się nowym katalogiem bieżącym dla programu.
char *getcwd (char *folder_name, ssize_t size); //- funkcja wpisuje do folder_name bieżący katalog roboczy o rozmiarze size.
int chmod (const char *path, mode_t new); //- zmiana uprawnień do pliku.
int chown (const char *path, uid_t id_wlaściciela, gid_t id_grupy); //- zmiana właściciela.

int nftw(const char *dir, int(*fn) (), int nopend, int flags)

//przykład funkcji dla nftw 

int process(const char* path, const struct stat* data, int flag)
{
    if(!S_ISDIR(data->st_mode) && flag == FTW_F)
    {
        bytes += data->st_size;
        printf("%ld %s\n", data->st_size, path);
    }

    return 0;
}

//SPrawdzanie jaki to rodzaj pliku

struct stat data;
int state = stat(dir_element->d_name, &data);
if(!S_ISDIR(data.st_mode))

// dir - katalog główny drzewa do przeglądnięcia
// fn - funkcja wywoływana dla każdego przeglądanego elementu w drzewie
// nopenfd - maksymalna ilość otwieranych przez funkcję deskryptorów
// flags - znaczniki definiujące zachowanie funkcji

// 3 - Procesy

#include <sys/types.h>

pid_t getpid(void) //- zwraca PID procesu wywołującego funkcję
pid_t getppid(void) //- zwraca PID procesu macierzystego
uid_t getuid(void) //- zwraca rzeczywisty identyfikator użytkownika UID
uid_t geteuid(void) // - zwraca efektywny identyfikator użytkownika UID
gid_t getgid(void) //- zwraca rzeczywisty identyfikator grupy GID
gid_t getegid(void) //- zwraca efektywny identyfikator grupy GID

pid_t fork( void ) // tworzenie procesu
// zwraca 0 jak sie jest w childzie 

int execl(char const *path, char const *arg0, NULL) // trzeba kończyc go nullem pierwsze jeszcze nazwa pliku idzie

#include <sys/wait.h>

// oczekiwanie na zakończenie procesu potomnego

pid_t wait ( int *statloc )

// while(wait(NULL) > 0){} oczekuje na zakonczenie wszystkich forków 

pid_t waitpid( pid_t pid, int *statloc, int options )

#include <stdlib.c>

exit();
// mozna stosowac ta funkcje do konczenia procesu

kill() // <- to własciwie działą w sygnałach 

// 4 - sygnały

// kill

#include <sys/types.h>
#include <signal.h>

kill(pid, sygnał); // wysyła sygnał do procesu

raise(sygnał) // to samo tylko do swojego procesu

sigqueue(pid, sig, struct sigval value)  // wysyła dodatkowa stuktóre wraz z sygnałem 

union sigval 
{
 int sival_int;
 void *sival_ptr;
} 

signal(sygnał, funkcja) // ustawia funkcje do odpalenia gdy pojawi sie sygnał 

// dodatkowa struktura 
struct sigaction act;
act.sa_flags = SA_SIGINFO;
act.sa_handler = handler; // handler przyjmuje 3 argumenty 
sigemptyset(&act.sa_mask);

sigaction((int sig_no, const struct sigaction *act, struct sigaction *old_act (TO ZWYKLE NULL)) // rozszerzenie sygnału

#include <unistd.h> 

void pause(); // program ma stopa do otrzymania sygnału
int sleep(unsigned int seconds); // usypia proces do czasu albo sygnału 

// funkcje do ustawiania setów sygnałów 
sigemptyset
sigfillset
sigaddset
sigdelset
sigismember
sigprocmask
sigpending

int sigsuspend(cost sigset_t *set); // oczekiwanie na grupe sygnałów 

// 5 - Potoki

// Nie można uzywać potoków do pisania i czytania w 2 strony tylko w 1

// Nienazwane

// stworznie potoku i fork

int fd[2];
pipe(fd); // tworzy potok 
pid_t pid = fork();
if (pid == 0) { // dziecko
    close(fd[1]); 
    // read(fd[0], ...) - odczyt danych z potoku
} else { // rodzic
    close(fd[0]);
    // write(fd[1], ...) - zapis danych do potoku
}


int dup2(int oldfd, int newfd); // podmienia deskryptory


// Przykład dup2 który ma zamiast czytania z STDIN to bedzie czytał z pliku do którgo moze pisać rodzic  
int fd[2];
pipe(fd);
pid_t pid = fork();
if (pid == 0) { // dziecko
    close(fd[1]); 
    dup2(fd[0], STDIN_FILENO);
    execlp("grep", "grep","Ala", NULL);
} else { // rodzic
    close(fd[0]);
    // write(fd[1], ...) - przesłanie danych do grep-a
}

FILE* popen(const char* command, const char* type); // realizuje powyzszy 
//przypadek w zaleznoscio od type albo piszemy na standardowe wejscie albo odczytujemy z wyjscia 
int pclose(FILE* stream);

// Nazwane 
#include <sys/stat.h>
#include <sys/types.h>

int mkfifo(const char *pathname, mode_t mode); // tworzy potok na który mozna pisać 
int mknod(const char *pathname, mode_t mode, dev_t dev);