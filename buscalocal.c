#include <string.h>

#include "buscalocal.h"
#include "fitness.h"
#include "util.h"

void trocaTimeslots(Problema *p, Individuo *ind, int t1, int t2) {
    int i, step, aux;

    step = p->nDias * p->nPerDias;

    for (i = 0; i < p->nSalas; i++) {
        aux = ind->aula[t1];
        ind->aula[t1] = ind->aula[t2];
        ind->aula[t2] = aux;

        t1 += step;
        t2 += step;
    }

}

void getProxTroca(int *t1, int *t2, int limite) {

    if (*t2 < limite) {
        printf("1\n");
        *t2 = *t2 + 1;
        return;
    } else if (*t2 - *t1 > 1) {
        printf("2\n");
        *t1 = *t1 + 1;
        *t2 = *t1 + 1;
        return;
    } else {
        printf("3\n");
        *t1 = 0;
        *t2 = 1;
        return;
    }
}

void buscaLocalHard(Problema *p, Individuo *ind) {

    int nTimeslots, t1, t2;
    int semMelhorias, nTentativas;
    float nViolacoesHard, nv;

    // quantidade de timeslots
    nTimeslots = p->nDias * p->nPerDias;

    // numero atual de violacoes hard
    nViolacoesHard = somaViolacoesHard(p, ind);

    // numero de iterações sem melhoria
    semMelhorias = 0;

    nTentativas = nTimeslots * (nTimeslots - 1) / 2;
    //printf("nTentativas: %d\n\n\n", nTentativas);

    t1 = t2 = 0;

    do {

        getProxTroca(&t1, &t2, nTimeslots - 1);

        trocaTimeslots(p, ind, t1, t2);

        // numero de violacoes
        nv = somaViolacoesHard(p, ind);
        if (nv < nViolacoesHard) {
            //printf("Melhorou: %f -> %f\n", nViolacoesHard, nv);
            nViolacoesHard = nv;
            semMelhorias = 0;
        } else {
            //printf("Nao melhorou %f\n", nViolacoesHard);
            // desfaz troca
            trocaTimeslots(p, ind, t1, t2);
            semMelhorias++;
        }

    } while (semMelhorias < nTentativas);

    ind->fitness = fitnessHard(p, ind);
}

void trocaAulas(int *timetable, int p1, int p2) {
    int aux = timetable[p1];
    timetable[p1] = timetable[p2];
    timetable[p2] = aux;
}

void buscaLocalSoft(Problema *p, Individuo *ind) {

    int nTimeslots, t1, t2, dimensao;
    int semMelhorias, nTentativas;
    float nViolacoesHard, nv;

    // quantidade de timeslots
    nTimeslots = p->nDias * p->nPerDias;

    dimensao = p->dimensao;

    // numero atual de violacoes hard
    nViolacoesHard = somaViolacoesHard(p, ind);

    // numero de iterações sem melhoria
    semMelhorias = 0;

    nTentativas = nTimeslots * (nTimeslots - 1) / 2;
    //printf("nTentativas: %d\n\n\n", nTentativas);

    t1 = t2 = 0;

    do {

        t1 = rand() % dimensao;

        if (ehAula(p, ind->aula[t1])) {
            t2 = rand() % dimensao;
        } else {
            do {
                t2 = rand() % dimensao;
            } while (!ehAula(p, ind->aula[t2]));
        }

        trocaAulas(ind->aula, t1, t2);

        // numero de violacoes
        nv = somaViolacoesHard(p, ind);
        if (nv < nViolacoesHard) {
            //printf("Melhorou: %f -> %f\n", nViolacoesHard, nv);
            nViolacoesHard = nv;
            semMelhorias = 0;
        } else {
            //printf("Nao melhorou %f\n", nViolacoesHard);
            // desfaz troca
            trocaAulas(ind->aula, t1, t2);
            semMelhorias++;
        }

    } while (semMelhorias < nTentativas);

    ind->fitness = fitnessHard(p, ind);
}

