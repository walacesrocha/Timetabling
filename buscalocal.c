#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "buscalocal.h"
#include "fitness.h"
#include "util.h"
#include "sa.h"

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

    nTentativas = 0;
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

Individuo *buscaLocalTimeslot(Problema *p, Individuo *ind) {

    int i, nTimeslots;
    int s1, s2;
    int melhorou, nTentativas;
    float fitnessAtual, novoFitness;
    Individuo *novoInd;

    printf("busca local timeslot\n");

    novoInd = copiaIndividuo(p, ind);

    // quantidade de timeslots
    nTimeslots = p->nDias * p->nPerDias;

    // fitness atual
    fitnessAtual = somaViolacoesSoft(p, novoInd);

    nTentativas = 500;
    //printf("nTentativas: %d\n\n\n", nTentativas);

    for (i = 0; i < nTimeslots;) {

        melhorou = 0;
        for (s1 = 0; s1 < p->nSalas; s1++) {
            for (s2 = s1 + 1; s2 < p->nSalas; s2++) {



                /*/s2 = rand() % p->nSalas;
                while (s2 == s1) {
                    s2 = rand() % p->nSalas;
                }*/

                int p1 = s1 * nTimeslots + i;
                int p2 = s2 * nTimeslots + i;

                trocaAulas(novoInd->aula, p1, p2);

                // numero de violacoes
                novoFitness = somaViolacoesSoft(p, novoInd);
                if (novoFitness < fitnessAtual) {
                    //printf("Melhorou: %f -> %f\n", fitnessAtual, novoFitness);
                    fitnessAtual = novoFitness;
                    novoInd->fitness = fitnessAtual;
                    melhorou = 1;
                    printf("TS: %f\n", novoFitness);
                } else {
                    //printf("Nao melhorou %f\n", fitnessAtual);
                    // desfaz troca
                    trocaAulas(novoInd->aula, p1, p2);
                }

            }
        }

        if (!melhorou) {
            // vai para proximo timeslot
            i++;
        }


    }

    return novoInd;
}

ListaVizinhos * alocaListaVizinhos(int k) {
    ListaVizinhos *listaVizinhos;

    listaVizinhos = (ListaVizinhos*) malloc(sizeof (ListaVizinhos));

    listaVizinhos->pos1 = (int*) malloc(k * sizeof (int));
    listaVizinhos->pos2 = (int*) malloc(k * sizeof (int));

    return listaVizinhos;
}

void desalocaListaVizinhos(ListaVizinhos * listaVizinhos) {
    free(listaVizinhos->pos1);
    free(listaVizinhos->pos2);
    free(listaVizinhos);
}

void geraListaVizinhos(Problema *p, Individuo*ind, ListaVizinhos * listaVizinhos) {
    int i, p1, p2, aux;

    for (i = 0; i < p->k; i++) {

        /*** MOVE EVENT ***/
        if (((float) rand()) / RAND_MAX < 0.5) {
            p->nMoves++;
move:

            p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
            p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

            //printf("posicoes sorteadas\n");

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

            // faz a troca das posicoes
            troca_par(ind, p1, p2);
            //aux = ind->aula[p1];
            //ind->aula[p1] = novoInd->aula[p2];
            //novoInd->aula[p2] = aux;

            if (somaViolacoesHardTroca(p, ind, p1, p2) > 0) {
                troca_par(ind, p1, p2);
                //printf("voltando move\n");
                goto move;
            }

        } else {
            p->nSwaps++;
            /*** SWAP EVENT ***/
swap:

            p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
            p2 = rand() % p->dimensao; // posicao que irá apontar outro horario de aula

            //printf("posicoes sorteadas\n");

            while (!ehAula(p, ind->aula[p1])) {
                p1++;
                if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
                    p1 = 0;
                }
            }

            while (!ehAula(p, ind->aula[p2])) {
                p2++;
                if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                    p2 = 0;
                }
            }

            // faz a troca das posicoes
            troca_par(ind, p1, p2);
            //aux = novoInd->aula[p1];
            //novoInd->aula[p1] = novoInd->aula[p2];
            //novoInd->aula[p2] = aux;

            if (somaViolacoesHardTroca(p, ind, p1, p2) > 0) {
                troca_par(ind, p1, p2);
                //printf("voltando swap\n");
                goto swap;
            }
        }

        troca_par(ind, p1, p2);
        listaVizinhos->pos1[i] = p1;
        listaVizinhos->pos2[i] = p2;


    }


}

int getTimeSlotVazioParaSala(Problema *p, Individuo *ind, int sala, int timeSlotExcluido) {
    int pLivres[p->nDias * p->nPerDias];
    int i, j, pos;
    int totalHorarios;

    totalHorarios = p->nDias * p->nPerDias;
    j = 0;
    for (i = 0; i < totalHorarios; i++) {
        if (i == timeSlotExcluido) {
            continue;
        }
        pos = sala * totalHorarios + i;

        if (!ehAula(p, ind->aula[pos])) {
            pLivres[j] = i;
            j++;
        }
    }

    /*printf("Livres[");
    for (i = 0; i < j; i++) {
        printf("%d ", pLivres[i]);
    }
    printf("]\n");*/

    if (j == 0) {
        // sem timeslot vazio
        return -1;
    }

    return pLivres[rand() % j];

}

int getPosVaziaParaSala(Problema *p, Individuo *ind, int sala, int posAtual) {
    int pLivres[p->nDias * p->nPerDias];
    int i, j;
    int totalHorarios;
    int aula = ind->aula[posAtual];

    Disciplina *disc = acessaDisciplina(p, aula);

    totalHorarios = p->nDias * p->nPerDias;
    j = 0;
    for (i = 0; i < disc->nSlotsDisponiveis; i++) {
        int pos2 = disc->slotsDisponiveis[i];

        if (pos2 == posAtual) continue;

        if (getSalaFromPos(p, pos2) != sala) {
            continue;
        }

        if (!ehAula(p, ind->aula[pos2])) {
            pLivres[j] = pos2;
            j++;
        }
    }

    /*printf("Livres[");
    for (i = 0; i < j; i++) {
        printf("%d ", pLivres[i]);
    }
    printf("]\n");*/

    if (j == 0) {
        // sem timeslot vazio
        return -1;
    }

    return pLivres[rand() % j];

}

int getSalaVazioParaTimeSlot(Problema *p, Individuo *ind, int salaExcluida, int timeslot) {
    int sLivres[p->nSalas];
    int i, j, pos;

    j = 0;
    for (i = 0; i < p->nSalas; i++) {
        if (i == salaExcluida) {
            continue;
        }
        pos = i * (p->nDias * p->nPerDias) + timeslot;

        if (!ehAula(p, ind->aula[pos])) {
            sLivres[j] = i;
            j++;
        }
    }

    /*printf("Livres[");
    for (i = 0; i < j; i++) {
        printf("%d ", sLivres[i]);
    }
    printf("]\n");*/

    if (j == 0) {
        // sem timeslot vazio
        return -1;
    }

    return sLivres[rand() % j];
}

int getSalaAdequada(Problema *p, Disciplina * disc) {
    int sLivres[p->nSalas];
    int i, j;

    j = 0;
    for (i = 0; i < p->nSalas; i++) {

        // escolhe sala adequada
        if (p->salas[i].capacidade >= disc->nAlunos) {
            sLivres[j] = i;
            j++;
        }
    }

    /*printf("[%d] Adequadas[", disc->nAlunos);
    for (i = 0; i < j; i++) {
        printf("%d ", sLivres[i]);
    }
    printf("]\n");*/

    if (j == 0) {
        // sem timeslot vazio
        return -1;
    }

    return sLivres[rand() % j];
}

