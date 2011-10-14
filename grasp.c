#include <stdio.h>
#include <stdlib.h>
#include "grasp.h"
#include "problema.h"

void ordenaDisiciplinas(AuxGrasp *p) {
    printf("GRASP\n");
}

AuxGrasp *geraAuxGrasp(Problema *p) {
    int i, j, k;
    AuxGrasp *auxGrasp;

    auxGrasp = (AuxGrasp*) malloc(sizeof (AuxGrasp));

    auxGrasp->candidatos = (int*) malloc(p->nAulas * sizeof (int));
    auxGrasp->explosao = (int*) malloc(p->nAulas * sizeof (int));
    auxGrasp->nCandidatos = p->nAulas;

    for (i = 0; i < p->nAulas; i++) {
        auxGrasp->explosao[i] = 0;
    }

    i = 0;
    for (j = 0; j < p->nDisciplinas; j++) {
        int nDisc = p->disciplinas[j].aulaInicial;
        for (k = 0; k < p->disciplinas[j].nAulas; k++) {
            auxGrasp->candidatos[i] = nDisc;
            nDisc++;
            i++;
        }
    }

    return auxGrasp;
}

Individuo *geraTimetableVazio(Problema *p) {
    int i;
    Individuo *ind = alocaIndividuo();

    criaIndividuo(ind, p);

    for (i = 0; i < ind->n; i++) {
        ind->aula[i] = p->nAulas + 1; // marca todas as aulas como vazias
    }

    return ind;
}

Individuo *geraSolucaoInicialGrasp(Problema *p) {
    int i, j, k;
    AuxGrasp *auxGrasp;
    Individuo *ind;

    auxGrasp = geraAuxGrasp(p);

    for (i = 0; i < auxGrasp->nCandidatos; i++) {
        printf("%d ", auxGrasp->candidatos[i]);
    }
    printf("\n");
    
    ind = geraTimetableVazio(p);

    while (auxGrasp->nCandidatos > 0) {// enquanto ha candidatos a alocarF

        break;
    }

    return ind;
}