Individuo *buscaLocalSoft2(Problema *p, Individuo *ind, int fase) {

    int i, nTimeslots, t1, t2, dimensao;
    int semMelhorias, nTentativas;
    float fitnessAtual, novoFitness;
    Individuo *novoInd;

    novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);

    novoInd->n = ind->n;
    novoInd->fitness = ind->fitness;
    for (i = 0; i < ind->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

    // quantidade de timeslots
    nTimeslots = p->nDias * p->nPerDias;

    dimensao = p->dimensao;

    // fitness atual
    if (fase == 1) {
        fitnessAtual = fitnessHard(p, novoInd);
    } else {
        fitnessAtual = fitness(p, novoInd);
    }

    // numero de iterações sem melhoria
    semMelhorias = 0;

    nTentativas = 50;
    //printf("nTentativas: %d\n\n\n", nTentativas);

    do {

        t1 = rand() % dimensao;

        if (ehAula(p, ind->aula[t1])) {
            do {
                t2 = rand() % dimensao;
            } while (getTimeslot(p, t1) == getTimeslot(p, t2));
        } else {
            do {
                t2 = rand() % dimensao;
            } while (getTimeslot(p, t1) == getTimeslot(p, t2) || (!ehAula(p, ind->aula[t2])));
        }

        /*if ((getTimeslot(p, t2) == getTimeslot(p, t1)) || (!ehAula(p, ind->aula[t1]) && !ehAula(p, ind->aula[t2]))) {
            printf("Troca: %d[t=%d,a=%d] <-> %d[t=%d,a=%d]\n", t1, getTimeslot(p, t1), ehAula(p, ind->aula[t1]),
                    t2, getTimeslot(p, t2), ehAula(p, ind->aula[t2]));
            exit(1);
        }

        if (ehAula(p, novoInd->aula[t1]) && ehAula(p, novoInd->aula[t2])) {
            if (strcmp(getDisciplina(p, novoInd->aula[t1]), getDisciplina(p, novoInd->aula[t2])) == 0) {
                printf("**************************************\n");
            }
        }*/

        trocaAulas(novoInd->aula, t1, t2);

        // numero de violacoes
        if (fase == 1) {
            novoFitness = fitnessHard(p, novoInd);
        } else {
            novoFitness = fitness(p, novoInd);
        }
        if (novoFitness > fitnessAtual) {
            //printf("Melhorou: %f -> %f\n", fitnessAtual, novoFitness);
            fitnessAtual = novoFitness;
            novoInd->fitness = fitnessAtual;
            semMelhorias = 0;
        } else {
            //printf("Nao melhorou %f\n", fitnessAtual);
            // desfaz troca
            trocaAulas(novoInd->aula, t1, t2);
            semMelhorias++;
        }

    } while (semMelhorias < nTentativas);

    return novoInd;
}

