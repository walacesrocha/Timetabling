#include <stdlib.h>


#include "util.h"

#include "genetico.h"
#include "populacao.h"
#include "individuo.h"
#include "buscalocal.h"

// variável global que indica se um indivíduo viável 
// foi encontrado durante a busca
int viavelEncontrado;

Populacao *solve(Problema *p) {
    int i;
    int geracoes;
    int v = 0;
    clock_t tBase;
    Populacao* populacao;
    float melhorFitnessHard, melhorFitnessSoft;
    Individuo *novoInd;

    viavelEncontrado = 0;

    tBase = clock(); // tempo base para contagem

    // gera a populacao inicial
    populacao = geraPopulacaoAleatoria(p, p->tPopulacao, 100);

    // fitness inicial
    for (i = 0; i < populacao->n; i++) {
        // calcula o fitness de populacao->ind[i]
        populacao->ind[i]->fitness = fitnessHard(p, populacao->ind[i]);
        if (!checkCrossover(populacao->ind[i]->aula, populacao->ind[i]->n)) {
            //printf("Individuo errado:\n");
            //imprimeIndividuo(populacao->ind[i]);
        }
        //printf("Fitness[%d]: %f\n", i, populacao->ind[i]->fitness);
    }

    //printf("ordenando...\n");
    ordenaPopulacao(populacao);

    geracoes = 1;
    melhorFitnessHard = melhorFitnessSoft = 0;

    do {

        //printf("[%d] ", geracoes);

        // SELECAO

        selecao(populacao);

        ordenaPopulacao(populacao);

        //printf("\n\nApos selecao:\n\n");
        //imprimePopulaao(populacao);

        // crossover
        crossover(p, populacao);

        ordenaPopulacao(populacao);

        // a cada 200 gerações verifica se a taxa de mutacao precisa ser aumentada
        if (geracoes % 50 == 0) {
            analisaTxMutacao(p, populacao);
        }
        // mutacao
        mutacao(p, populacao);

        ordenaPopulacao(populacao);

        geracoes++;

        if (!viavelEncontrado) {
            if (populacao->ind[0]->fitness > melhorFitnessHard) {
                novoInd = buscaLocal2Etapas(p, populacao->ind[0], 1);
                melhorFitnessHard = populacao->ind[0]->fitness;
                p->tempoMelhorResposta = tempoDecorrido(tBase);
            }
        } else {
            if (populacao->ind[0]->fitness > melhorFitnessSoft) {
                novoInd = buscaLocal2Etapas(p, populacao->ind[0], 2);
                melhorFitnessSoft = populacao->ind[0]->fitness;
                p->tempoMelhorResposta = tempoDecorrido(tBase);
            }
        }

        if (novoInd != 0) {// fez busca local
            if (novoInd->fitness > populacao->ind[0]->fitness) {
                //printf("Melhorou\n");
                populacao->ind[populacao->n - 1] = novoInd;
                ordenaPopulacao(populacao);
            }
        }

        if (geracoes % 20 == 0) {
            printf("Fitness: %f\n", populacao->ind[0]->fitness);
        }
        //imprimeFitness(populacao);

    } //while (tempoDecorrido(tBase) < p->tempoLimite); // teste de parada
    while (tempoDecorrido(tBase) - p->tempoMelhorResposta < 120); // teste de parada


    return populacao;
}

void analisaTxMutacao(Problema*p, Populacao*pop) {
    float menor, maior;

    menor = pop->ind[pop->n - 1]->fitness; // menor fitness
    maior = pop->ind[0]->fitness; // maior fitness

    //printf("%f ", menor / maior);
    if (menor / maior > 0.98) {
        //printf("Aumentando txMutacao de %f ", p->txMutacao);
        p->txMutacao *= 1.05;
        //printf("para %f \n", p->txMutacao);
    }
}

/*
 * Recalcula o fitness de todos os indivíduos devido a mudança de 
 * função (fitnessHard() -> fitness())
 */
void recalculaFitness(Problema*p, Populacao *pop) {
    int i;
    //printf("\n\nRecalculando fitness\n\n");
    for (i = 0; i < pop->n; i++) {
        pop->ind[i]->fitness = fitness(p, pop->ind[i]);
        //printf("Fitness[%d]=%f\n", i, pop->ind[i]->fitness);
    }
}

/*
 * Recalcula o fitness do indivíduo.
 */
void atualizaFitness(Problema*p, Populacao *pop, Individuo *i) {

    if (viavelEncontrado) {
        i->fitness = fitness(p, i);
    } else {
        i->fitness = fitnessHard(p, i);

        if (i->fitness >= 1000) {
            // individuo viavel encontrado
            viavelEncontrado = 1;
            //printf("\n\n******** VIAVEL ENCONTRADO **********\n\n");

            recalculaFitness(p, pop);
        }
    }
}

/*
 * Faz selecao genetica usando o algoritmo Tournament-3
 */
void selecao(Populacao *populacao) {

    int i, j, p;
    Individuo *i1, *i2, *i3, *melhor; // ponteiros para tournament-3


    for (i = populacao->n / 2; i < populacao->n; i++) {

        // escolhe tres individuos da segunda metade

        p = rand() % populacao->n;
        i1 = populacao->ind[p];

        p = rand() % populacao->n;
        i2 = populacao->ind[p];

        p = rand() % populacao->n;
        i3 = populacao->ind[p];

        // verifica qual dos 3 é o melhor
        if (i1->fitness > i2->fitness) {
            if (i1->fitness > i3->fitness) {
                melhor = i1;
                j = 1;
            } else {
                melhor = i3;
                j = 3;
            }
        } else {
            if (i2->fitness > i3->fitness) {
                melhor = i2;
                j = 2;
            } else {
                melhor = i3;
                j = 3;
            }
        }

        // substitui individuo pelo que foi selecionado
        populacao->ind[i]->fitness = melhor->fitness;
        populacao->ind[i]->n = melhor->n;

        for (j = 0; j < melhor->n; j++) {
            populacao->ind[i]->aula[j] = melhor->aula[j];
        }

    }
}