Individuo * geraVizinho3(Problema *p, Individuo * ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

    float prob = ((float) rand()) / RAND_MAX;

    if (prob < 0.22) {
lecture_move:

        p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
        p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

        //printf("posicoes sorteadas\n");

        while (!ehAula(p, novoInd->aula[p1])) {
            p1++;
            if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p1 = 0;
            }
        }

        while (ehAula(p, novoInd->aula[p2])) {
            p2++;
            if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p2 = 0;
            }
        }

        // faz a troca das posicoes
        aux = novoInd->aula[p1];
        novoInd->aula[p1] = novoInd->aula[p2];
        novoInd->aula[p2] = aux;

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando move\n");
            goto lecture_move;
        }

    } else if (prob < 0.44) {
        p->nMoves++;
timemove:

        p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula

        //printf("posicoes sorteadas\n");

        while (!ehAula(p, novoInd->aula[p1])) {
            p1++;
            if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p1 = 0;
            }
        }

        int timeslot = getTimeSlotFromPos(p1, p);

        int sala = getSalaFromPos(p, p1);
        int timeSlotVazio = getTimeSlotVazioParaSala(p, novoInd, sala, timeslot);

        p2 = getPosVaziaParaSala(p, novoInd, sala, p1);

        //imprimeIndividuo2(p,novoInd);
        //printf("Sala: %d\n", sala);
        //printf("TimeSlot: %d\n", timeslot);
        //printf("TVazio: %d\n", timeSlotVazio);
        //scanf("%d\n",&p2);

        if (p2 == -1) {
            goto timemove;
        }

        //p2 = sala * (p->nDias * p->nPerDias) + timeSlotVazio;



        /*if (timeslot == p->nDias * p->nPerDias) {
            p2 = p1 - timeslot;
        } else {
            p2 = p1 + 1;
        }
        int tentativas = 0;
        while (ehAula(p, novoInd->aula[p2])) {
            p2++;
            tentativas++;
            if (getTimeSlotFromPos(p2, p) == 0) {// volta ao inicio do vetor 'aula'
                p2 = p2 - (p->nDias * p->nPerDias);
            }

            if (tentativas > p->nDias * p->nPerDias) {
                goto timemove;
            }
        }*/

        // faz a troca das posicoes
        troca_par(novoInd, p1, p2);

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando move\n");
            goto timemove;
        }
    } else if (prob < 0.66) {
        p->nSwaps++;
        /*** SWAP EVENT ***/
roommove:

        p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula

        //printf("posicoes sorteadas\n");

        while (!ehAula(p, novoInd->aula[p1])) {
            p1++;
            if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p1 = 0;
            }
        }

        int timeslot = getTimeSlotFromPos(p1, p);

        int sala = getSalaFromPos(p, p1);
        int salaVazia = getSalaVazioParaTimeSlot(p, novoInd, sala, timeslot);

        /*imprimeIndividuo2(p, novoInd);
        printf("Sala: %d\n", sala);
        printf("TimeSlot: %d\n", timeslot);
        printf("SalaVazia: %d\n", salaVazia);
        if (salaVazia == -1) {
            scanf("%d\n", &p2);
        }*/

        if (salaVazia == -1) {
            goto roommove;
        }

        p2 = salaVazia * (p->nDias * p->nPerDias) + timeslot;



        /*if (timeslot == p->nDias * p->nPerDias) {
            p2 = p1 - timeslot;
        } else {
            p2 = p1 + 1;
        }
        int tentativas = 0;
        while (ehAula(p, novoInd->aula[p2])) {
            p2++;
            tentativas++;
            if (getTimeSlotFromPos(p2, p) == 0) {// volta ao inicio do vetor 'aula'
                p2 = p2 - (p->nDias * p->nPerDias);
            }

            if (tentativas > p->nDias * p->nPerDias) {
                goto timemove;
            }
        }*/

        // faz a troca das posicoes
        troca_par(novoInd, p1, p2);

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando move\n");
            goto roommove;
        }
    } else if (prob < 0.88) {
rooms:
        p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula

        //printf("posicoes sorteadas\n");

        while (!ehAula(p, novoInd->aula[p1])) {
            p1++;
            if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p1 = 0;
            }
        }

        Disciplina *disc = acessaDisciplina(p, novoInd->aula[p1]);
        int salaAdequada = getSalaAdequada(p, disc);

        /*imprimeIndividuo2(p, novoInd);
        printf("Disc: %s, [%d, %d]\n", disc->nomeDisciplina, disc->aulaInicial, disc->aulaInicial + disc->nAulas - 1);
        //printf("TimeSlot: %d\n", timeslot);
        printf("SalaAdequada: %d\n", salaAdequada);
        //if (salaVazia == -1) {
        scanf("%d\n", &p2);
        //}*/

        if (salaAdequada == -1) {
            goto rooms;
        }

        int nTrocas = 0;
        for (i = 0; i < disc->nSlotsDisponiveis; i++) {
            int pos = disc->slotsDisponiveis[i];

            if (ehAula(p, novoInd->aula[pos])) {
                if (novoInd->aula[pos] >= disc->aulaInicial &&
                        novoInd->aula[pos] < disc->aulaInicial + disc->nAulas) {

                    int timeslot = getTimeSlotFromPos(pos, p);

                    p2 = salaAdequada * (p->nDias * p->nPerDias) + timeslot;

                    //printf("%d <-> %d Conf=%d\n", novoInd->aula[pos], novoInd->aula[p2], aulasConflitantes(p, novoInd->aula[pos], novoInd->aula[p2]));



                    //printf("Trocando... => [%d,%d] (%d,%d) S=(%d,%d)\n", getTimeSlotFromPos(pos, p), getTimeSlotFromPos(p2, p), pos, p2, getSalaFromPos(p, pos), getSalaFromPos(p, p2));

                    troca_par(novoInd, pos, p2);

                    /*if (somaViolacoesHardTroca(p, novoInd, pos, p2) > 0) {
                        printf("Oh my...\n");
                        printf("%f\n", funcaoObjetivo(p, novoInd));
                        exit(1);
                    }*/
                    nTrocas++;
                }
            }

        }

        /*printf("Trocas: %d/%d\n", nTrocas, disc->nAulas);
        imprimeIndividuo2(p, novoInd);
        scanf("%d\n", &p2);*/


        // faz a troca das posicoes
        /*troca_par(novoInd, p1, p2);

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando move\n");
            goto rooms;
        }*/

    } else {
        int nTentativas = 0;
compact:
        p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula

        //printf("posicoes sorteadas\n");

        while (!ehAula(p, novoInd->aula[p1])) {
            p1++;
            if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p1 = 0;
            }
        }

        Disciplina *disc = acessaDisciplina(p, novoInd->aula[p1]);
        Curriculo *curr = disc->curriculos[rand() % disc->nCurriculos];

        int dia = getDia(p, p1);
        int horario = getPeriodo(p, p1);

        somaViolacoesSoft(p, novoInd);
        int anterior = novoInd->soft3;


        if (aulaIsolada(p, novoInd, p1, dia, horario)) {

            for (i = 0; i < disc->nSlotsDisponiveis; i++) {
                if (!ehAula(p, novoInd->aula[i])) {
                    troca_par(novoInd, i, p1);

                    if (somaViolacoesHardTroca(p, novoInd, p1, i) > 0) {
                        troca_par(novoInd, i, p1);
                        //printf("voltando move\n");
                        continue;
                    }

                    dia = getDia(p, i);
                    horario = getPeriodo(p, i);

                    if (aulaIsolada(p, novoInd, i, dia, horario)) {
                        // desfaz pq na tirou violação
                        troca_par(novoInd, i, p1);
                    } else {
                        somaViolacoesSoft(p, novoInd);
                        //printf("diminui compact %d %d\n", anterior, novoInd->soft3);
                        //break;
                        return novoInd;
                    }
                }
            }

            nTentativas++;

            if (nTentativas < 10) {
                goto compact;
            }


        } else {
            nTentativas++;

            if (nTentativas < 10) {
                goto compact;
            }
        }


        /*printf("Trocas: %d/%d\n", nTrocas, disc->nAulas);
        imprimeIndividuo2(p, novoInd);
        scanf("%d\n", &p2);*/


        // faz a troca das posicoes
        /*troca_par(novoInd, p1, p2);

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando move\n");
            goto rooms;
        }*/

    }


    return novoInd;
}

Individuo * lectureMove(Problema *p, Individuo * ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

lecture_move:

    p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
    p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, novoInd->aula[p1])) {
        p1++;
        if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p1 = 0;
        }
    }

    while (ehAula(p, novoInd->aula[p2])) {
        p2++;
        if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p2 = 0;
        }
    }

    // faz a troca das posicoes
    aux = novoInd->aula[p1];
    novoInd->aula[p1] = novoInd->aula[p2];
    novoInd->aula[p2] = aux;

    /*if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
        troca_par(novoInd, p1, p2);
        //printf("voltando move\n");
        goto lecture_move;
    }*/

    return novoInd;
}