Individuo *buscaLocal2Etapas(Problema *p, Individuo *ind, int fase) {

    int i, nTimeslots, t1, t2, dimensao;
    int semMelhorias, nTentativas;
    float fitnessAtual, novoFitness;
    Individuo *novoInd;

    novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);

    novoInd->n = ind->n;
    novoInd->fitness = ind->fitness;
    for (i = 0; i < ind->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

    // quantidade de timeslots
    nTimeslots = p->nDias * p->nPerDias;

    dimensao = p->dimensao;

    // fitness atual
    if (fase == 1) {
        fitnessAtual = fitnessHard(p, novoInd);
    } else {
        fitnessAtual = fitness(p, novoInd);
    }

    // numero de iterações sem melhoria
    semMelhorias = 0;

    nTentativas = 50;
    //printf("nTentativas: %d\n\n\n", nTentativas);

    do {

        t1 = rand() % dimensao;

        if (ehAula(p, ind->aula[t1])) {
            do {
                t2 = rand() % dimensao;
            } while (getTimeslot(p, t1) == getTimeslot(p, t2));
        } else {
            do {
                t2 = rand() % dimensao;
            } while (getTimeslot(p, t1) == getTimeslot(p, t2) || (!ehAula(p, ind->aula[t2])));
        }

        /*if ((getTimeslot(p, t2) == getTimeslot(p, t1)) || (!ehAula(p, ind->aula[t1]) && !ehAula(p, ind->aula[t2]))) {
            printf("Troca: %d[t=%d,a=%d] <-> %d[t=%d,a=%d]\n", t1, getTimeslot(p, t1), ehAula(p, ind->aula[t1]),
                    t2, getTimeslot(p, t2), ehAula(p, ind->aula[t2]));
            exit(1);
        }

        if (ehAula(p, novoInd->aula[t1]) && ehAula(p, novoInd->aula[t2])) {
            if (strcmp(getDisciplina(p, novoInd->aula[t1]), getDisciplina(p, novoInd->aula[t2])) == 0) {
                printf("**************************************\n");
            }
        }*/

        trocaAulas(novoInd->aula, t1, t2);

        // numero de violacoes
        if (fase == 1) {
            novoFitness = fitnessHard(p, novoInd);
        } else {
            novoFitness = fitness(p, novoInd);
        }
        if (novoFitness > fitnessAtual) {
            //printf("Melhorou: %f -> %f\n", fitnessAtual, novoFitness);
            fitnessAtual = novoFitness;
            novoInd->fitness = fitnessAtual;
            semMelhorias = 0;
        } else {
            //printf("Nao melhorou %f\n", fitnessAtual);
            // desfaz troca
            trocaAulas(novoInd->aula, t1, t2);
            semMelhorias++;
        }

    } while (semMelhorias < nTentativas);

    if (fase == 1) {
        return novoInd;
    }

    semMelhorias = 0;
    nTentativas = 5000;
    do {

        int timeslot = rand() % (p->nDias * p->nPerDias);
        int s1 = rand() % p->nSalas;
        int s2 = rand() % p->nSalas;

        s1 = timeslot + s1 * (p->nDias * p->nPerDias);
        s2 = timeslot + s2 * (p->nDias * p->nPerDias);

        /*if ((getTimeslot(p, t2) == getTimeslot(p, t1)) || (!ehAula(p, ind->aula[t1]) && !ehAula(p, ind->aula[t2]))) {
            printf("Troca: %d[t=%d,a=%d] <-> %d[t=%d,a=%d]\n", t1, getTimeslot(p, t1), ehAula(p, ind->aula[t1]),
                    t2, getTimeslot(p, t2), ehAula(p, ind->aula[t2]));
            exit(1);
        }

        if (ehAula(p, novoInd->aula[t1]) && ehAula(p, novoInd->aula[t2])) {
            if (strcmp(getDisciplina(p, novoInd->aula[t1]), getDisciplina(p, novoInd->aula[t2])) == 0) {
                printf("**************************************\n");
            }
        }*/

        trocaAulas(novoInd->aula, s1, s2);

        // numero de violacoes
        novoFitness = fitness(p, novoInd);

        if (novoFitness > fitnessAtual) {
            //printf("Melhorou: %f -> %f\n", fitnessAtual, novoFitness);
            fitnessAtual = novoFitness;
            novoInd->fitness = fitnessAtual;
            semMelhorias = 0;
        } else {
            //printf("Nao melhorou %f\n", fitnessAtual);
            // desfaz troca
            trocaAulas(novoInd->aula, s1, s2);
            semMelhorias++;
        }

    } while (semMelhorias < nTentativas);

    return novoInd;
}

Individuo *buscaLocal(Problema *p, Individuo *ind, int fase) {

    int i, nTimeslots, t1, t2, dimensao;
    int semMelhorias, nTentativas;
    float fitnessAtual, novoFitness;
    Individuo *novoInd;

    printf("busca local\n");

    novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);

    novoInd->n = ind->n;
    novoInd->fitness = ind->fitness;
    for (i = 0; i < ind->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }
    imprimeIndividuo(novoInd);

    // quantidade de timeslots
    nTimeslots = p->nDias * p->nPerDias;

    dimensao = p->dimensao;

    // fitness atual
    if (fase == 1) {
        fitnessAtual = fitnessHard(p, novoInd);
    } else {
        fitnessAtual = fitness(p, novoInd);
    }

    // numero de iterações sem melhoria
    semMelhorias = 0;

    nTentativas = 100;
    //printf("nTentativas: %d\n\n\n", nTentativas);

    do {

        printf("tentativa...");
        t1 = rand() % dimensao;
        
        if (!ehAula(p, novoInd->aula[t1])) {
             do {
                t2 = rand() % dimensao;
            } while ((!ehAula(p, novoInd->aula[t2])));
        }

        trocaAulas(novoInd->aula, t1, t2);

        // numero de violacoes
        if (fase == 1) {
            novoFitness = fitnessHard(p, novoInd);
        } else {
            novoFitness = fitness(p, novoInd);
        }
        if (novoFitness > fitnessAtual) {
            //printf("Melhorou: %f -> %f\n", fitnessAtual, novoFitness);
            fitnessAtual = novoFitness;
            novoInd->fitness = fitnessAtual;
            semMelhorias = 0;
        } else {
            //printf("Nao melhorou %f\n", fitnessAtual);
            // desfaz troca
            trocaAulas(novoInd->aula, t1, t2);
            semMelhorias++;
        }

        printf("feito.\n");

    } while (semMelhorias < nTentativas);

    return novoInd;
}
