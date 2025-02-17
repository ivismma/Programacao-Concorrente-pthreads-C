#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define N_THREADS 5

/*
    Crie 5 threads que ficam em laço com sleep(1) enquanto a flag
    "continuar" for verdadeira.
    
    Crie uma thread para capturar control-c e mudar a flag para falso.
    
    A thread main apenas cria todas as threads e fica esperando pelo final
    das 5 threads.
*/

bool CONTINUAR = true;

pthread_t threadLoop[N_THREADS];
pthread_t handler;

// função das threads (loop de sleep(1) )
void threadLoopTask(){
	while(CONTINUAR){
	    printf("thread %zu vai dormir\n", pthread_self());	
		sleep(1);
	}
		
	printf("thread %zu - CONTINUAR = 0.\n", pthread_self());
}

// tratamento do sinal
void signalHandler(){
	printf("Control C acionado.\n");
}

void threadHandlerTask(){
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	
	while(true){
		int sinal = sigwaitinfo(&mask, NULL);
		if(sinal == SIGINT){
		    CONTINUAR = false;
		    signalHandler(); // função que trataria tal caso
		}
	}
}

int main(void){

    // main não irá tratar SIGINT (vai ignorar)...
    sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	pthread_sigmask(SIG_BLOCK, &mask, NULL); // <-- bloqueia os sinais citados (só SIGINT)
	
    // threads filhas irão herdar isso, exceto a que irá tratar o SIGINT (thread handler)

	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&threadLoop[i], NULL, (void *) threadLoopTask, NULL);

    // thread handler:
    pthread_create(&handler, NULL, (void *) threadHandlerTask, NULL);

	for(int i = 0; i < N_THREADS; ++i)
		pthread_join(threadLoop[i], NULL);

	return 0;
}
