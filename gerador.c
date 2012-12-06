/* 
 * File:   gerador.c
 * Author: walace
 *
 * Created on 6 de Dezembro de 2012, 12:02
 */

#include <stdio.h>
#include <stdlib.h>

#include "individuo.h"
#include "gerador.h"
#include "auxiliar.h"

Gerador *getGeradorInicial(int n) {
    Gerador *gerador = (Gerador*) malloc(sizeof (Gerador));
    gerador->p1 = rand() % (n - 1);
    gerador->p2 = gerador->p1 + 1;
    gerador->n = n;

    return gerador;
}

Individuo *getProxVizinho(Problema*p, Individuo *ind, Gerador *gerador) {

    Individuo *novoInd = copiaIndividuo(p, ind);

    do {

        if (!ehAula(p, novoInd->aula[gerador->p1]) && !ehAula(p, novoInd->aula[gerador->p2])) {
            incrementaPosGerador(gerador);
            continue;
        }

        if (aulasMesmaDisciplina3(p, novoInd->aula[gerador->p1], novoInd->aula[gerador->p2])) {
            incrementaPosGerador(gerador);
            continue;
        }

        troca_par(novoInd, gerador->p1, gerador->p2);
        if (somaViolacoesHardTroca(p, novoInd, gerador->p1, gerador->p2) > 0) {
            // desfaz
            troca_par(novoInd, gerador->p1, gerador->p2);
            incrementaPosGerador(gerador);
            continue;
        }
        break;
    } while (1);

    incrementaPosGerador(gerador);

    return novoInd;
}

void incrementaPosGerador(Gerador *gerador) {
    gerador->p2++;

    if (gerador->p2 == gerador->n) {
        gerador->p2 = 1;
        gerador->p1++;

        if (gerador->p1 == gerador->n) {
            gerador->p1 = 0;
        }
    }

    //printf("(%d, %d) [%d]\n",gerador->p1,gerador->p2,gerador->n);
}

void imprimeGerador(Gerador *gerador) {
    printf("(%d, %d)\n", gerador->p1, gerador->p2);
}

void configuraGeradores(Individuo *ind, Gerador **geradores, int nGeradores) {
    int i;
    Gerador *gerador;

    printf("NConflitos: %d, Geradores: %d\n", ind->nConflitos, nGeradores);
    for (i = 0; i < ind->nConflitos; i++) {
        if (i == nGeradores)break;

        gerador = geradores[i];
        gerador->p1 = ind->posConflitos[i];
        gerador->p2 = 0;

        imprimeGerador(gerador);
    }

}

