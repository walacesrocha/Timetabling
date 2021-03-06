/* 
 * File:   main.c
 * Author: walace
 *
 * Created on 26 de Abril de 2011, 13:05
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "genetico.h"
#include "fitness.h"

/*Leandro inicio*/
#include "individuo.h"
#include "populacao.h"
#include "util.h"
#include "buscalocal.h"
#include "fitness.h"
#include "sa.h"
#include "grasp.h"

/*Leandro fim*/

void imprimeVetor(int *v, int n) {
    int i;
    for (i = 0; i < n; i++) {
        printf("%d  ", v[i]);
    }
    printf("\n");
}

void teste(Problema *p, Individuo *ind) {
    int p1, p2;
    int i;

    for (i = 0; i < 15000; i++) {

        p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
        p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

        while (!ehAula(p, ind->aula[p1])) {
            p1++;
            if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p1 = 0;
            }
        }

        while (ehAula(p, ind->aula[p2])) {
            p2++;
            if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p2 = 0;
            }
        }

        if (p2 > p1) {
            printf("[%d,%d]\n", p1, p2);
        } else {
            printf("[%d,%d]\n", p2, p1);
        }
    }



}

/*
 * ./main instancia populacao txCrossover txMutacao maxTrocasMutacao
 */
int main(int argc, char** argv) {

    int i, j;
    clock_t t1, t2;
    Individuo *ind, *viz;

    //testeParametros(argv[1]);
    //exit(0);
    //testeCompleto();
    //exit(0);

    srand(time(0));

    int v1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int v2[10] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};





    t1 = clock();

    Problema *p = lerInstancia(argv[1]);

    ind = geraIndividuoAleatorio(p, 100);
    
    //imprimeIndividuo3(p,ind);
    /*inicializaMatrizesAuxiliares(p, ind);


    //exit(0);
    Neighbour *mov;
    for (i = 0; i < 100000; i++) {

        if ((float)rand()/RAND_MAX < 0.5) {
            mov = geraSwap(p, ind);
        } else {
            mov = geraMove(p, ind);
        }

        troca_par_completo(p, ind, mov->p1, mov->p2);
        somaViolacoesSoft2(p, ind,0);
        
        free(mov);

    }*/

    //exit(0);
    for (i = 0; i < 100000; i++) {
        viz = geraVizinho(p, ind);
        somaViolacoesSoft(p, viz);
        liberaIndividuo(viz);
    }
    exit(0);

    printf("Aulas: %d/%d\n", p->nAulas, p->dimensao);
    printf("Perc. Vazio:: %f\n", (float) (p->dimensao - p->nAulas) / p->dimensao);
    exit(0);

    ind = geraIndividuoAleatorio(p, 10);
    teste(p, ind);
    exit(0);

    p->t0 = atof(argv[2]);
    p->tf = atof(argv[3]);
    p->beta = atof(argv[4]);
    p->txSwap = 0.5;

    ind = simulatedAnnealing(p, NULL);

    printf("FO: %f\n", funcaoObjetivo(p, ind, 10000));
    printf("HARD: %f\n", somaViolacoesHard(p, ind));
    printf("SOFT: %f\n", somaViolacoesSoft(p, ind));
    //printf("Violações Hard: %f\n", somaViolacoesHard(p, ind));
    //printf("Violações Soft: %f\n", somaViolacoesSoft(p, ind));

    exit(0);

    /*printf("População: %d\n", p->tPopulacao);
    printf("Crossover: %f\n", p->txCrossover);
    printf("Mutação: %f\n", p->txMutacao);
    printf("MaxTrocas: %d\n", p->maxTrocasMutacao);
    printf("Tempo Limite: %f\n", p->tempoLimite);*/

    Populacao *populacao = solve(p);

    imprimeResposta(p, populacao->ind[0], stdout);

    printf("%s\n", argv[1]);
    printf("Fitness Hard: %f\n", fitnessHard(p, populacao->ind[0]));
    printf("Fitness: %f\n", fitness(p, populacao->ind[0]));
    printf("Violações Hard: %f\n", somaViolacoesHard(p, populacao->ind[0]));
    printf("Violações Soft: %f\n", somaViolacoesSoft(p, populacao->ind[0]));
    printf("Tempo de Obtencao Melhor Resposta: %f\n", p->tempoMelhorResposta);
    j = 0;
    for (i = 0; i < populacao->n; i++) {
        if (fitnessHard(p, populacao->ind[i]) == 1000) {
            j++;
        }
    }
    printf("Soluções viáveis: %d\n", j);
    printf("\n******************************************************\n");
    //printf("T=%f\n", tempoDecorrido(t1));

    ind = geraIndividuoAleatorio(p, 100);

    //buscaLocalHard(p, ind);

    desalocaProblema(p);


    /*for (a1 = 0; a1 < 15; a1++) {
        for (a2 = 0; a2 < 15; a2++) {

            if (aulasMesmaDisciplina(p, a1, a2)) {
                printf("%d %d ==\n", a1, a2);
            } else {
                printf("%d %d !=\n", a1, a2);
            }
        }
    }*/
    //imprimeInstancia(p);

    /*
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {

            if (disciplinasMesmoCurriculo(p, lista[i], lista[j])) {
                printf("%s %s SIM\n");
            } else {
                printf("%s %s NAO\n");
            }
        }
    }

    for (a1 = 0; a1 < 40; a1++) {
        printf("%d %s\n", a1, getDisciplina(p, a1));
    }

    for (a1 = 0; a1 < 15; a1++) {
        for (a2 = 0; a2 < 24; a2++) {

            if (aulasMesmoCurriculo(p, a1, a2)) {
                printf("%d %d ==\n", a1, a2);
            } else {
                printf("%d %d !=\n", a1, a2);
            }
        }
    }*/




    /*Leandro inicio*/
    //Individuo *indiv;
    //calculaNumeroTotalAulas (p);    
    //indiv = geraIndividuoAleatorio (p, 444);
    //imprimeIndividuo (indiv);
    //printf ("tam indiv: %d\n", indiv->n);
    //Populacao *populacao1 = geraPopulacaoAleatoria(p, 10, 444);
    //imprimePopulaao(populacao1);
    //ordenaPopulacao(populacao1);
    //printf("\n\nOrdenação-------------------------\n\n");
    //imprimePopulaao(populacao1);
    //liberaPopulacao(populacao1);

    /*Leandro fim*/





    /*
    printf("v1: ");
    imprimeVetor(v1,8);
    printf("v2: ");
    imprimeVetor(v2,8);

    procCrossover(v1, v2, 8);

    printf("\n\nCrossover\n\n");

    printf("v1: ");
    imprimeVetor(v1,8);
    printf("v2: ");
    imprimeVetor(v2,8);

    if (checkCrossover(v1,8)){
        printf("OK!\n");
    } else {
        printf("Falha\n");
    }*/

    /*
    printf("Total de aulas: %d\n", p->nAulas);

    printf("P\tD\tP\tS\n");
    for (i = 0; i < p->nDias * p->nSalas * p->nPerDias; i++) {

        printf("%d\t%d\t%d\t%d\n", i, getDiaFromPos(i, p), getPeriodoFromPos(i, p), getRoomFromPos(i, p));
    }

    imprimeResposta(p, populacao1->ind[0]);

    imprimeInstancia(p);

    printf("nAulas: %d",p->nAulas);

    Individuo *indiv = alocaIndividuo();
    criaIndividuo(indiv, p);
    inicializaIndividuo(indiv, p->nAulas);

    //printf ("Num curriculos:%d\n", p->nCurriculos);
    indiv = populacao1->ind[1];
    printf ("Fitness:%f\n", indiv->fitness);
    indiv->fitness = somaViolacoesHard (p, indiv);
    printf ("Fitness:%f\n", indiv->fitness);

    somafs = somaViolacoesSoft(p, indiv);
    printf ("soma restrict soft: %f\n", somafs);
    //printf("Sao do mesmo curriculo: %d\n", aulasMesmoCurriculo(p, 29, 179));

    imprimeIndividuo3(p, indiv);
    //aplicaMutacao (indiv, 1);
    //printf ("Apos mutacao ...\n\n");
    //imprimeIndividuo3 (p, indiv);
    //printf("soma fitness soft: %d\n", somafs);*/

    return (EXIT_SUCCESS);
}