Individuo * timeMove(Problema *p, Individuo * ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

timemove:

    p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, novoInd->aula[p1])) {
        p1++;
        if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p1 = 0;
        }
    }

    //int timeslot = getTimeSlotFromPos(p1, p);

    int sala = getSalaFromPos(p, p1);
    //int timeSlotVazio = getTimeSlotVazioParaSala(p, novoInd, sala, timeslot);

    p2 = getPosVaziaParaSala(p, novoInd, sala, p1);

    //imprimeIndividuo2(p,novoInd);
    //printf("Sala: %d\n", sala);
    //printf("TimeSlot: %d\n", timeslot);
    //printf("TVazio: %d\n", timeSlotVazio);
    //scanf("%d\n",&p2);

    if (p2 == -1) {
        goto timemove;
    }

    //p2 = sala * (p->nDias * p->nPerDias) + timeSlotVazio;



    /*if (timeslot == p->nDias * p->nPerDias) {
        p2 = p1 - timeslot;
    } else {
        p2 = p1 + 1;
    }
    int tentativas = 0;
    while (ehAula(p, novoInd->aula[p2])) {
        p2++;
        tentativas++;
        if (getTimeSlotFromPos(p2, p) == 0) {// volta ao inicio do vetor 'aula'
            p2 = p2 - (p->nDias * p->nPerDias);
        }

        if (tentativas > p->nDias * p->nPerDias) {
            goto timemove;
        }
    }*/

    // faz a troca das posicoes
    troca_par(novoInd, p1, p2);

    /*if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
        troca_par(novoInd, p1, p2);
        //printf("voltando move\n");
        goto timemove;
    }*/

    return novoInd;
}

Individuo * roomMove(Problema *p, Individuo * ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

roommove:

    p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, novoInd->aula[p1])) {
        p1++;
        if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p1 = 0;
        }
    }

    int timeslot = getTimeSlotFromPos(p1, p);

    int sala = getSalaFromPos(p, p1);
    int salaVazia = getSalaVazioParaTimeSlot(p, novoInd, sala, timeslot);

    /*imprimeIndividuo2(p, novoInd);
    printf("Sala: %d\n", sala);
    printf("TimeSlot: %d\n", timeslot);
    printf("SalaVazia: %d\n", salaVazia);
    if (salaVazia == -1) {
        scanf("%d\n", &p2);
    }*/

    if (salaVazia == -1) {
        goto roommove;
    }

    p2 = salaVazia * (p->nDias * p->nPerDias) + timeslot;



    /*if (timeslot == p->nDias * p->nPerDias) {
        p2 = p1 - timeslot;
    } else {
        p2 = p1 + 1;
    }
    int tentativas = 0;
    while (ehAula(p, novoInd->aula[p2])) {
        p2++;
        tentativas++;
        if (getTimeSlotFromPos(p2, p) == 0) {// volta ao inicio do vetor 'aula'
            p2 = p2 - (p->nDias * p->nPerDias);
        }

        if (tentativas > p->nDias * p->nPerDias) {
            goto timemove;
        }
    }*/

    // faz a troca das posicoes
    troca_par(novoInd, p1, p2);

    /*if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
        troca_par(novoInd, p1, p2);
        //printf("voltando move\n");
        goto roommove;
    }*/

    return novoInd;
}

Individuo * rooms(Problema *p, Individuo * ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

rooms:
    p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, novoInd->aula[p1])) {
        p1++;
        if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p1 = 0;
        }
    }

    Disciplina *disc = acessaDisciplina(p, novoInd->aula[p1]);
    int salaAdequada = getSalaAdequada(p, disc);

    /*imprimeIndividuo2(p, novoInd);
    printf("Disc: %s, [%d, %d]\n", disc->nomeDisciplina, disc->aulaInicial, disc->aulaInicial + disc->nAulas - 1);
    //printf("TimeSlot: %d\n", timeslot);
    printf("SalaAdequada: %d\n", salaAdequada);
    //if (salaVazia == -1) {
    scanf("%d\n", &p2);
    //}*/

    if (salaAdequada == -1) {
        goto rooms;
    }

    int nTrocas = 0;
    for (i = 0; i < disc->nSlotsDisponiveis; i++) {
        int pos = disc->slotsDisponiveis[i];

        if (ehAula(p, novoInd->aula[pos])) {
            if (novoInd->aula[pos] >= disc->aulaInicial &&
                    novoInd->aula[pos] < disc->aulaInicial + disc->nAulas) {

                int timeslot = getTimeSlotFromPos(pos, p);

                p2 = salaAdequada * (p->nDias * p->nPerDias) + timeslot;

                //printf("%d <-> %d Conf=%d\n", novoInd->aula[pos], novoInd->aula[p2], aulasConflitantes(p, novoInd->aula[pos], novoInd->aula[p2]));



                //printf("Trocando... => [%d,%d] (%d,%d) S=(%d,%d)\n", getTimeSlotFromPos(pos, p), getTimeSlotFromPos(p2, p), pos, p2, getSalaFromPos(p, pos), getSalaFromPos(p, p2));

                troca_par(novoInd, pos, p2);

                /*if (somaViolacoesHardTroca(p, novoInd, pos, p2) > 0) {
                    printf("Oh my...\n");
                    printf("%f\n", funcaoObjetivo(p, novoInd));
                    exit(1);
                }*/
                nTrocas++;
            }
        }

    }

    /*printf("Trocas: %d/%d\n", nTrocas, disc->nAulas);
    imprimeIndividuo2(p, novoInd);
    scanf("%d\n", &p2);*/


    // faz a troca das posicoes
    /*troca_par(novoInd, p1, p2);

    if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
        troca_par(novoInd, p1, p2);
        //printf("voltando move\n");
        goto rooms;
    }*/

    return novoInd;
}

Individuo * compact(Problema *p, Individuo * ind) {
    int i, p1, p2, aux;
    int nTentativas = 0;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

compact:
    p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, novoInd->aula[p1])) {
        p1++;
        if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p1 = 0;
        }
    }

    Disciplina *disc = acessaDisciplina(p, novoInd->aula[p1]);
    Curriculo *curr = disc->curriculos[rand() % disc->nCurriculos];

    int dia = getDia(p, p1);
    int horario = getPeriodo(p, p1);

    somaViolacoesSoft(p, novoInd);
    int anterior = novoInd->soft3;


    if (aulaIsolada(p, novoInd, p1, dia, horario)) {

        for (i = 0; i < disc->nSlotsDisponiveis; i++) {
            if (!ehAula(p, novoInd->aula[i])) {
                troca_par(novoInd, i, p1);

                if (somaViolacoesHardTroca(p, novoInd, p1, i) > 0) {
                    troca_par(novoInd, i, p1);
                    //printf("voltando move\n");
                    continue;
                }

                dia = getDia(p, i);
                horario = getPeriodo(p, i);

                if (aulaIsolada(p, novoInd, i, dia, horario)) {
                    // desfaz pq na tirou violação
                    troca_par(novoInd, i, p1);
                } else {
                    somaViolacoesSoft(p, novoInd);
                    //printf("diminui compact %d %d\n", anterior, novoInd->soft3);
                    //break;
                    return novoInd;
                }
            }
        }

        nTentativas++;

        if (nTentativas < 10) {
            goto compact;
        }


    } else {
        nTentativas++;

        if (nTentativas < 10) {
            goto compact;
        }
    }


    /*printf("Trocas: %d/%d\n", nTrocas, disc->nAulas);
    imprimeIndividuo2(p, novoInd);
    scanf("%d\n", &p2);*/


    // faz a troca das posicoes
    /*troca_par(novoInd, p1, p2);

    if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
        troca_par(novoInd, p1, p2);
        //printf("voltando move\n");
        goto rooms;
    }*/



    return novoInd;
}

