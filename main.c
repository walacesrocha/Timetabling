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

/*Leandro fim*/

void imprimeVetor(int *v, int n) {
    int i;
    for (i = 0; i < n; i++) {
        printf("%d  ", v[i]);
    }
    printf("\n");
}

void testeParametros(char *instancia) {
    int pop, maxTrocas;
    float txCrossover, txMutacao;
    float fHard, f;
    Problema *p = lerInstancia(instancia);
    insereCurriculosNasDisciplinas(p);
    insereIndisponibilidadesNasDisciplinas(p);
    ordenaDisciplinasPorRestricoes(p);


    for (pop = 10; pop <= 50; pop += 10) {
        for (txCrossover = 0.1; txCrossover <= 0.9; txCrossover += 0.1) {
            for (txMutacao = 0.1; txMutacao <= 0.9; txMutacao += 0.1) {
                for (maxTrocas = 1; maxTrocas <= 5; maxTrocas++) {
                    p->tPopulacao = pop;
                    p->txCrossover = txCrossover;
                    p->txMutacao = txMutacao;
                    p->maxTrocasMutacao = maxTrocas;
                    p->tempoLimite = 600;

                    Populacao *populacao = solve(p);

                    f = fitness(p, populacao->ind[0]);
                    fHard = fitnessHard(p, populacao->ind[0]);

                    printf("%d %f %f %d %f %f\n", pop, txCrossover, txMutacao, maxTrocas, fHard, f);

                }
            }
        }
    }

}

void testeCompleto() {
    char instancias[22][100] = {"toy", "comp01"};

    int i, j;
    int nExecucoes = 5;
    Populacao *pop;


    for (i = 0; i < 2; i++) {
        char base[50] = "instancias/";
        strcat(base, instancias[i]);
        strcat(base, ".ctt");

        printf("%s:", base);
        Problema *p = lerInstancia(base);
        insereCurriculosNasDisciplinas(p);
        insereIndisponibilidadesNasDisciplinas(p);
        ordenaDisciplinasPorRestricoes(p);
        for (j = 0; j < nExecucoes; j++) {
            p->tPopulacao = 30;
            p->txCrossover = 0.7;
            p->txMutacao = 0.5;
            p->maxTrocasMutacao = 1;
            p->tempoLimite = 5;

            pop = solve(p);

            printf("%f\n", pop->ind[0]->fitness);

        }
    }
}

/*
 * ./main instancia populacao txCrossover txMutacao maxTrocasMutacao
 */
int main(int argc, char** argv) {

    int a1, a2, i, j, somafs;
    char *lista[10] = {"c0001", "c0002", "c0004", "c0005", "c0014", "c0015", "c0016", "c0017", "c0078", "c0071"};
    int v1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int v2[8] = {3, 7, 5, 1, 6, 8, 2, 4};
    clock_t t1, t2;
    Individuo *ind;

    //testeParametros(argv[1]);
    //exit(0);
    //testeCompleto();
    //exit(0);

    srand(time(0));

    t1 = clock();

    Problema *p = lerInstancia(argv[1]);


    //p->tPopulacao = atoi(argv[2]);
    //p->txCrossover = atof(argv[3]);
    //p->txMutacao = atof(argv[4]);
    //p->maxTrocasMutacao = atoi(argv[5]);
    //p->tempoLimite = atof(argv[6]);

    ind = simulatedAnnealing(p);
    printf("%f", funcaoObjetivo(p, ind));
    //printf("Violações Hard: %f\n", somaViolacoesHard(p, ind));
    //printf("Violações Soft: %f\n", somaViolacoesSoft(p, ind));

    exit(0);

    /*printf("População: %d\n", p->tPopulacao);
    printf("Crossover: %f\n", p->txCrossover);
    printf("Mutação: %f\n", p->txMutacao);
    printf("MaxTrocas: %d\n", p->maxTrocasMutacao);
    printf("Tempo Limite: %f\n", p->tempoLimite);*/

    Populacao *populacao = solve(p);

    imprimeResposta(p, populacao);

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

    imprimeResposta(p, populacao1);

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

