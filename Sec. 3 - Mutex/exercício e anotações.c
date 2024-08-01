/* Se��o 3 (ex. e anota��es)

Crie um programa com 8 threads usando la�o

Metade das threads fazem 10 dep�sitos de R$ 20 a
cada 1 segundo..

A outra metade das threads fazem retiradas de R$ 20
reais a cada 1 segundo.

O saldo nunca pode ficar negativo. Se uma retirada
for solicitada e o saldo � menor do que R$ 20, apenas
o valor dispon�vel � retirado.

Ap�s as 8 threads terminarem, a thread principal (main)
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

	// cria��o das threads de dep�sito.
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&deposita_t[i], NULL, (void *) deposita, NULL);
	
	// cria��o das threads de saque.
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&saca_t[i], NULL, (void *) saca, NULL);
	
	// execu��o das threads.
	for(int i = 0; i < N_THREADS; ++i){
		pthread_join(saca_t[i], NULL);
		pthread_join(deposita_t[i], NULL);
	}

	printf("\nSaldo final: R$ %.2lf\n", saldo);

	return 0;
}

/* Anota��es importantes:

S�o 4 itera��es para saque e 4 para dep�sito, executados *concorrentemente*.

O saldo final � de certa forma aleat�rio devido ao escalonamento das threads,
efetuado pelo sistema operacional.
Se para cada saque, at� o seu momento forem escalonados mais saques para cada 
dep�sito efetuado anteriormente, ent�o haver� um (ou mais) saque(s)
de R$ 0. Ou seja, o saldo ser� positivo porque ainda existem threads de 
dep�sito a serem executadas na fila do CPU e menos de saque.

O saldo nunca ser� negativo porque ir�o ocorrer, inevitavelmente, 10 dep�sitos.
Mas dentre os 10 saques, poder�o existir saques de R$ 0. Ou seja, o m�nimo �
R$ 0 -> (20*10 - 20*10) <-- Isso acontecer� se: Para *cada* saque, o saldo seja de
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
