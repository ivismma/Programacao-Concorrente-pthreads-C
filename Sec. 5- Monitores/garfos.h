#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define N_FILOSOFOS 5

#define PENSANDO 0
#define COM_FOME 1
#define COMENDO  2

void objetivoFilosofo(int *filosofo);
void mostraMesaAtual(void);
void garfos_init(int capacidade);
static void pega_garfos(int fid);
static void larga_garfos(int fid);
static void tenta_atender(int fid);

extern int qtdFilosofos;
extern int *estado_filosofos;
