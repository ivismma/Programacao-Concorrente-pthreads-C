/*
Altere o programa s07-a34-tarefa.c colocando um campo a 
mais na estrutura param_t, onde cada thread calculadora 
deverá depositar o maior valor que ela encontrou.

Faça a thread que recebe PTHREAD_BARRIER_SERIAL_THREAD ao 
final dos cálculos comparar os valores obtidos por todas as
threads e colocar o maior de todos na tela.
*/

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <float.h>
#include <locale.h>

#define	N_CALCULADORAS 6 // processadores
#define	N_ELEMENTOS	   10000000

// parâmetros de cada thread
struct param_t {
	int id;
	int inicio;
	int fim;
	double resultado;
};

struct param_t thparam[N_CALCULADORAS];

static double *vetorDados;

// threads
static pthread_t th_calculadora[N_CALCULADORAS];	

// barreiras
static pthread_barrier_t barreira_para_iniciar;
static pthread_barrier_t barreira_para_terminar;

// esse mutex servirá apenas para evitar printf's bagunçados (ocorreu no meu Windows).
pthread_mutex_t mutex_printf = PTHREAD_MUTEX_INITIALIZER;

static void codigo_tarefa_calculadora( struct param_t *param) {
	double meumax = -DBL_MAX;
	int bret1, bret2; // barrier return
	double todosmax = 0;
	
	pthread_mutex_lock(&mutex_printf);
	printf("Thread %d\n", param->id);
	pthread_mutex_unlock(&mutex_printf);

	// dorme por 1 ou 2s...
	sleep(1 + rand()%2); 

	bret1 = pthread_barrier_wait(&barreira_para_iniciar); 

	// calcula
	for(int i=param->inicio; i<param->fim; ++i) {
		if(meumax < vetorDados[i] )
			meumax = vetorDados[i];
	}

	// dorme 1 ou 2s...
	sleep(1 + rand()%2); 

	param->resultado = meumax; // da thread
	bret2 = pthread_barrier_wait(&barreira_para_terminar); 
	
	pthread_mutex_lock(&mutex_printf);
	printf("Thread %d pesquisou de %d ate %d, achou %lf, bret1 %d, bret2 %d\n",
		 param->id, param->inicio, param->fim-1, meumax, bret1, bret2);
	pthread_mutex_unlock(&mutex_printf);
	
	// a thread "especial" é a última de todas, cujo barrier return == -1, 
	// ela é quem atingiu a barreira por último e será responsável por executar 
	// o restante do código. O resto, já finalizou.
	
	if(bret2 == PTHREAD_BARRIER_SERIAL_THREAD) {
		for(int i=0; i<N_CALCULADORAS; ++i)
			if(todosmax < thparam[i].resultado)
				todosmax = thparam[i].resultado;
		printf("Thread %d é especial!!! o maior valor de todos: %lf\n", param->id, todosmax);
	}
	
}


int main(void){
	setlocale(LC_ALL, "Portuguese");
	printf("Início\n");
	
	srand(time(NULL));
	vetorDados = (double *) calloc(N_ELEMENTOS, sizeof(double));
	if(vetorDados == NULL )
		exit(1); // falha aloc.

	for(int i=0; i<N_ELEMENTOS; ++i)
		vetorDados[i] = i;

	// parâmetros da thread
	for(int i=0; i<N_CALCULADORAS; ++i) {
		thparam[i].id = i;
		// definir os limites de cada thread:
		thparam[i].inicio = i*(N_ELEMENTOS/N_CALCULADORAS);
		thparam[i].fim = (i+1)*(N_ELEMENTOS/N_CALCULADORAS);
	}

	// inicializa as barreiras
	pthread_barrier_init(&barreira_para_iniciar, NULL, N_CALCULADORAS);
	pthread_barrier_init(&barreira_para_terminar, NULL, N_CALCULADORAS);

	// cria as threads
	for(int i=0; i<N_CALCULADORAS; ++i)
		pthread_create(&th_calculadora[i], NULL, (void *) codigo_tarefa_calculadora, (void *) &thparam[i]);

	// thread main() espera as threads terminarem.
	for(int i=0; i<N_CALCULADORAS; ++i)
		pthread_join(th_calculadora[i], NULL);

	// destroi as barreiras
	pthread_barrier_destroy(&barreira_para_iniciar);
	pthread_barrier_destroy(&barreira_para_terminar);

	printf("Fim\n");
	
	free(vetorDados);
	return(0);
}



