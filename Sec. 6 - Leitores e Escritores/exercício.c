/*

Crie um monitor para proteger uma tabela a qual é 
acessada por muitos leitores e poucos escritores.

Teste criando 14 threads leitoras e 2 escritoras.
Utilize o mecanismo rwlock.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define N_READER 14
#define N_WRITER  2
#define L 8
#define C 8

void criaTabela(int tabela[L][C]); // cria tabela zerada.
void tarefaEscrita(int destino[L][C]);
void tarefaLeitura(int destino[L][C]);
void escreve(int destino[L][C]);
void consulta(int destino[L][C]);


pthread_t leitor[N_READER];
pthread_t escritor[N_WRITER];

pthread_rwlock_t rw_tabela = PTHREAD_RWLOCK_INITIALIZER;

int main(void){
	
	int tabela[L][C];
	criaTabela(tabela);
	
	// criação das (02) threads de escrita
	for(int i = 0; i < N_WRITER; ++i)
		pthread_create(&escritor[i], NULL, (void *) tarefaEscrita, &tabela);
	
	// criação das (14) threads de leitura
	for(int i = 0; i < N_READER; ++i)
		pthread_create(&leitor[i], NULL, (void *) tarefaLeitura, &tabela);
	
	// thread main() espera threads de leitura terminarem...
	for(int i = 0; i < N_READER; ++i)
		pthread_join(leitor[i], NULL);
	
	printf("\nTodos os leitores terminaram...\n");
	
	// thread main() espera threads de escrita terminarem...
	for(int i = 0; i < N_WRITER; ++i)
		pthread_join(escritor[i], NULL);
	
	return 0;
}

void tarefaEscrita(int destino[L][C]){
	srand(time(NULL));
	for(int i = 0; i < 20; ++i)
		escreve(destino);
}

void tarefaLeitura(int destino[L][C]){
	srand(time(NULL));
	for(int i = 0; i < 10; ++i)
		consulta(destino);
}

void escreve(int destino[L][C]){
		sleep(3);
		printf("\n[%d] Vai tentar escrever (aguardar os leitores)...\n\n", pthread_self());
		pthread_rwlock_wrlock(&rw_tabela);
		int num = 1+rand()%100;
		int i = rand()%L, j = rand()%C;
		printf("[%d] Escrevendo %d na posicao [%d][%d]...\n", pthread_self(), num, i, j);
		destino[i][j] = num; // gera num de 1 a 100 em pos. aleatória
		pthread_rwlock_unlock(&rw_tabela);
}

void consulta(int destino[L][C]){
		sleep(1);
		pthread_rwlock_rdlock(&rw_tabela);
		int i = rand()%L, j = rand()%C;
		int leitura = destino[i][j];
		printf("[%d] Consulta a posicao [%d][%d] e obtem >> %d <<\n", pthread_self(), i, j, leitura);
		pthread_rwlock_unlock(&rw_tabela);
}

void criaTabela(int tabela[L][C]){
	for(int i = 0; i < L; ++i)
		for(int j = 0; j < C; ++j)
			tabela[i][j] = 0;
}
