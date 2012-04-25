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

/*
 * ./main instancia populacao txCrossover txMutacao maxTrocasMutacao
 */
int main(int argc, char** argv) {

    int i, j;
    clock_t t1, t2;
    Individuo *ind;


    srand(time(0));

    t1 = clock();

    Problema *p = lerInstancia(argv[1]);


    p->maxIterGrasp = atoi(argv[2]);
    p->buscaLocalGrasp = atoi(argv[3]);
    p->txSwap = atof(argv[4]);
    p->nIterSemMelhoras = atoi(argv[5]);
    p->threshold = atof(argv[6]);

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
    printf("MEDIA: %f\n",p->mediaSolucoes);

    t2 = clock();

    //printf("T: %f\n", (float) (t2 - t1) / CLOCKS_PER_SEC);


    //imprimeResposta(p, ind);


    //ind = geraIndividuoAleatorio(p, 100);
    //printf("%f\n", funcaoObjetivo(p, ind));

    liberaIndividuo(ind);

    desalocaProblema(p);


    return (EXIT_SUCCESS);
}

