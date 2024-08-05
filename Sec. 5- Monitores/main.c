#include <stdio.h>
#include <pthread.h>
#include "garfos.h"

// Programação concorrente em: Problema do jantar dos filósofos.

pthread_t filosofos[N_FILOSOFOS];

int qtdFilosofos = 0;
int *estado_filosofos;

int main(void){

	int id_filosofo[N_FILOSOFOS] = {0,1,2,3,4};
	garfos_init(N_FILOSOFOS);
	
	// criação das threads
	for(int i = 0; i < N_FILOSOFOS; ++i)
		pthread_create(&filosofos[i], NULL, (void *) objetivoFilosofo, (void *) &id_filosofo[i]);
	
	// thread main() espera threads terminarem
	for(int i = 0; i < N_FILOSOFOS; ++i)
		pthread_join(filosofos[i], NULL);
	
	return 0;
}

void mostraMesaAtual(void){
	for(int i = 0; i < qtdFilosofos; ++i){
		switch(estado_filosofos[i]){
			case PENSANDO:
				printf("P ");
				break;
			case COM_FOME:
				printf("F ");
				break;
			case COMENDO:
				printf("C ");
				break;
		}
	}
	printf("\n");
}