/*
 * Crossover na metade pior da população usando o algoritmo PMX.
 */
void crossover(Problema *p, Populacao *populacao) {
    int i, p2;

    for (i = populacao->n / 2; i < populacao->n; i++) {

        // probabilidade de fazer o crossover
        if ((float) rand() / RAND_MAX <= p->txCrossover) {

            do {// escolhe parent 2
                p2 = rand() % populacao->n;
            } while (p2 == i);

            // i <-- crossover(i , p2)

            // procedimento de crossover
            procCrossover(populacao->ind[i]->aula, populacao->ind[p2]->aula,
                    populacao->ind[i]->n);

            // recalcula fitness
            atualizaFitness(p, populacao, populacao->ind[i]);

            /*if (!checkCrossover(populacao->ind[i]->aula, populacao->ind[i]->n)) {
                printf("FALHA CROSSOVER\n");
                exit(1);
            }*/

            //imprimeIndividuo(populacao->ind[i]);
        }
    }
}

/*
 * Realiza o procedimento de crossover entre v1 e v2.
 * v1 é substituído pelo filho gerado. PMX
 */
void procCrossover(int *v1, int *v2, int n) {
    int corte1, corte2;
    int aux;
    int j, k;

    corte1 = rand() % n;

    do {// escolhe corte 2 diferente do corte 1
        corte2 = rand() % n;
    } while (corte1 == corte2);

    if (corte2 < corte1) {
        aux = corte1;
        corte1 = corte2;
        corte2 = aux;
    }

    // copia parte antes do corte
    for (j = 0; j <= corte1 - 1; j++) {
        int gene = v1[j];

        // verifica se já na está presente
        while (presente(v2, corte1, corte2, gene)) {
            // procura um substituto para nao repetir
            for (k = corte1; k <= corte2; k++) {
                if (v2[k] == gene) {
                    // gene substituto
                    gene = v1[k];
                    break;
                }
            }
        }

        v1[j] = gene;
    }

    // copia parte depois do corte
    for (j = corte2 + 1; j < n; j++) {
        int gene = v1[j];

        // verifica se já está presente
        while (presente(v2, corte1, corte2, gene) ||
                presente(v1, 0, corte1 - 1, gene)) {
            // procura um substituto para nao repetir
            for (k = corte1; k <= corte2; k++) {
                if (v2[k] == gene) {
                    // gene substituto
                    gene = v1[k];
                    break;
                }
            }
        }

        v1[j] = gene;
    }

    // copia a parte entre os cortes
    for (j = corte1; j <= corte2; j++) {
        v1[j] = v2[j];
    }
}

/*
 * Verifica se o 'valor' esta presente no 'vetor' entre as posicoes 'p1' e 'p2'
 */
int presente(int *vetor, int p1, int p2, int valor) {
    int i;

    for (i = p1; i <= p2; i++) {
        if (vetor[i] == valor) return 1;
    }

    return 0;
}

int checkCrossover(int *v, int n) {

    int i;

    for (i = 0; i < n; i++) {
        if (presente(v, i + 1, n - 1, v[i])) {
            return 0; // valor repetido no vetor
        }

        if (v[i] < 0 || v[i] > n) {
            return 0;
        }
    }

    // passou nos testes
    return 1;

}

void aplicaMutacao(Problema *p, Individuo *indiv, int maxTrocas) {
    int i, pos1, pos2, nTentativas;
    int nTrocas = 1 + numero_aleatorio(maxTrocas);
    float somaViolacoesTroca;

    for (i = 0; i < nTrocas; i++) {
        for (nTentativas = 1; nTentativas > 0 && nTentativas < MAXTENTATIVAS; nTentativas++) {
            pos1 = numero_aleatorio(indiv->n);
            pos2 = numero_aleatorio(indiv->n);
            somaViolacoesTroca = somaViolacoesHardTroca(p, indiv, pos1, pos2);

            //printf("pos1: %d, pos2: %d\n", pos1, pos2); ///////////

            if (ehAula(p, indiv->aula[pos1]) || ehAula(p, indiv->aula[pos2])) {
                troca_par(indiv, pos1, pos2);
                if (somaViolacoesHardTroca(p, indiv, pos1, pos2) > somaViolacoesTroca) {
                    troca_par(indiv, pos1, pos2);
                } else {
                    nTentativas = -33;
                }
            }

            //if (troca_par_restrita(p, indiv, pos1, pos2) ) { nTentativas = -33; }

        }
        //if (nTentativas >= MAXTENTATIVAS) printf("ultrapassou maxtentativas\n"); ///////////
    }
}

void mutacao(Problema *p, Populacao *populacao) {
    int i;
    float a;
    for (i = populacao->n / 4; i < populacao->n; i++) {
        a = (float) rand() / RAND_MAX;
        //printf("p=%f", a);
        if (a <= p->txMutacao) {
            aplicaMutacao(p, populacao->ind[i], p->maxTrocasMutacao);

            // recalcula fitness
            atualizaFitness(p, populacao, populacao->ind[i]);
        }
    }
}




