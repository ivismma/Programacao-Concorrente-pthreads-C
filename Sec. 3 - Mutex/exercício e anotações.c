/* Seção 3 (ex. e anotações)

Crie um programa com 8 threads usando laço

Metade das threads fazem 10 depósitos de R$ 20 a
cada 1 segundo..

A outra metade das threads fazem retiradas de R$ 20
reais a cada 1 segundo.

O saldo nunca pode ficar negativo. Se uma retirada
for solicitada e o saldo é menor do que R$ 20, apenas
o valor disponível é retirado.

Após as 8 threads terminarem, a thread principal (main)
coloca na tela o saldo final.

O saldo inicia zerado.
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N_THREADS 4

double saldo = 0;

pthread_t saca_t[N_THREADS];
pthread_t deposita_t[N_THREADS];

pthread_mutex_t mutex_saldo = PTHREAD_MUTEX_INITIALIZER;

void deposita(void);
void saca(void);

int main(void){

	// criação das threads de depósito.
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&deposita_t[i], NULL, (void *) deposita, NULL);
	
	// criação das threads de saque.
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&saca_t[i], NULL, (void *) saca, NULL);
	
	// execução das threads.
	for(int i = 0; i < N_THREADS; ++i){
		pthread_join(saca_t[i], NULL);
		pthread_join(deposita_t[i], NULL);
	}

	printf("\nSaldo final: R$ %.2lf\n", saldo);

	return 0;
}

/* Anotações importantes:

São 4 iterações para saque e 4 para depósito, executados *concorrentemente*.

O saldo final é de certa forma aleatório devido ao escalonamento das threads,
efetuado pelo sistema operacional.
Se para cada saque, até o seu momento forem escalonados mais saques para cada 
depósito efetuado anteriormente, então haverá um (ou mais) saque(s)
de R$ 0. Ou seja, o saldo será positivo porque ainda existem threads de 
depósito a serem executadas na fila do CPU e menos de saque.

O saldo nunca será negativo porque irão ocorrer, inevitavelmente, 10 depósitos.
Mas dentre os 10 saques, poderão existir saques de R$ 0. Ou seja, o mínimo é
R$ 0 -> (20*10 - 20*10) <-- Isso acontecerá se: Para *cada* saque, o saldo seja de
pelo menos R$ 20.

A linha 96 permite que meu programa mostre quanto foi sacado de cada tentativa.
*/


void deposita(void){
	for(int i = 0; i < 10; ++i){
		printf("Depositando R$ 20...\n");
		pthread_mutex_lock(&mutex_saldo);
		saldo += 20;
		pthread_mutex_unlock(&mutex_saldo);
		printf("R$ 20 depositados...\n");
		sleep(1);
	}
}

void saca(void){
	for(int i = 0; i < 10; ++i){
		printf("Vai tentar retirar R$ 20...\n");
		pthread_mutex_lock(&mutex_saldo);
		double saque = (saldo >= 20)? 20 : saldo;
		saldo -= saque;
		pthread_mutex_unlock(&mutex_saldo);
		printf("Saque efetuado R$ %lf...\n", saque);
		sleep(1);
	}
	
}
