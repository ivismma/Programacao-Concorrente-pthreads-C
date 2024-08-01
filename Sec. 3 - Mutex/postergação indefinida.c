
// Conceito de Postergação indefinida do sistema linux

// Faz 10 iterações pegando o mutex em cada uma
void codigo_tarefa_1(void){
	for(int ns=0; ns < 10; ++ns) {
		// -------> se não houver *chamada de sistema* entre unlock e lock nesse laço
		// poderá haver *postergação indefinida* de outras threads.
		printf("Tarefa 1: passaram %d iteracoes\n", ns+1);	// Chance de escalonamento
		pthread_mutex_lock(&em);
		printf("Tarefa 1: estah com o Mutex\n");
		sleep(1);
 		pthread_mutex_unlock(&em);
 		// prossegue pra próx iteração até sys. call. --------->
 	}
}

/*
 Exemplo acima: após mutex unlock, executa próx iteração e 
 retoma o próprio lock no loop novamente, impedindo outras 
 threads de trabalharem até que termine totalmente sua execução.
*/
