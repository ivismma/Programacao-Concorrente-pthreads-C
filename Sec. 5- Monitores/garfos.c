#include "garfos.h"

#define esq(f) (((f)+(qtdFilosofos-1))%qtdFilosofos)
#define dir(f) (((f)+1)%qtdFilosofos)

pthread_mutex_t em = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t *tem_garfos;

void objetivoFilosofo(int *filosofo){
	for(int i = 0; i < 10; ++i){
		sleep(1);
		pega_garfos(*filosofo);
		sleep(1);
		larga_garfos(*filosofo);
	}
}

void garfos_init(int capacidade){
	pthread_mutex_lock(&em);
	if(qtdFilosofos == 0){
		qtdFilosofos = capacidade;
		
		estado_filosofos = (int *) malloc(qtdFilosofos * sizeof(int));
		for(int i = 0; i < qtdFilosofos; ++i)
			estado_filosofos[i] = PENSANDO;
		
		tem_garfos = (pthread_cond_t *) malloc(qtdFilosofos * sizeof(pthread_cond_t));
		for(int i = 0; i < qtdFilosofos; ++i)
			pthread_cond_init( &tem_garfos[i], NULL); // Inicializa condição
	}
	pthread_mutex_unlock(&em);
}

static void pega_garfos(int fid){
	pthread_mutex_lock(&em);
	estado_filosofos[fid] = COM_FOME;
	tenta_atender(fid);
	while(estado_filosofos[fid] == COM_FOME)
		pthread_cond_wait(&tem_garfos[fid], &em);
	pthread_mutex_unlock(&em);
}

static void larga_garfos(int fid){
	pthread_mutex_lock(&em);
	estado_filosofos[fid] = PENSANDO;
	if(estado_filosofos[esq(fid)] == COM_FOME)
		tenta_atender(esq(fid));
	if(estado_filosofos[dir(fid)] == COM_FOME)
		tenta_atender(dir(fid));
	pthread_mutex_unlock(&em);
}

static void tenta_atender(int fid){
	if(estado_filosofos[esq(fid)] == PENSANDO &&
	estado_filosofos[dir(fid)] != COMENDO){
			   
		estado_filosofos[fid] = COMENDO;
		pthread_cond_signal(&tem_garfos[fid]);
		            // acorda caso ele esteja esperando..
	}
	mostraMesaAtual();
}
