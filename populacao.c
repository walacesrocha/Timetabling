#include <stdlib.h>

#include "populacao.h"
#include "individuo.h"

/*Estrutura: 

typedef struct populacao{
        Individuo **ind;
        int n;
}Populacao;

 */

Populacao *alocaPopulacao(void) {
    Populacao *pop = (Populacao*) malloc(sizeof (Populacao));
    return pop;
}


////////

void criaPopulacao(Populacao *pop, int tam) {
    pop->n = tam;
    pop->ind = (Individuo**) malloc((pop->n) * sizeof (Individuo*));
}

Populacao *geraPopulacaoAleatoria(Problema *p, int tam, int nTrocas) {
    int i;
    Populacao *pop = alocaPopulacao();
    criaPopulacao(pop, tam);
    for (i = 0; i < tam; i++) {
        pop->ind[i] = geraIndividuoAleatorio(p, nTrocas);
        pop->ind[i]->fitness = rand() % 50;
    }
    return pop;
}

void imprimePopulaao(Populacao *pop) {
    int i;
    for (i = 0; i < pop->n; i++) {
        imprimeIndividuo(pop->ind[i]);
        printf("Fitness: %f\n", pop->ind[i]->fitness);
        printf("\n\n\n");
    }
    printf("\n");
}

void liberaPopulacao(Populacao *pop) {
    int i;
    for (i = 0; i < pop->n; i++) {
        liberaIndividuo(pop->ind[i]);
    }
    free(pop->ind);
    free(pop);
}

/* Inicio Walace */

/*
 * Ordena a população pelo valor do fitness.
 * A ordenação é decrescente e utiliza o método 'insertion sort'
 */
void ordenaPopulacao(Populacao *p) {
    int i, j;
    Individuo *chave;

    for (j = 1; j < p->n; j++) {
        chave = p->ind[j];
        i = j - 1;
        while (i >= 0 && p->ind[i]->fitness < chave->fitness) {
            p->ind[i + 1] = p->ind[i];
            i--;
        }
        p->ind[i + 1] = chave;
    }
}

void imprimeFitness(Populacao *p) {
    int i;
    printf("[ ");
    for (i = 0; i < p->n; i++) {
        printf("%.1f ", p->ind[i]->fitness);
    }
    printf("]\n");
}