Individuo * move(Problema *p, Individuo * ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

    /*** MOVE EVENT ***/
move:

    p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
    p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, novoInd->aula[p1])) {
        p1++;
        if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p1 = 0;
        }
    }

    while (ehAula(p, novoInd->aula[p2])) {
        p2++;
        if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p2 = 0;
        }
    }

    // faz a troca das posicoes
    aux = novoInd->aula[p1];
    novoInd->aula[p1] = novoInd->aula[p2];
    novoInd->aula[p2] = aux;

    /*if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
        troca_par(novoInd, p1, p2);
        //printf("voltando move\n");
        goto move;
    }*/

    return novoInd;
}

Individuo * swap(Problema *p, Individuo * ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

swap:

    p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
    p2 = rand() % p->dimensao; // posicao que irá apontar outro horario de aula

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, novoInd->aula[p1])) {
        p1++;
        if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p1 = 0;
        }
    }

    while (!ehAula(p, novoInd->aula[p2])) {
        p2++;
        if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
            p2 = 0;
        }
    }

    // faz a troca das posicoes
    aux = novoInd->aula[p1];
    novoInd->aula[p1] = novoInd->aula[p2];
    novoInd->aula[p2] = aux;

    /*if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
        troca_par(novoInd, p1, p2);
        //printf("voltando swap\n");
        goto swap;
    }*/

    return novoInd;
}

void embaralhaMovimentos(Movimento *movs, int n) {
    int i;

    for (i = 0; i < 100; i++) {
        int p1 = rand() % n;
        int p2 = rand() % n;

        Movimento aux = movs[p1];
        movs[p1] = movs[p2];
        movs[p2] = aux;
    }

}

void configuraQtMovsVNS(Problema*p, Individuo *indBase, Movimento *movs, int *iteracoes, int qtMovs) {

    int i;
    int total;

    printf("I,%d,%d,%d,%d\n", indBase->soft1, indBase->soft2,
            indBase->soft3, indBase->soft4);

    total = 0;

    for (i = 0; i < qtMovs; i++) {
        iteracoes[i] = 0;
        switch (movs[i]) {
            case MOVE:
                iteracoes[i] += (indBase->soft1 + indBase->soft2 + indBase->soft3 + indBase->soft4);
                //printf("MOVE: %d\n", iteracoes[i]);
                break;
            case SWAP:
                iteracoes[i] += (indBase->soft1 + indBase->soft2 + indBase->soft3 + indBase->soft4);
                //printf("SWAP: %d\n", iteracoes[i]);
                break;
            case TIME_MOVE:
                iteracoes[i] += (indBase->soft2 + indBase->soft3);
                //printf("TIME_MOVE: %d\n", iteracoes[i]);
                break;
            case ROOM_MOVE:
                iteracoes[i] += (indBase->soft1 + indBase->soft4);
                //printf("ROOM_MOVE: %d\n", iteracoes[i]);
                break;
            case MIN_WORKING_DAYS_MOVE:
                iteracoes[i] += (indBase->soft2);
                //printf("ROOMS: %d\n", iteracoes[i]);
                break;
            case COMPACT:
                iteracoes[i] += indBase->soft3;
                //printf("COMPACT: %d\n", iteracoes[i]);
                break;
            default:
                break;
        }

        total += iteracoes[i];
    }

    float fator = (float) p->nIterSemMelhoras / total;

    //printf("Iter: %d, Max: %d, Fator: %f\n", total, p->nIterSemMelhoras, fator);

    for (i = 0; i < qtMovs; i++) {
        iteracoes[i] = ceil(fator * iteracoes[i]);

    }



}

Individuo * buscaLocalGraspVNS(Problema*p, Individuo * indInicial) {
    Movimento mov;
    Individuo *solucaoAtual;
    float foAtual, fo;
    float deltaF;
    int i, iteracoes = 0;
    int haMelhoras;
    int pMov;
    Movimento movimentos[6] = {TIME_MOVE, ROOM_MOVE, MIN_WORKING_DAYS_MOVE, COMPACT, MOVE, SWAP};
    int iteracoesMax[6] = {0, 0, 0, 0, 0, 0};
    int nMovs = 6;
    Neighbour *neighbour;

    foAtual = funcaoObjetivo(p, indInicial, p->pesoHard);
    solucaoAtual = indInicial;

    embaralhaMovimentos(movimentos, nMovs);

    pMov = 0;

    configuraQtMovsVNS(p, solucaoAtual, movimentos, iteracoesMax, nMovs);

    do {

        haMelhoras = 0;
        for (pMov = 0; pMov < nMovs; pMov++) {
            //printf("%d\n", iteracoesMax[pMov]);
            for (i = 0; i < iteracoesMax[pMov]; i++) {

                switch (movimentos[pMov]) {
                    case MOVE:
                        neighbour = geraMove(p, solucaoAtual);
                        break;
                    case SWAP:
                        neighbour = geraSwap(p, solucaoAtual);
                        break;
                    case TIME_MOVE:
                        neighbour = geraTimeMove(p, solucaoAtual);
                        break;
                    case ROOM_MOVE:
                        neighbour = geraRoomMove(p, solucaoAtual);
                        break;
                    case MIN_WORKING_DAYS_MOVE:
                        neighbour = geraMinWorkingDaysMove(p, solucaoAtual);
                        break;
                    case COMPACT:
                        neighbour = geraIsolatedLectureMove(p, solucaoAtual);
                        break;
                    default:
                        break;

                }

                deltaF = neighbour->deltaHard * p->pesoHard + neighbour->deltaSoft;

                if (deltaF <= 0) {// função objetivo decresceu
                    foAtual += deltaF;

                    troca_par_completo(p, solucaoAtual, neighbour->p1, neighbour->p2);
                    //melhorInd = solucaoAtual;
                    if (deltaF < 0) {
                        printf("VNS: %f\n", foAtual);
                        haMelhoras = 1;
                        iteracoes = 0; // continua buscando
                    }
                }

                //printf("ADesalocar: %p %p %p\n", aDesalocar, solucaoAtual, vizinho);
                free(neighbour);
            }
        }

        //p->pesoHard += 1;
        foAtual = funcaoObjetivo(p, solucaoAtual, p->pesoHard);

        configuraQtMovsVNS(p, solucaoAtual, movimentos, iteracoesMax, nMovs);
        printf("==> %.2f/%.2f\n", somaViolacoesHard(p, solucaoAtual), somaViolacoesSoft2(p, solucaoAtual));


        /*if (iteracoes % 2000 == 0) {// troca o tipo de movimento
            if (pMov == nMovs - 1) {
                pMov = 0; // volta ao inicio
            } else {
                pMov++; // proximo movimento
            }
            printf("%d %f\n", iteracoes, foAtual);
            printf("trocou para %d\n", movimentos[pMov]);
            printf("I,%d,%d,%d,%d\n", solucaoAtual->soft1, solucaoAtual->soft2,
                    solucaoAtual->soft3, solucaoAtual->soft4);
        }*/
    } while (haMelhoras);

    return solucaoAtual;
}

int temAdjacencia(Problema *p, Individuo *ind, int pos, int aula) {
    int i;
    int periodo = getPeriodoFromPos(pos, p);
    int timeslot = getTimeSlotFromPos(pos, p);
    int temAdjacente = 0;
    int qtHorarios = p->nDias * p->nPerDias;

    if (periodo > 0) {// caso nao seja o primeiro periodo...
        // ... verifica se tem aula do mesmo curriculo no periodo anterior

        for (i = 0; i < p->nSalas; i++) {
            int pos = timeslot - 1 + qtHorarios * i;


            if (ehAula(p, ind->aula[pos])) {
                if (aulasMesmoCurriculo2(p, aula, ind->aula[pos])) {
                    temAdjacente = 1;
                    break;
                }
            }
        }
    }

    if (!temAdjacente && periodo < p->nPerDias - 1) {// caso nao tenha aula antes, 
        // e nao e o ultimo periodo do dia
        // ... verifica se tem aula do mesmo curriculo no periodo posterior

        for (i = 0; i < p->nSalas; i++) {
            int pos = timeslot + 1 + qtHorarios * i;

            if (ehAula(p, ind->aula[pos])) {
                if (aulasMesmoCurriculo(p, aula, ind->aula[pos])) {
                    temAdjacente = 1;
                    break;
                }
            }
        }
    }

    return temAdjacente;

}

