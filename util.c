#include <time.h>

#include "util.h"
#include "problema.h"
#include "individuo.h"

/*
 *
 * Funções uteis.
 */

/*
 * Retorna qual o dia a partir da posicao no vetor
 */
int getDiaFromPos(int pos, Problema *p) {
    int q = pos % (p->nDias * p->nPerDias);
    q = q / p->nPerDias;

    return q;
}

int getPeriodoFromPos(int pos, Problema *p) {
    int q = pos % (p->nDias * p->nPerDias);
    q = q % p->nPerDias;

    return q;
}

int getRoomFromPos(int pos, Problema *p) {
    int q = pos / (p->nDias * p->nPerDias);

    return q;
}

float tempoDecorrido(clock_t t1) {
    clock_t t2 = clock();

    return (float) (t2 - t1) / CLOCKS_PER_SEC;
}

void imprimeMatrizTimetable(Problema *p, Individuo *ind) {
    int i, s, t;

    i = 0;
    for (s = 0; s < p->nSalas; s++) {
        for (t = 0; t < p->nDias * p->nPerDias; t++) {
            printf("%3d ", ind->aula[i]);
            i++;
        }
        printf("\n");
    }

}

int getTimeslot(Problema *p, int pos) {
    return pos % (p->nDias * p->nPerDias);
}

int esgotouTempoLimite(Problema *p) {
    clock_t agora = clock();

    if ((double) (agora - p->inicio) / CLOCKS_PER_SEC > p->tempoLimite) {
        return 1;
    } else {
        return 0;
    }
}

long getHashCode(Individuo *ind) {
    int i;
    long code;

    code = 0;
    for (i = 0; i < ind->n; i++) {
        code += (i + 1) * ind->aula[i];
    }

    return code;
}
