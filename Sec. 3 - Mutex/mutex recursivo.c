#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void codigo_tarefa_1(void);
void codigo_tarefa_2(void);
double calcula_juro(double x);
void deposita_juros(double y);

pthread_t t1;
pthread_t t2;

double saldo = 10000.0;	

pthread_mutex_t mutex_saldo = PTHREAD_MUTEX_INITIALIZER;
// Obs: n�o h� necessidade de inicializar PTHREAD_MUTEX_INITIALIZER se a solu��o for usada.



/* Anota��es importantes

A O thread executa deposita_juros e realiza o lock mutex do saldo,
mas chama dentro dela a fun��o calcula_juro que por si s� tamb�m
tenta efetuar o lock do mutex, e ent�o ocorre Deadlock.

Solu��o: Alterar o atributo do mutex para P_THREAD_MUTEX_RECURSIVE 
que permite que fun��es chamadas pela mesma thread tenham acesso
ao mutex.
*/

int main(void){
	printf("Inicio\n");
	printf("Saldo inicial %0.2lf\n", saldo);
    
    // Adicionar as pr�ximas 4 linhas altera o atributo do mutex e resolve o problema.
    
	// pthread_mutexattr_t mat;		// Cria uma vari�vel do tipo atributos de mutex
	// pthread_mutexattr_init(&mat);	// Inicializa com valores de atributos default
	// pthread_mutexattr_settype(&mat, PTHREAD_MUTEX_RECURSIVE);	// Muda para atributo RECURSIVE
	// pthread_mutex_init(&mutex_saldo, &mat);		// Muda mutex_saldo para o tipo recursivo
    
	pthread_create(&t1, NULL, (void *) codigo_tarefa_1, NULL);
	pthread_create(&t2, NULL, (void *) codigo_tarefa_2, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("Saldo final ficou %0.2lf\n", saldo);

	printf("Fim\n");
	return(0);
}

double calcula_juro(double x){
	double juros;
	pthread_mutex_lock(&mutex_saldo); // Mas mutex_saldo j� est� trancada! (deadlock)
	juros = saldo * (x / 100.0);      // A execu��o ir� parar para sempre - Com threads esperando pela outra.
	pthread_mutex_unlock(&mutex_saldo);
	return juros;
}


void deposita_juros(double y){
	double juros;
	pthread_mutex_lock(&mutex_saldo);
	juros = calcula_juro(y);
	saldo = saldo + juros;
	pthread_mutex_unlock(&mutex_saldo);
}

void codigo_tarefa_1(void){
	for(int i=0; i < 10; ++i) {
		printf("Tarefa 1 vai depositar 20 reais, i=%d\n",i);
		pthread_mutex_lock(&mutex_saldo);
		saldo = saldo + 20;
		pthread_mutex_unlock(&mutex_saldo);
	}
}

void codigo_tarefa_2(void){
	for(int i=0; i < 10; ++i) {
		printf("Tarefa 2 vai depositar juros de 5%%\n");
		deposita_juros(5);
	}
}