float checaAulasIsoladas(Problema *p, Individuo *ind, int*curriculos, int nCurriculos,
        int* dias, int nDias) {

    int c, dia, periodo;
    int nAulas;
    float soma = 0;
    int i, j;

    for (i = 0; i < nCurriculos; i++) {
        for (j = 0; j < nDias; j++) {
            c = curriculos[i];
            dia = dias[j];
            for (periodo = 0; periodo < p->nPerDias; periodo++) {

                nAulas = ind->currDiasPeriodos[c][dia][periodo];
                // se o curriculo tem aula no dia/horario
                if (ind->currDiasPeriodos[c][dia][periodo]) {
                    if (periodo == 0) {
                        if (!ind->currDiasPeriodos[c][dia][periodo + 1]) {
                            soma += nAulas;
                        }
                    } else if (periodo == p->nPerDias - 1) {
                        if (!ind->currDiasPeriodos[c][dia][periodo - 1]) {
                            soma += nAulas;
                        }
                    } else {
                        if (!ind->currDiasPeriodos[c][dia][periodo - 1] && !ind->currDiasPeriodos[c][dia][periodo + 1]) {
                            soma += nAulas;
                        }
                    }
                }
            }
        }
    }

    return soma; // peso 2


}

int insereTimeslot(int *timeslots, int nTimeslots, int novoTimeslot) {
    int i;
    for (i = 0; i < nTimeslots; i++) {
        if (timeslots[i] == novoTimeslot) {
            return nTimeslots;
        }
    }

    // ainda nao estava
    timeslots[nTimeslots] = novoTimeslot;
    return nTimeslots + 1;

}

