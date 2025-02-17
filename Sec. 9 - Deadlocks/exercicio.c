/*
	Adapte o programa s09-a42-quatrocondicoes.c para 
	detectar deadlocks.

	Cada thread agora executa um laço com 1000 iterações,
	o corpo do laço é o código da tarefa em s09-a42-quatrocondicoes.c

	Para cada thread crie um contador inteiro que é zerado pela própria
	thread ao final de cada iteração do seu laço.
	
	Uma nova thread "vigia", acorda a cada segundo e incrementa cada
	um dos contadores. Ela então compara cada contador com um limite
	estabelecido para a thread em questão.
	
	Caso o contador da thread 'x' passe do limite definido para a thread
	'x', a mesma é considerada em deadlock e o programa é abortado com
	uma msg de erro.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define	MAX_TEXTO 1000
#define MAX_ITER 1000
#define MAX_TIMER 4 // usei 4 segundos para todas as threads...

static pthread_t t1;
static int t1_timer = 0;

static pthread_t t2; 
static int t2_timer = 0;

static pthread_t t3;
static int t3_timer = 0;

// thread que irá vigiar as outras threads:
static pthread_t vigia;
static pthread_mutex_t mutex_vigia= PTHREAD_MUTEX_INITIALIZER;

///////////////////////////////////////////////////////////////

static char texto1[MAX_TEXTO];
static pthread_mutex_t mutex_texto1 = PTHREAD_MUTEX_INITIALIZER;

static char texto2[MAX_TEXTO];
static pthread_mutex_t mutex_texto2 = PTHREAD_MUTEX_INITIALIZER;

static char texto3[MAX_TEXTO];
static pthread_mutex_t mutex_texto3 = PTHREAD_MUTEX_INITIALIZER;

/////////////////////////////////////////////////////////////////

//  mutex para printf (desconsiderar todos os mutexes de printf)
// pois no meu Windows, escalonamento de threads bagunça o output.
static pthread_mutex_t mutex_printf = PTHREAD_MUTEX_INITIALIZER;

static void threadVigiaTask(void){
	while(1){
		sleep(1);
		// pega mutex dos timers (vigia)
		pthread_mutex_lock(&mutex_vigia);
		
		// incrementa e já checa se passaram do limite de tempo...
		if(++t1_timer > MAX_TIMER){
			printf("entrou em deadlock... encerrando...\n");
			exit(1);
		}
		
		if(++t2_timer > MAX_TIMER){
			printf("entrou em deadlock... encerrando...\n");
			exit(1);
		}
		
		if(++t3_timer > MAX_TIMER){
			printf("entrou em deadlock... encerrando...\n");
			exit(1);
		}
		
		// libera mutex
		pthread_mutex_unlock(&mutex_vigia);
	}
}

static void codigo_tarefa_1(void) {
	for(int i = 0; i < MAX_ITER; ++i){
		pthread_mutex_lock( &mutex_texto1);
		
		pthread_mutex_lock(&mutex_printf);
		printf("Tarefa 1 pegou o texto 1\n");
		pthread_mutex_unlock(&mutex_printf);
		
		strcpy( texto1, "qwerty111");
		sleep(1);
		
		pthread_mutex_lock(&mutex_texto2);
		
		pthread_mutex_lock(&mutex_printf);
		printf("Tarefa 1 pegou o texto 2\n");
		pthread_mutex_unlock( &mutex_printf);
		
		strcpy( texto2, texto1);
		sleep(1);
	
		pthread_mutex_lock(&mutex_printf);
		printf("Tarefa 1 vai soltar texto 2 e 1\n");
		pthread_mutex_unlock(&mutex_printf);
		
		pthread_mutex_unlock( &mutex_texto2);
		pthread_mutex_unlock( &mutex_texto1);
		
		pthread_mutex_lock(&mutex_vigia);
		t1_timer = 0;
		pthread_mutex_unlock(&mutex_vigia);
	}
}


/** Tarefa 2
	Acessa as variaveis texto2 e texto3
*/
static void codigo_tarefa_2(void) {
	for(int i = 0; i < MAX_ITER; ++i){
		pthread_mutex_lock( &mutex_texto2);
		
		pthread_mutex_lock( &mutex_printf);
		printf("Tarefa 2 pegou o texto 2\n");
		pthread_mutex_unlock( &mutex_printf);
		
		strcpy( texto2, "qwerty222");
		sleep(1);
		
		pthread_mutex_lock( &mutex_texto3);
		
		pthread_mutex_lock(&mutex_printf);
		printf("Tarefa 2 pegou o texto 3\n");
		pthread_mutex_unlock(&mutex_printf);
		
		strcpy( texto3, texto2);
		sleep(1);
		
		pthread_mutex_lock(&mutex_printf);
		printf("Tarefa 2 vai soltar texto 3 e 2\n");
		pthread_mutex_unlock(&mutex_printf);
		
		pthread_mutex_unlock( &mutex_texto3);
		pthread_mutex_unlock( &mutex_texto2);
		
		pthread_mutex_lock(&mutex_vigia);
		t2_timer = 0;
		pthread_mutex_unlock(&mutex_vigia);
	}
}

/** Tarefa 3
	Acessa as variaveis texto3 e texto1
*/
static void codigo_tarefa_3(void) {
	for(int i = 0; i < MAX_ITER; ++i){
		pthread_mutex_lock( &mutex_texto3);
		
		pthread_mutex_lock(&mutex_printf);
		printf("Tarefa 3 pegou o texto 3\n");
		pthread_mutex_unlock(&mutex_printf);
		
		strcpy( texto3, "qwerty333");
		sleep(1);
		
		pthread_mutex_lock( &mutex_texto1);
		
		pthread_mutex_lock(&mutex_printf);
		printf("Tarefa 3 pegou o texto 1\n");
		pthread_mutex_unlock(&mutex_printf);
		
		strcpy( texto1, texto3);
		sleep(1);
		
		pthread_mutex_lock(&mutex_printf);
		printf("Tarefa 3 vai soltar texto 1 e 3\n");
		pthread_mutex_unlock(&mutex_printf);
		
		pthread_mutex_unlock( &mutex_texto1);
		pthread_mutex_unlock( &mutex_texto3);
		
		pthread_mutex_lock(&mutex_vigia);
		t3_timer = 0;
		pthread_mutex_unlock(&mutex_vigia);
	}
}

int main(void){
	printf("Inicio\n");
  
	pthread_create(&t1, NULL, (void *) codigo_tarefa_1, NULL);
	pthread_create(&t2, NULL, (void *) codigo_tarefa_2, NULL);
	pthread_create(&t3, NULL, (void *) codigo_tarefa_3, NULL);
	pthread_create(&vigia, NULL, (void *) threadVigiaTask, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);

	printf("Fim\n");
	return(0);
}
