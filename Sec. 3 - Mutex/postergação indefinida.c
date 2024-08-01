
// Conceito de Posterga��o indefinida do sistema linux

// Faz 10 itera��es pegando o mutex em cada uma
void codigo_tarefa_1(void){
	for(int ns=0; ns < 10; ++ns) {
		// -------> se n�o houver *chamada de sistema* entre unlock e lock nesse la�o
		// poder� haver *posterga��o indefinida* de outras threads.
		printf("Tarefa 1: passaram %d iteracoes\n", ns+1);	// Chance de escalonamento
		pthread_mutex_lock(&em);
		printf("Tarefa 1: estah com o Mutex\n");
		sleep(1);
 		pthread_mutex_unlock(&em);
 		// prossegue pra pr�x itera��o at� sys. call. --------->
 	}
}

/*
 Exemplo acima: ap�s mutex unlock, executa pr�x itera��o e 
 retoma o pr�prio lock no loop novamente, impedindo outras 
 threads de trabalharem at� que termine totalmente sua execu��o.
*/