void avaliaNeighbour(Problema *p, Individuo *ind, Neighbour *move) {

    int p1, p2, i;
    int aula1, aula2;
    float hard1, hard2;
    float soft1, soft2;
    float RC1, MW1, IL1, RS1;
    float RC2, MW2, IL2, RS2;
    int totalDiasOcupados1;
    int totalSalasOcupadas1;
    int totalDiasOcupados2;
    int totalSalasOcupadas2;
    int diasAVerificar[2] = {0, 0};
    int nDias;
    int timeslot1, timeslot2;
    int *curriculosAVerificar;
    int totalMaxCurriculos, nCurriculos;
    int dia1, dia2;
    int sala1, sala2;
    Sala *s1, *s2;
    Disciplina *disc1, *disc2;

    move->deltaHard = move->deltaSoft = 0;

    p1 = move->p1;
    p2 = move->p2;

    // violacoes antes da troca
    hard1 = somaViolacoesHardTroca(p, ind, p1, p2);

    // troca
    troca_par(ind, p1, p2);

    // violacoes depois da troca
    hard2 = somaViolacoesHardTroca(p, ind, p1, p2);

    // volta individuo original
    troca_par(ind, p1, p2);

    // calcula deltaHard
    move->deltaHard = hard2 - hard1;

    if (move->deltaHard > 0) {
        return;
    }


    ////////////// SOFT //////////////////

    soft1 = soft2 = 0;
    RC1 = MW1 = IL1 = RS1 = RC2 = MW2 = IL2 = RS2 = 0;

    s1 = p->salas + getSalaFromPos(p, p1);
    s2 = p->salas + getSalaFromPos(p, p2);

    disc1 = acessaDisciplina(p, ind->aula[p1]);
    //printf("Disc1: %s\n", disc1->nomeDisciplina);
    if (ehAula(p, ind->aula[p2])) {
        disc2 = acessaDisciplina(p, ind->aula[p2]);
        //printf("Disc2: %s\n", disc2->nomeDisciplina);
    } else {
        disc2 = NULL;

    }

    aula1 = ind->aula[p1];
    aula2 = ind->aula[p2];

    //printf("Soft1: %.2f -> ", soft1);


    /**
     * Capacidade de sala
     */

    if (disc1->nAlunos > s1->capacidade) {
        RC1 += disc1->nAlunos - s1->capacidade;
    }
    if (disc2 && disc2->nAlunos > s2->capacidade) {
        RC1 += disc2->nAlunos - s2->capacidade;
    }

    // conta o total de dias ocupados
    totalDiasOcupados1 = 0;
    totalDiasOcupados2 = 0;
    for (i = 0; i < p->nDias; i++) {
        if (ind->diasOcupados[disc1->pVetor][i]) {
            totalDiasOcupados1++;
        }
        if (disc2 && ind->diasOcupados[disc2->pVetor][i]) {
            totalDiasOcupados2++;
        }
    }

    // conta o total de salas ocupadas
    totalSalasOcupadas1 = 0;
    totalSalasOcupadas2 = 0;
    for (i = 0; i < p->nSalas; i++) {
        if (ind->salasUsadas[disc1->pVetor][i]) {
            totalSalasOcupadas1++;
        }
        if (disc2 && ind->salasUsadas[disc2->pVetor][i]) {
            totalSalasOcupadas2++;
        }
    }
    /*printf("\n");
    printf("Salas ocupadas1: ");
    for (i = 0; i < p->nSalas; i++) {
        printf("%d ", salasOcupadas1[i]);

    }
    printf("\n");
    printf("Salas ocupadas2: ");
    for (i = 0; i < p->nSalas; i++) {
        printf("%d ", salasOcupadas2[i]);

    }
    printf("\n");
    printf("Salas ocupadas1: %d\n", totalSalasOcupadas1);
    printf("Salas ocupadas2: %d\n", totalSalasOcupadas2);*/

    /**
     * Estabilidade de sala
     */
    // ROOM_STABILITY
    RS1 += (totalSalasOcupadas1 - 1);
    // ROOM_STABILITY
    if (disc2) {
        RS1 += (totalSalasOcupadas2 - 1);
    }


    /**
     * MinWorkingDays
     */
    // MIN_WORKING_DAYS
    if (totalDiasOcupados1 < disc1->minDiasAula) {
        // penalizacao MIN_WORKING_DAYS
        MW1 += (disc1->minDiasAula - totalDiasOcupados1); // peso = 5
    }
    // MIN_WORKING_DAYS
    if (disc2 && totalDiasOcupados2 < disc2->minDiasAula) {
        // penalizacao MIN_WORKING_DAYS
        MW1 += (disc2->minDiasAula - totalDiasOcupados2); // peso = 5
    }


    /**
     * Aulas isoladas
     */

    // ISOLATED_LECTURE
    //alocacao->custo = 0;
    /*/if (!temAdjacencia(p, ind, p1, aula1)) {
        IL1 += 2; // penalidade peso 2
    }
    if (disc2 && !temAdjacencia(p, ind, p2, aula2)) {
        IL1 += 2;
    }*/



    /*/printf(" RC1: %.2f ", RC1);
    printf(" MW1: %.2f ", MW1);
    printf(" IL1: %.2f ", IL1);
    printf(" RS1: %.2f \n", RS1);*/


    /// calcular soft2
    /**
     * Capacidade de sala
     */

    //printf("Soft2: %.2f -> ", soft2);
    // Disc1 ==> s2
    // Disc2 ==> s1
    if (disc1->nAlunos > s2->capacidade) {
        RC2 += disc1->nAlunos - s2->capacidade;
    }
    if (disc2 && disc2->nAlunos > s1->capacidade) {
        RC2 += disc2->nAlunos - s1->capacidade;
    }
    //printf(" RC%.2f -> ", RC2);

    dia1 = getDiaFromPos(p1, p);
    dia2 = getDiaFromPos(p2, p);

    // vai do dia1 para o dia2
    ind->diasOcupados[disc1->pVetor][dia1]--;
    ind->diasOcupados[disc1->pVetor][dia2]++;

    if (disc2) {
        ind->diasOcupados[disc2->pVetor][dia2]--;
        ind->diasOcupados[disc2->pVetor][dia1]++;
    }

    // vai da sala1 para sala2
    sala1 = getSalaFromPos(p, p1);
    sala2 = getSalaFromPos(p, p2);

    ind->salasUsadas[disc1->pVetor][sala1]--;
    ind->salasUsadas[disc1->pVetor][sala2]++;

    if (disc2) {
        ind->salasUsadas[disc2->pVetor][sala2]--;
        ind->salasUsadas[disc2->pVetor][sala1]++;
    }

    // conta o total de dias ocupados
    totalDiasOcupados1 = 0;
    totalDiasOcupados2 = 0;
    for (i = 0; i < p->nDias; i++) {
        if (ind->diasOcupados[disc1->pVetor][i]) {
            totalDiasOcupados1++;
        }
        if (disc2 && ind->diasOcupados[disc2->pVetor][i]) {
            totalDiasOcupados2++;
        }
    }

    // conta o total de salas ocupadas
    totalSalasOcupadas1 = 0;
    totalSalasOcupadas2 = 0;
    for (i = 0; i < p->nSalas; i++) {
        if (ind->salasUsadas[disc1->pVetor][i]) {
            totalSalasOcupadas1++;
        }
        if (disc2 && ind->salasUsadas[disc2->pVetor][i]) {
            totalSalasOcupadas2++;
        }
    }
    /*printf("Salas ocupadas1: ");
    for (i = 0; i < p->nSalas; i++) {
        printf("%d ", salasOcupadas1[i]);

    }
    printf("\n");
    printf("Salas ocupadas2: ");
    for (i = 0; i < p->nSalas; i++) {
        printf("%d ", salasOcupadas2[i]);

    }
    printf("\n");
    printf("Salas ocupadas1: %d\n", totalSalasOcupadas1);
    printf("Salas ocupadas2: %d\n", totalSalasOcupadas2);*/

    /**
     * Estabilidade de sala
     */
    // ROOM_STABILITY
    RS2 += (totalSalasOcupadas1 - 1);
    // ROOM_STABILITY
    if (disc2) {
        RS2 += (totalSalasOcupadas2 - 1);
    }
    //printf(" RS%.2f -> ", RS2);

    /**
     * MinWorkingDays
     */
    // MIN_WORKING_DAYS
    if (totalDiasOcupados1 < disc1->minDiasAula) {
        // penalizacao MIN_WORKING_DAYS
        MW2 += (disc1->minDiasAula - totalDiasOcupados1); // peso = 5
    }
    // MIN_WORKING_DAYS
    if (disc2 && totalDiasOcupados2 < disc2->minDiasAula) {
        // penalizacao MIN_WORKING_DAYS
        MW2 += (disc2->minDiasAula - totalDiasOcupados2); // peso = 5
    }

    // restaurando as matrizes originais
    // vai do dia2 para o dia1
    ind->diasOcupados[disc1->pVetor][dia1]++;
    ind->diasOcupados[disc1->pVetor][dia2]--;

    if (disc2) {
        ind->diasOcupados[disc2->pVetor][dia2]++;
        ind->diasOcupados[disc2->pVetor][dia1]--;
    }

    // vai da sala2 para sala1
    ind->salasUsadas[disc1->pVetor][sala1]++;
    ind->salasUsadas[disc1->pVetor][sala2]--;

    if (disc2) {
        ind->salasUsadas[disc2->pVetor][sala2]++;
        ind->salasUsadas[disc2->pVetor][sala1]--;
    }

    //printf(" MW%.2f -> ", MW2);

    // ISOLATED_LECTURE
    //alocacao->custo = 0;
    /*if (!temAdjacencia(p, ind, p2, aula1)) {
        IL2 += 2; // penalidade peso 2
    }
    if (disc2 && !temAdjacencia(p, ind, p1, aula2)) {
        IL2 += 2;
    }*/

    dia1 = getDiaFromPos(p1, p);
    dia2 = getDiaFromPos(p2, p);

    //printf("A inserir: %d e %d\n", timeslot1, timeslot2);

    diasAVerificar[0] = dia1;
    if (dia1 != dia2) {
        diasAVerificar[1] = dia2;
        nDias = 2;
    } else {
        nDias = 1;
    }

    // lista dos curriculos que foram alterados
    totalMaxCurriculos = disc1->nCurriculos;
    if (disc2) {
        totalMaxCurriculos += disc2->nCurriculos;
    }

    curriculosAVerificar = (int *) malloc(totalMaxCurriculos * sizeof (int));
    nCurriculos = 0;

    for (i = 0; i < disc1->nCurriculos; i++) {
        nCurriculos = insereTimeslot(curriculosAVerificar, nCurriculos, disc1->curriculos[i]->pVetor);
    }

    if (disc2) {
        for (i = 0; i < disc2->nCurriculos; i++) {
            nCurriculos = insereTimeslot(curriculosAVerificar, nCurriculos, disc2->curriculos[i]->pVetor);
        }
    }


    //printf("NT=%d\n", nTimeslots);

    IL1 = checaAulasIsoladas(p, ind, curriculosAVerificar, nCurriculos, diasAVerificar, nDias);

    int periodo1 = getPeriodoFromPos(p1, p);
    int periodo2 = getPeriodoFromPos(p2, p);
    // modifica matriz para contagem
    for (i = 0; i < disc1->nCurriculos; i++) {
        ind->currDiasPeriodos[disc1->curriculos[i]->pVetor][dia1][periodo1]--;
        ind->currDiasPeriodos[disc1->curriculos[i]->pVetor][dia2][periodo2]++;
    }
    if (disc2) {
        for (i = 0; i < disc2->nCurriculos; i++) {
            ind->currDiasPeriodos[disc2->curriculos[i]->pVetor][dia1][periodo1]++;
            ind->currDiasPeriodos[disc2->curriculos[i]->pVetor][dia2][periodo2]--;
        }
    }

    IL2 = checaAulasIsoladas(p, ind, curriculosAVerificar, nCurriculos, diasAVerificar, nDias);

    // restaura matriz
    for (i = 0; i < disc1->nCurriculos; i++) {
        ind->currDiasPeriodos[disc1->curriculos[i]->pVetor][dia1][periodo1]++;
        ind->currDiasPeriodos[disc1->curriculos[i]->pVetor][dia2][periodo2]--;
    }
    if (disc2) {
        for (i = 0; i < disc2->nCurriculos; i++) {
            ind->currDiasPeriodos[disc2->curriculos[i]->pVetor][dia1][periodo1]--;
            ind->currDiasPeriodos[disc2->curriculos[i]->pVetor][dia2][periodo2]++;
        }
    }

    //printf("Vou verificar: ");
    /*for (i=0;i<nTimeslots;i++){
        printf("%d ", timeslotsAVerificar[i]);
    }
    printf("\n");*/

    //printf(" IL%.2f \n", IL2);

    /*printf(" RC2: %.2f ", RC2);
    printf(" MW2: %.2f ", MW2);
    printf(" IL2: %.2f ", IL2);
    printf(" RS2: %.2f \n", RS2);*/


    // calcula deltaSoft
    soft1 = RC1 * p->pesoRC + MW1 * p->pesoMW + IL1 * p->pesoIL + RS1 + p->pesoRS;
    soft2 =  RC2 * p->pesoRC + MW2 * p->pesoMW + IL2 * p->pesoIL + RS2 + p->pesoRS;
    
    move->deltaSoft = soft2 - soft1;


    free(curriculosAVerificar);

}

Neighbour *geraSwap(Problema *p, Individuo *ind) {
    Neighbour *swap;

    swap = (Neighbour*) malloc(sizeof (Neighbour));
    swap->m = SWAP;


    swap->p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
    swap->p2 = rand() % p->dimensao; // posicao que irá apontar outro horario de aula

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, ind->aula[swap->p1])) {
        swap->p1++;
        if (swap->p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            swap->p1 = 0;
        }
    }

    while (swap->p1 == swap->p2 || !ehAula(p, ind->aula[swap->p2]) ||
            aulasMesmaDisciplina3(p, ind->aula[swap->p1], ind->aula[swap->p2])) {
        swap->p2++;
        if (swap->p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
            swap->p2 = 0;
        }
    }

    avaliaNeighbour(p, ind, swap);

    return swap;

}

