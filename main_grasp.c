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
#include "gerador.h"

/*Leandro fim*/


void imprimeVetor(int *v, int n) {
    int i;
    for (i = 0; i < n; i++) {
        printf("%d  ", v[i]);
    }
    printf("\n");
}

char *getNomeInstancia(char *nomeArquivo) {
    char *nome = nomeArquivo;
    int i;

    for (i = 0; i < strlen(nomeArquivo); i++, nome++) {
        printf("%s\n", nome);
        if (strncmp("comp", nome, 4) == 0) {
            break;
        }
    }
    printf("Nome: %s\n", nome);

    return nome;
}

void contaAulas(Problema *p, Individuo *ind) {
    int total = 0, i;
    return;

    printf("Contando aulas: \n");
    for (i = 0; i < p->dimensao; i++) {
        printf("%d ", ind->aula[i]);
        if (ehAula(p, ind->aula[i])) {
            total++;
        }
    }

    printf("Total de aulas: %d\n", total);
}

void configuraParametros(Problema *p, int argc, char **argv) {
    p->maxIterGrasp = 200;
    p->threshold = 0.15;
    p->pAproveitamento = 0;
    p->buscaLocalGrasp = 2;
    p->nIterSemMelhoras = 10000;
    p->k = 10;
    p->t0 = 3;
    p->tf = 0.001;
    p->beta = 0.995;
    p->seed = 0;
    p->tempoLimite = -1;
    p->info = 0;

    int i;

    for (i = 2; i < argc; i++) {
        if (strncmp("seed", argv[i], 4) == 0) {
            p->seed = atoi(argv[i] + 5);
        }

        if (strncmp("maxIter", argv[i], 7) == 0) {
            p->maxIterGrasp = atoi(argv[i] + 8);
        }
        if (strncmp("bl", argv[i], 2) == 0) {
            if (strncmp(argv[i] + 3, "sa", 2) == 0) {
                p->buscaLocalGrasp = 6; // SA
            } else if (strncmp(argv[i] + 3, "vns", 3) == 0) {
                p->buscaLocalGrasp = 5; // VNS
            } else if (strncmp(argv[i] + 3, "pf", 2) == 0) {
                p->buscaLocalGrasp = 1; // profundidade
            }
        }

        if (strncmp("n", argv[i], 1) == 0) {
            p->nIterSemMelhoras = atoi(argv[i] + 2);
        }

        if (strncmp("k", argv[i], 1) == 0) {
            p->k = atoi(argv[i] + 2);
        }

        if (strncmp("ti=", argv[i], 3) == 0) {
            p->t0 = atof(argv[i] + 3);
        }

        if (strncmp("tf", argv[i], 2) == 0) {
            p->tf = atof(argv[i] + 3);
        }

        if (strncmp("beta", argv[i], 4) == 0) {
            p->beta = atof(argv[i] + 5);
        }

        if (strncmp("alfa", argv[i], 4) == 0) {
            p->threshold = atof(argv[i] + 5);
        }

        if (strncmp("timeout", argv[i], 7) == 0) {
            p->tempoLimite = atoi(argv[i] + 8);
        }

        if (strncmp("info", argv[i], 4) == 0) {
            p->info = atoi(argv[i] + 5);
        }

        if (strncmp("txap", argv[i], 4) == 0) {
            p->pAproveitamento = atof(argv[i] + 5);
        }

    }

    printf("MaxIter: %d\n", p->maxIterGrasp);
    printf("Threshold: %f\n", p->threshold);
    printf("Tx. Aprov.: %f\n", p->pAproveitamento);
    printf("Busca local: %d\n", p->buscaLocalGrasp);
    printf("n: %d\n", p->nIterSemMelhoras);
    printf("k: %d\n", p->k);
    printf("T0: %f\n", p->t0);
    printf("Tf: %f\n", p->tf);
    printf("Beta: %f\n", p->beta);
    printf("Seed: %d\n", p->seed);
    printf("Timeout: %d\n", p->tempoLimite);
    printf("Info: %d\n", p->info);
}

