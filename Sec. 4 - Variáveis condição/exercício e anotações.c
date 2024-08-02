/* Se��o 4

Crie um programa que simula uma linha de produ��o com 3 est�gios.

No primeiro est�gio 5 threads fundem pe�as brutas de alum�nio, no
segundo est�gio 5 threads pegam as pe�as brutas do alum�nio e
fazem o polimento e, no terceiro est�gio, 5 threads pegam as pe�as
polidas e fazem a pintura.

Mantenha 3 contadores: N�mero de pe�as brutas fundidas esperando
polimento, n� de pe�as esperando pintura e n� de pe�as pintadas.

Quando uma thread n�o tem pe�a para trabalhar ela precisa ficar
bloqueada (wait) at� o est�gio anterior gerar uma pe�a para ela.

Dica: o programa ter�, al�m da thread main(), 
15 threads, 3 mutexes e 2 vari�veis condi��o

O programa termina ap�s a produ��o de 50 pe�as pintadas.
*/

#include <stdio.h>
#include <locale.h>
#include <unistd.h>
#include <pthread.h>

#define N_THREADS 5
#define N_PRODUCAO 10 // Cada thread faz 10 das opera��es (5 * 10 = 50)

// S� p/ exibir uma demonstra��o de estoque a cada segundo
#define DEMO printf("\n>>>  Estoque atual: Pe�as fundidas: %d  Pe�as polidas: %d  Fabricadas: %d  <<<\n",\
pecasFundidas, pecasPolidas, pecasPintadas);

/* Obs:
Os printf()'s est�o dentro dos mutexes com o �nico objetivo de mostrar as itera��es 
acontecendo, em geral n�o � recomendado tomar muito espa�o dentro de sess�es 
cr�ticas p/ n�o aumentar a espera entre as threads pelo bloqueio.
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
int pecasPintadas = 0; // pe�as fabricadas.

int main(void){
	setlocale(LC_ALL, "Portuguese");
	
	// cria��o das threads
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&fundirPeca[i], NULL, (void *) fundir, NULL);
	
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&polirPeca[i], NULL, (void *) polir, NULL);
		
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&pintarPeca[i], NULL, (void *) pintar, NULL);
	
	// execu��o das threads
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
		
		printf("Thread[%zu]Fundindo pe�a...\n", pthread_self());
		pthread_mutex_lock(&mutex_pecasFundidas);
		DEMO;
		++pecasFundidas;
		pthread_cond_signal(&temFundida);
		
		printf("Thread[%zu]Pe�a fundida...\n", pthread_self());
		
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
		printf("Thread[%zu] Pegou uma pe�a fundida...\n", pthread_self());
		pthread_mutex_unlock(&mutex_pecasFundidas);
		
		printf("Thread[%zu]Polindo pe�a...\n", pthread_self());
		pthread_mutex_lock(&mutex_pecasPolidas);
		
		++pecasPolidas;
		pthread_cond_signal(&temPolida);
		
		printf("Thread[%zu]Pe�a polida...\n", pthread_self());
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
		
		printf("Thread[%zu] Pegou uma pe�a polida...\n", pthread_self());
		pthread_mutex_unlock(&mutex_pecasPolidas);
		
		
		pthread_mutex_lock(&mutex_pecasPintadas);
		printf("Thread[%zu] Pintando pe�a...\n", pthread_self());
		++pecasPintadas;
		
		printf("Thread[%zu] Pe�a pintada e terminada...\n", pthread_self());
		pthread_mutex_unlock(&mutex_pecasPintadas);
		
	}
}
