#ifndef __POPULACAO_h
#define __POPULACAO_h

#include "individuo.h"

typedef struct populacao{
	Individuo **ind;
        int n;
}Populacao;



Populacao *alocaPopulacao (void);

void criaPopulacao (Populacao *pop, int tam);

/*gera uma populacao de tam individuos gerados aleatoriamente com nTrocas trocas entre pares aleatorios de posicoes do vetor*/
Populacao *geraPopulacaoAleatoria (Problema *p, int tam, int nTrocas);

void imprimePopulaao (Populacao *pop);

void liberaPopulacao (Populacao *pop);

/* Inicio Walace */
void ordenaPopulacao(Populacao *p);
void imprimeFitness(Populacao *p);

/* Fim Walace */

#endif