Neighbour *geraMove(Problema *p, Individuo *ind) {
    Neighbour *move;

    move = (Neighbour*) malloc(sizeof (Neighbour));
    move->m = MOVE;


    /*** MOVE EVENT ***/


    move->p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
    move->p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, ind->aula[move->p1])) {
        move->p1++;
        if (move->p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            move->p1 = 0;
        }
    }

    while (move->p1 == move->p2 || ehAula(p, ind->aula[move->p2])) {
        move->p2++;
        if (move->p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
            move->p2 = 0;
        }
    }

    avaliaNeighbour(p, ind, move);

    return move;


}

int getHorariosVagosSala(Problema*p, Individuo* ind, int *posVazias, int sala) {
    int nVazias;
    int i, pos;
    int qtHorarios;

    qtHorarios = p->nDias * p->nPerDias;
    nVazias = 0;

    for (i = 0; i < qtHorarios; i++) {
        pos = i + sala * qtHorarios;

        if (!ehAula(p, ind->aula[pos])) {
            posVazias[nVazias] = pos;
            nVazias++;
        }
    }

    return nVazias;
}

int getHorariosVagosTimeslot(Problema*p, Individuo* ind, int *posVazias, int timeslot) {
    int nVazias;
    int i, pos;
    int qtHorarios;

    qtHorarios = p->nDias * p->nPerDias;
    nVazias = 0;

    for (i = 0; i < p->nSalas; i++) {
        pos = timeslot + i * qtHorarios;

        if (!ehAula(p, ind->aula[pos])) {
            posVazias[nVazias] = pos;
            nVazias++;
        }
    }

    return nVazias;
}

Neighbour *geraTimeMove(Problema *p, Individuo *ind) {
    Neighbour *move;
    int *pVazias;
    int nVazias;
    int sala;


    move = (Neighbour*) malloc(sizeof (Neighbour));
    move->m = TIME_MOVE;

    pVazias = (int*) malloc((p->nDias * p->nPerDias) * sizeof (int));


    /*** TIME_MOVE EVENT ***/

    move->p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
    move->p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, ind->aula[move->p1])) {
        move->p1++;
        if (move->p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            move->p1 = 0;
        }
    }

    sala = getSalaFromPos(p, move->p1);

    nVazias = getHorariosVagosSala(p, ind, pVazias, sala);

    if (nVazias == 0) {
        return geraTimeMove(p, ind);
    }

    move->p2 = pVazias[rand() % nVazias];

    avaliaNeighbour(p, ind, move);

    /*printf("TIME_MOVE: (S=%d,T=%d), (S=%d,T=%d)\n", getSalaFromPos(p, move->p1),
            getTimeSlotFromPos(move->p1, p),
            getSalaFromPos(p, move->p2),
            getTimeSlotFromPos(move->p2, p));**/

    return move;
}

Neighbour *geraRoomMove(Problema *p, Individuo *ind) {

    Neighbour *move;
    int *pVazias;
    int nVazias;
    int timeslot;


    move = (Neighbour*) malloc(sizeof (Neighbour));
    move->m = ROOM_MOVE;

    pVazias = (int*) malloc((p->nSalas) * sizeof (int));


    /*** TIME_MOVE EVENT ***/

    move->p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
    move->p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

    //printf("posicoes sorteadas\n");

    while (!ehAula(p, ind->aula[move->p1])) {
        move->p1++;
        if (move->p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
            move->p1 = 0;
        }
    }

    timeslot = getTimeSlotFromPos(move->p1, p);

    nVazias = getHorariosVagosTimeslot(p, ind, pVazias, timeslot);

    if (nVazias == 0) {
        return geraRoomMove(p, ind);
    }

    move->p2 = pVazias[rand() % nVazias];

    avaliaNeighbour(p, ind, move);

    /*printf("ROOM_MOVE: (S=%d,T=%d), (S=%d,T=%d)\n", getSalaFromPos(p, move->p1),
            getTimeSlotFromPos(move->p1, p),
            getSalaFromPos(p, move->p2),
            getTimeSlotFromPos(move->p2, p));*/

    return move;
}

int escolheAulaEmDiaEspecifico(Problema *p, Individuo *ind, int *dias, int nDias, Disciplina *disc) {

    int sala, periodo;
    int i;
    int pos;
    int qtHorarios = p->nDias * p->nPerDias;
    int posicoes[disc->nAulas];
    int n;

    n = 0;
    //printf("Posicoes de aula: ");
    for (sala = 0; sala < p->nSalas; sala++) {
        for (periodo = 0; periodo < p->nPerDias; periodo++) {
            for (i = 0; i < nDias; i++) {
                pos = (dias[i] * p->nPerDias) + periodo + sala*qtHorarios;

                if (ehAula(p, ind->aula[pos])) {
                    if (acessaDisciplina(p, ind->aula[pos]) == disc) {
                        posicoes[n] = pos;
                        //printf("%d ", pos);
                        n++;
                    }

                }
            }
        }
    }
    //printf("\n");

    return posicoes[rand() % n];
}

int escolheHorarioVagoEmDiaEspecifico(Problema *p, Individuo *ind, int *diasSemAula, int nDiasSemAula) {

    int sala, periodo;
    int i;
    int pos;
    int qtHorarios = p->nDias * p->nPerDias;
    int posicoes[p->dimensao - p->nAulas];
    int n;

    n = 0;
    //printf("Posicoes vagas: ");
    for (sala = 0; sala < p->nSalas; sala++) {
        for (periodo = 0; periodo < p->nPerDias; periodo++) {
            for (i = 0; i < nDiasSemAula; i++) {
                pos = (diasSemAula[i] * p->nPerDias) + periodo + sala*qtHorarios;

                if (!ehAula(p, ind->aula[pos])) {
                    posicoes[n] = pos;
                    //printf("%d ", pos);
                    n++;
                }
            }
        }
    }
    //printf("\n");

    return posicoes[rand() % n];
}

Neighbour *geraMinWorkingDaysMove(Problema *p, Individuo *ind) {
    Neighbour *move;
    int nDisc;
    int testados;
    int totalDiasUsados;
    int dia;
    int diasComExcesso[p->nDias];
    int diasSemAula[p->nDias];
    int nDiasComExcesso;
    int nDiasSemAula;
    Disciplina *disc;

    move = (Neighbour*) malloc(sizeof (Neighbour));
    move->m = MIN_WORKING_DAYS_MOVE;

    nDisc = rand() % p->nDisciplinas;
    testados = 0;

    // procura disciplina com penalidade MIN_WORKING_DAYS
    while (1) {
        totalDiasUsados = 0;

        for (dia = 0; dia < p->nDias; dia++) {
            if (ind->diasOcupados[nDisc][dia]) {
                totalDiasUsados++;
            }
        }

        if (totalDiasUsados < (p->disciplinas + nDisc)->minDiasAula) {
            break;
        }

        // passa para proxima disciplina
        nDisc++;

        if (nDisc == p->nDisciplinas) {
            nDisc = 0; // volta ao inicio
        }

        testados++;

        // nao ha como melhorar: gera movimento qualquer
        if (testados == p->nDisciplinas) {
            return geraMove(p, ind);
        }

    }

    //printf("Disciplina MW: %s\n", (p->disciplinas + nDisc)->nomeDisciplina);


    nDiasComExcesso = 0;
    for (dia = 0; dia < p->nDias; dia++) {
        if (ind->diasOcupados[nDisc][dia] > 1) {
            diasComExcesso[nDiasComExcesso] = dia;
            nDiasComExcesso++;
        }
    }

    nDiasSemAula = 0;
    for (dia = 0; dia < p->nDias; dia++) {
        if (ind->diasOcupados[nDisc][dia] == 0) {
            diasSemAula[nDiasSemAula] = dia;
            nDiasSemAula++;
        }
    }

    /*int i;
    printf("Dias em excesso: ");
    for (i = 0; i < nDiasComExcesso; i++) {
        printf("%d ", diasComExcesso[i]);
    }
    printf("\n");


    printf("Dias sem aula: ");
    for (i = 0; i < nDiasSemAula; i++) {
        printf("%d ", diasSemAula[i]);
    }
    printf("\n");*/

    /*** MOVE EVENT ***/



    move->p1 = escolheAulaEmDiaEspecifico(p, ind, diasComExcesso, nDiasComExcesso, (p->disciplinas + nDisc)); // posicao que ira apontar um horário de aula
    move->p2 = escolheHorarioVagoEmDiaEspecifico(p, ind, diasSemAula, nDiasSemAula); // posicao que irá apontar um horario vazio

    //printf("posicoes sorteadas\n");

    avaliaNeighbour(p, ind, move);

    return move;


}

