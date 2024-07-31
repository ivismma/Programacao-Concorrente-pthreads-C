/* Se��o 2

Crie um programa com 5 threads usando la�o e passe
para cada uma o nome da respectiva thread.

Cada thread dorme por 10 segundos de 1 em 1 segundo.

Cada thread retorna ao t�rmino um c�digo de ERRO caso
o seu nome n�o inicie com letra mai�scula, e SUCESSO
caso inicie.
*/

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <locale.h>
#include <pthread.h>

#define N_THREADS 5

pthread_t thread[N_THREADS];

int SUCESSO = 10;
int ERRO    = 11;

typedef struct {
	int segundos;
	char *nome;
} param_t;

bool maiusculo(char c){ return (c >= 'A' && c <= 'Z'); }
void tarefa(param_t *param);

int main(void){
	setlocale(LC_ALL, "Portuguese");
	
	param_t parametros[N_THREADS];  // par�metros de cada thread.
	int *threadret[N_THREADS];       // vari�veis de retorno das threads.
	//  ( array de ponteiros !!! )
	
	
	// inicializa��o de vari�vel das threads
	for(int i = 0; i < N_THREADS; ++i)
		parametros[i].segundos = 10;
	
	// defini��o de nome das threads
	parametros[0].nome = "Task 1";   // SUCESSO
	parametros[1].nome = "Tarefa 2"; // SUCESSO
	parametros[2].nome = "tarefa 3"; // ERRO
	parametros[3].nome = "task 4";   // ERRO
	parametros[4].nome = "Task 5";   // SUCESSO
	
	// cria��o de threads
	for(int i = 0; i < N_THREADS; ++i)
		pthread_create(&thread[i], NULL, (void *) tarefa, &parametros[i]); 
	
	// execu��o de threads
	for(int i = 0; i < N_THREADS; ++i)
		pthread_join(thread[i], (void **) &threadret[i]);
	
	// exibi��o do retorno de cada thread
	for(int i = 0; i < N_THREADS; ++i){
		printf("Thread[%d] \"%s\" retornou %d ", i, parametros[i].nome, *(threadret[i]));
		                                       //  >> dereferencia o pointer no array <<
	if(*(threadret[i]) == 10)
		printf("(SUCESSO)\n");
	else
		printf("(ERRO)\n");
	}
	
	return 0;
}

void tarefa(param_t *param){
	for(int i = 0; i < param->segundos; ++i){
		sleep(1);
		printf("[%s] est� em %d segundos...\n", param->nome, i+1);
	}
	if(maiusculo(param->nome[0]))
		pthread_exit(&SUCESSO);
	else
		pthread_exit(&ERRO);
}

