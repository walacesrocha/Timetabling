#include "problema.h"
#include "populacao.h"
#include "fitness.h"


Populacao *solve(Problema *p);
void selecao(Populacao *populacao);
void crossover(Problema*p, Populacao *populacao);
void procCrossover(int *v1, int *v2, int n);
int checkCrossover(int *v, int n);
//aplica mutacao em um individuo
void aplicaMutacao2(Individuo *indiv, int maxTrocas);
void aplicaMutacao(Problema *p, Individuo *indiv, int maxTrocas);
//aplica mutacao segundo uma taxa em 3/4 da populacao
void mutacao(Problema*p, Populacao *pop);
void analisaTxMutacao(Problema*p, Populacao*pop);
