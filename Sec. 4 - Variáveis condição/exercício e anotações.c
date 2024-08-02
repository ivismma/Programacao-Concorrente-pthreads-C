/* Seção 4

Crie um programa que simula uma linha de produção com 3 estágios.

No primeiro estágio 5 threads fundem peças brutas de alumínio, no
segundo estágio 5 threads pegam as peças brutas do alumínio e
fazem o polimento e, no terceiro estágio, 5 threads pegam as peças
polidas e fazem a pintura.

Mantenha 3 contadores: Número de peças brutas fundidas esperando
polimento, nº de peças esperando pintura e nº de peças pintadas.

Quando uma thread não tem peça para trabalhar ela precisa ficar
bloqueada (wait) até o estágio anterior gerar uma peça para ela.

Dica: o programa terá, além da thread main(), 
15 threads, 3 mutexes e 2 variáveis condição

O programa termina após a produção de 50 peças pintadas.
*/

#include <stdio.h>
#include <locale.h>
#include <unistd.h>
#include <pthread.h>

#define N_THREADS 5
#define N_PRODUCAO 10 // Cada thread faz 10 das operações (5 * 10 = 50)

// Só p/ exibir uma demonstração de estoque a cada segundo
#define DEMO printf("\n>>>  Estoque atual: Peças fundidas: %d  Peças polidas: %d  Fabricadas: %d  <<<\n",\
pecasFundidas, pecasPolidas, pecasPintadas);

/* Obs:
Os printf()'s estão dentro dos mutexes com o único objetivo de mostrar as iterações 
acontecendo, em geral não é recomendado tomar muito espaço dentro de sessões 
críticas p/ não aumentar a espera entre as threads pelo bloqueio.
*/

void fundir(void);
void polir(void);
void pintar(void);

pthread_t fundirPeca[N_THREADS];
pthread_t polirPeca[N_THREADS]; 
pthread_t pintarPeca[N_THREADS];

pthread_mutex_t mutex_pecasFundidas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pecasPolidas  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pecasPintadas = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t temFundida = PTHREAD_COND_INITIALIZER;
pthread_cond_t temPolida  = PTHREAD_COND_INITIALIZER;

int pecasFundidas = 0; // pronta para polir...
int pecasPolidas  = 0; // pronta para pintar...
int pecasPintadas = 0; // peças fabricadas.

int main(void){
	setlocale(LC_ALL, "Portuguese");
	
	// criação das threads
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&fundirPeca[i], NULL, (void *) fundir, NULL);
	
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&polirPeca[i], NULL, (void *) polir, NULL);
		
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&pintarPeca[i], NULL, (void *) pintar, NULL);
	
	// execução das threads
	for(int i = 0; i < N_THREADS; ++i)
		pthread_join(fundirPeca[i], NULL);
	
	for(int i = 0; i < N_THREADS; ++i)
		pthread_join(polirPeca[i], NULL);
		
	for(int i = 0; i < N_THREADS; ++i)
		pthread_join(pintarPeca[i], NULL);
	
	DEMO;
	return 0;
}

void fundir(void){
	for(int i = 0; i < N_PRODUCAO; ++i){
		sleep(1);
		
		printf("Thread[%zu]Fundindo peça...\n", pthread_self());
		pthread_mutex_lock(&mutex_pecasFundidas);
		DEMO;
		++pecasFundidas;
		pthread_cond_signal(&temFundida);
		
		printf("Thread[%zu]Peça fundida...\n", pthread_self());
		
		pthread_mutex_unlock(&mutex_pecasFundidas);
	}
}

void polir(void){
	for(int i = 0; i < N_PRODUCAO; ++i){
		sleep(1);
		
		pthread_mutex_lock(&mutex_pecasFundidas);
		DEMO;
		while(pecasFundidas == 0)
			pthread_cond_wait(&temFundida, &mutex_pecasFundidas);
		--pecasFundidas;
		printf("Thread[%zu] Pegou uma peça fundida...\n", pthread_self());
		pthread_mutex_unlock(&mutex_pecasFundidas);
		
		printf("Thread[%zu]Polindo peça...\n", pthread_self());
		pthread_mutex_lock(&mutex_pecasPolidas);
		
		++pecasPolidas;
		pthread_cond_signal(&temPolida);
		
		printf("Thread[%zu]Peça polida...\n", pthread_self());
		pthread_mutex_unlock(&mutex_pecasPolidas);
		
		
	}
	
}

void pintar(void){
	for(int i = 0; i < N_PRODUCAO; ++i){
		sleep(1);
		
		pthread_mutex_lock(&mutex_pecasPolidas);
		DEMO;
		while(pecasPolidas == 0)
			pthread_cond_wait(&temPolida, &mutex_pecasPolidas);
		--pecasPolidas;
		
		printf("Thread[%zu] Pegou uma peça polida...\n", pthread_self());
		pthread_mutex_unlock(&mutex_pecasPolidas);
		
		
		pthread_mutex_lock(&mutex_pecasPintadas);
		printf("Thread[%zu] Pintando peça...\n", pthread_self());
		++pecasPintadas;
		
		printf("Thread[%zu] Peça pintada e terminada...\n", pthread_self());
		pthread_mutex_unlock(&mutex_pecasPintadas);
		
	}
}