/*
 * Parametros:
 *      argv[1]: arquivo de instância
 *      maxItex: maximo de iteracoes
 *      alfa: valor de threshold da LRC
 *      bl: busca local: hc,sa
 *      n: numero de iteracoes sem melhora
 *      ti: temperatura inicial
 *      tf: temperatura final
 *      beta: taxa de resfriamento
 *      seed: seed de numero aleatorio
 *      time: tempo limite
 */
int main(int argc, char** argv) {

    int i, j;
    clock_t t1, t2;
    Individuo *ind;
    char arquivoResposta[30];
    FILE *fp;





    //srand(time(0));


    t1 = clock();

    Problema *p = lerInstancia(argv[1]);

    configuraParametros(p, argc, argv);

    srand(p->seed);

    //p->maxIterGrasp = atoi(argv[2]);
    //p->buscaLocalGrasp = atoi(argv[3]);
    //p->txSwap = atof(argv[4]);
    //p->nIterSemMelhoras = atoi(argv[5]);
    //p->threshold = atof(argv[6]);
    //p->pAproveitamento = 0.95;//atof(argv[7]);
    p->pesoHard = 10000;

    /*for (i = 0; i < p->nCurriculos; i++) {
        printf("Curr: %s (%d) pos=%d\n", (p->curriculos + i)->nomeCurriculo,
                (p->curriculos + i)->nDisciplinas, (p->curriculos + i)->pVetor);
    }

    for (j = 0; j < p->nDisciplinas; j++) {
        printf("Disc: %s (%d): \n", (p->disciplinas + j)->nomeDisciplina,
                (p->disciplinas + j)->nCurriculos);

        for (i = 0; i < (p->disciplinas + j)->nCurriculos; i++) {
            Curriculo *c = (p->disciplinas + j)->curriculos[i];
            printf("\tCur: %s (%d) \n", c->nomeCurriculo, c->pVetor);
        }
    }*/


    //ind = geraIndividuoAleatorio(p, 100);

    //imprimeIndividuo3(p,ind);
    //inicializaMatrizesAuxiliares(p, ind);


    //exit(0);
    for (i = 0; i < 0; i++) {
        somaViolacoesSoft2(p, ind,0);

        Neighbour *mov;

        if (1) {
            mov = geraIsolatedLectureMove(p, ind);
        } else {
            mov = geraMove(p, ind);
        }

        imprimeIndividuo3(p, ind);
        imprimeMatCurrDiasPeriodo(p, ind);

        float deltaF = mov->deltaHard + mov->deltaSoft;
        printf("DeltaF=%f (%.1f + %.1f)\n", deltaF, mov->deltaHard, mov->deltaSoft);

        float f1 = funcaoObjetivo2(p, ind, 1);
        float f1Soft = somaViolacoesSoft2(p, ind,0);

        //printf("1) H=%f, S=%f: %d,%d,%d,%d\n", somaViolacoesHard(p, ind), somaViolacoesSoft(p, ind),
        //ind->soft1, ind->soft2, ind->soft3, ind->soft4);
        printf("1) H=%f, S=%f: %d,%d,%d,%d\n", somaViolacoesHard(p, ind), somaViolacoesSoft2(p, ind,0),
                ind->soft1, ind->soft2, ind->soft3, ind->soft4);

        troca_par_completo(p, ind, mov->p1, mov->p2);
        printf("POS: (%d, %d)\n", mov->p1, mov->p2);

        float f2 = funcaoObjetivo2(p, ind, 1);
        float f2Soft = somaViolacoesSoft2(p, ind,0);

        //printf("2) H=%f, S=%f: %d,%d,%d,%d\n", somaViolacoesHard(p, ind), somaViolacoesSoft(p, ind),
        //ind->soft1, ind->soft2, ind->soft3, ind->soft4);
        printf("2) H=%f, S=%f: %d,%d,%d,%d\n", somaViolacoesHard(p, ind), somaViolacoesSoft2(p, ind,0),
                ind->soft1, ind->soft2, ind->soft3, ind->soft4);

        imprimeIndividuo3(p, ind);
        imprimeMatCurrDiasPeriodo(p, ind);

        printf("\n\n%d___________________________________________________________________________________\n\n", i);

        if (f2Soft - f1Soft != mov->deltaSoft) {
            printf("[%d] F1/2: %.1f/%.1f D=%.1f\n", i, f1Soft, f2Soft, mov->deltaSoft);
            scanf("%d", &j);
        } else {
            continue;
        }


    }

    //exit(0);


    /*Gerador *gerador = getGeradorInicial(p->dimensao);
    while (1) {
        incrementaPosGerador(gerador);
    }*/


    /*for(i=0;i<p->nDisciplinas;i++){
        printf("%s\t%d\n",p->disciplinas[i].nomeDisciplina,p->disciplinas[i].nIndisponibilidades);
    }*/

    p->inicio = clock();
    ind = grasp(p);
    p->fim = clock();

    /*p->t0 = atof(argv[2]);
    p->rho = atof(argv[3]);
    p->beta = atof(argv[4]);
    p->aceitaPioraSA = atoi(argv[5]);*/

    //ind = buscaLocalGraspProfundidade(p, ind);
    zeraMatCurrDiasPeriodos(p, ind);
    inicializaMatrizesAuxiliares(p, ind);
    printf("FO: %f\n", funcaoObjetivo2(p, ind, 10000));
    printf("HARD: %f\n", somaViolacoesHard(p, ind));
    printf("SOFT: %f\n", somaViolacoesSoft2(p, ind,0));
    printf("MEDIA: %f\n", p->mediaSolucoes);
    printf("RoomCapacity: %f\n", p->soft1);
    printf("MinWorkDays: %f\n", p->soft2);
    printf("CurrCompactness: %f\n", p->soft3);
    printf("RoomStability: %f\n", p->soft4);
    printf("%.2f <-> %.2f <-> %.2f\n", p->f1, p->f2, p->f3);
    printf("F1 -> F2: %f\n", p->f2 - p->f1);
    printf("F2 -> F3: %f\n", p->f3 - p->f2);
    printf("TEMPO: %f\n", ((double) (p->fim - p->inicio)) / CLOCKS_PER_SEC);
    printf("%d, %d, %d, %d\n", p->vHard, p->semEfeito, p->comPiora, p->comMelhora);

    //imprimeResposta(p,ind, stdout);

    //printf("\nWARNING: ESCOLHA DO P2 NO MOVE COMPACT\n");

    t2 = clock();

    /*for(i=0;i<disc->nSlotsDisponiveis;i++){
        printf("%d ", disc->slotsDisponiveis[i]);
    }
    printf("\n");*/

    //printf("T: %f\n", (float) (t2 - t1) / CLOCKS_PER_SEC);

    /* resposta em arquivo
     * 
    strcpy(arquivoResposta,"sol_");
    fp = fopen(strcat(arquivoResposta,getNomeInstancia(argv[1])), "w");
    imprimeResposta(p, ind, fp);
    fclose(fp);*/


    //ind = geraIndividuoAleatorio(p, 100);
    //printf("%f\n", funcaoObjetivo(p, ind));

    /*Movimento movimentos[7] = {MOVE, SWAP, LECTURE_MOVE, TIME_MOVE, ROOM_MOVE,
        ROOMS, COMPACT};
    int iteracoesMax[7] = {0, 0, 0, 0, 0, 0, 0};

    ind->soft1 = 10;
    ind->soft2 = 0;
    ind->soft3 = 5;
    ind->soft4 = 0;

    configuraQtMovsVNS(p, ind, movimentos, iteracoesMax, 7);

    printf("[");
    for (i = 0; i < 7; i++) {
        printf("%d ", iteracoesMax[i]);

    }
    printf("]\n");*/


    liberaIndividuo(ind);

    desalocaProblema(p);


    return (EXIT_SUCCESS);
}

