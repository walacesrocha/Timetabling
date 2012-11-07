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

/*
 * ./main instancia populacao txCrossover txMutacao maxTrocasMutacao
 */
int main(int argc, char** argv) {

    int i, j;
    clock_t t1, t2;
    Individuo *ind;
    char arquivoResposta[30];
    FILE *fp;


    srand(time(0));

    t1 = clock();

    Problema *p = lerInstancia(argv[1]);

    p->maxIterGrasp = atoi(argv[2]);
    p->buscaLocalGrasp = atoi(argv[3]);
    p->txSwap = atof(argv[4]);
    p->nIterSemMelhoras = atoi(argv[5]);
    p->threshold = atof(argv[6]);
    p->k = 10;


    /*for(i=0;i<p->nDisciplinas;i++){
        printf("%s\t%d\n",p->disciplinas[i].nomeDisciplina,p->disciplinas[i].nIndisponibilidades);
    }*/
    ind = grasp(p);

    /*p->t0 = atof(argv[2]);
    p->rho = atof(argv[3]);
    p->beta = atof(argv[4]);
    p->aceitaPioraSA = atoi(argv[5]);*/

    //ind = buscaLocalGraspProfundidade(p, ind);
    printf("FO: %f\n", funcaoObjetivo(p, ind));
    printf("HARD: %f\n", somaViolacoesHard(p, ind));
    printf("SOFT: %f\n", somaViolacoesSoft(p, ind));
    printf("MEDIA: %f\n", p->mediaSolucoes);
    printf("RoomCapacity: %f\n", p->soft1);
    printf("MinWorkDays: %f\n", p->soft2);
    printf("CurrCompactness: %f\n", p->soft3);
    printf("RoomStability: %f\n", p->soft4);
    printf("MOVES: %d\n", p->nMoves);
    printf("SWAPS: %d\n", p->nSwaps);

    printf("F1 -> F2: %f\n", p->f2 - p->f1);
    printf("F2 -> F3: %f\n", p->f3 - p->f2);

    t2 = clock();

    Disciplina *disc = acessaDisciplina(p, 4);

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