int aulaDoCurriculoEmTimeslot(Problema *p, Individuo *ind, Curriculo *curr, int *timeSlots, int nTimeSlots) {

    int aula;
    int i, c;
    int posicoes[p->nSalas * nTimeSlots];
    int n;
    int sala;
    int pos;
    int qtHorarios;

    qtHorarios = p->nDias * p->nPerDias;

    // procura uma aula para comparacao
    for (i = 0; i < p->nDisciplinas; i++) {
        for (c = 0; c < (p->disciplinas + i)->nCurriculos; c++) {
            if ((p->disciplinas + i)->curriculos[c] == curr) {
                aula = (p->disciplinas + i)->aulaInicial;
            }
        }
    }
    //printf("Escolheu aula: %d\n",aula);
    n = 0;

    //printf("NTimeslots: %d\n",nTimeSlots);
    for (sala = 0; sala < p->nSalas; sala++) {
        for (i = 0; i < nTimeSlots; i++) {
            pos = timeSlots[i] + sala*qtHorarios;
            //printf("%d ",pos);

            if (ehAula(p, ind->aula[pos]) && aulasMesmoCurriculo2(p, aula, ind->aula[pos])) {
                posicoes[n] = pos;
                //printf("X ",pos);
                n++;
            }
        }
    }
    //printf("\n");

    return posicoes[rand() % n];
}

void checaCurriculosIsolados(Problema *p, Individuo *ind) {

    int c, dia, periodo;
    int nAulas;

    for (c = 0; c < p->nCurriculos; c++) {
        ind->curriculosComIL[c] = 0;
        for (dia = 0; dia < p->nDias && !ind->curriculosComIL[c]; dia++) {
            for (periodo = 0; periodo < p->nPerDias && !ind->curriculosComIL[c]; periodo++) {

                // se o curriculo tem aula no dia/horario
                if (ind->currDiasPeriodos[c][dia][periodo]) {
                    if (periodo == 0) {
                        if (!ind->currDiasPeriodos[c][dia][periodo + 1]) {
                            ind->curriculosComIL[c] = 1;
                            break;
                        }
                    } else if (periodo == p->nPerDias - 1) {
                        if (!ind->currDiasPeriodos[c][dia][periodo - 1]) {
                            ind->curriculosComIL[c] = 1;
                            break;
                        }
                    } else {
                        if (!ind->currDiasPeriodos[c][dia][periodo - 1] && !ind->currDiasPeriodos[c][dia][periodo + 1]) {
                            ind->curriculosComIL[c] = 1;
                            break;
                        }
                    }
                }
            }
        }
    }

}

Neighbour *geraIsolatedLectureMove(Problema *p, Individuo *ind) {

    Neighbour *move;
    int nCurr;
    int sala, dia, periodo, pos;
    int currComIL[p->nCurriculos];
    int timeSlotComIL[p->nDias * p->nPerDias];
    int timeSlotPromissor[p->nDias * p->nPerDias];
    int nCurrComIL;
    int nTimeSlotComIL;
    int nTimeSlotPromissor;
    int i;
    int melhorP2;
    float melhorDelta = 999999;

    move = (Neighbour*) malloc(sizeof (Neighbour));
    move->m = COMPACT;


    checaCurriculosIsolados(p, ind);
    // verifica quais curriculos tem penalização IL
    nCurrComIL = 0;
    // procura disciplina com penalidade MIN_WORKING_DAYS
    //printf("Curriculos X Flag: ");
    for (i = 0; i < p->nCurriculos; i++) {
        //printf("%d ",ind->curriculosComIL[i]);
        if (ind->curriculosComIL[i]) {
            currComIL[nCurrComIL] = i;
            nCurrComIL++;
        }
    }
    //printf("\n");



    // nao tem penalização IL: gera outro movimento
    if (nCurrComIL == 0) {
        return geraRoomMove(p, ind);
    }

    // escolhe um aleatoriamente
    nCurr = currComIL[rand() % nCurrComIL];

    //printf("Curriculo IL: %s\n", (p->curriculos + nCurr)->nomeCurriculo);
    //imprimeMatCurrDiasPeriodo(p, ind);


    nTimeSlotComIL = 0;
    nTimeSlotPromissor = 0;
    for (dia = 0; dia < p->nDias; dia++) {
        for (periodo = 0; periodo < p->nPerDias; periodo++) {

            if (ind->currDiasPeriodos[nCurr][dia][periodo]) {
                if (periodo == 0) {
                    if (!ind->currDiasPeriodos[nCurr][dia][periodo + 1]) {
                        timeSlotComIL[nTimeSlotComIL] = (dia * p->nPerDias) + periodo;
                        nTimeSlotComIL++;
                    }
                } else if (periodo == p->nPerDias - 1) {
                    if (!ind->currDiasPeriodos[nCurr][dia][periodo - 1]) {
                        timeSlotComIL[nTimeSlotComIL] = (dia * p->nPerDias) + periodo;
                        nTimeSlotComIL++;
                    }
                } else {
                    if (!ind->currDiasPeriodos[nCurr][dia][periodo - 1] && !ind->currDiasPeriodos[nCurr][dia][periodo + 1]) {
                        timeSlotComIL[nTimeSlotComIL] = (dia * p->nPerDias) + periodo;
                        nTimeSlotComIL++;
                    }
                }
            }

            if (!ind->currDiasPeriodos[nCurr][dia][periodo]) {
                if (periodo == 0) {
                    if (ind->currDiasPeriodos[nCurr][dia][periodo + 1]) {
                        timeSlotPromissor[nTimeSlotPromissor] = (dia * p->nPerDias) + periodo;
                        nTimeSlotPromissor++;
                    }
                } else if (periodo == p->nPerDias - 1) {
                    if (ind->currDiasPeriodos[nCurr][dia][periodo - 1]) {
                        timeSlotPromissor[nTimeSlotPromissor] = (dia * p->nPerDias) + periodo;
                        nTimeSlotPromissor++;
                    }
                } else {
                    if (ind->currDiasPeriodos[nCurr][dia][periodo - 1] || ind->currDiasPeriodos[nCurr][dia][periodo + 1]) {
                        timeSlotPromissor[nTimeSlotPromissor] = (dia * p->nPerDias) + periodo;
                        nTimeSlotPromissor++;
                    }
                }
            }
        }
    }

    /*printf("Timeslots com IL: ");
    for (i = 0; i < nTimeSlotComIL; i++) {
        printf("%d ", timeSlotComIL[i]);
    }
    printf("\n");**/


    /*/printf("Timeslots promissor: ");
    for (i = 0; i < nTimeSlotPromissor; i++) {
        printf("%d ", timeSlotPromissor[i]);
    }
    printf("\n");*/

    move->p1 = aulaDoCurriculoEmTimeslot(p, ind, p->curriculos + nCurr, timeSlotComIL, nTimeSlotComIL);

    //printf("IL escolhido: %d\n", move->p1);
    //fflush(stdout);

    for (sala = 0; sala < p->nSalas; sala++) {
        for (i = 0; i < nTimeSlotPromissor; i++) {
            pos = timeSlotPromissor[i] + sala * (p->nDias * p->nPerDias);

            move->p2 = pos;

            avaliaNeighbour(p, ind, move);

            //printf("%d -> %.0f\n", pos, move->deltaHard + move->deltaSoft);

            if (move->deltaHard + move->deltaSoft < melhorDelta) {
                melhorP2 = pos;
                melhorDelta = move->deltaHard + move->deltaSoft;
            }
        }
    }

    move->p2 = melhorP2;

    /*int i;
    printf("Dias em excesso: ");
    for (i = 0; i < nDiasComExcesso; i++) {
        printf("%d ", diasComExcesso[i]);
    }
    printf("\n");


    printf("Dias sem aula: ");
    for (i = 0; i < nDiasSemAula; i++) {
        printf("%d ", diasSemAula[i]);
    }
    printf("\n");*/

    //printf("posicoes sorteadas\n");

    avaliaNeighbour(p, ind, move);

    return move;
}
