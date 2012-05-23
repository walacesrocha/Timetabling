#include <string.h>

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

ListaVizinhos *alocaListaVizinhos(int k) {
    ListaVizinhos *listaVizinhos;

    listaVizinhos = (ListaVizinhos*) malloc(sizeof (ListaVizinhos));

    listaVizinhos->pos1 = (int*) malloc(k * sizeof (int));
    listaVizinhos->pos2 = (int*) malloc(k * sizeof (int));

    return listaVizinhos;
}

void desalocaListaVizinhos(ListaVizinhos *listaVizinhos) {
    free(listaVizinhos->pos1);
    free(listaVizinhos->pos2);
    free(listaVizinhos);
}

void geraListaVizinhos(Problema *p, Individuo*ind, ListaVizinhos *listaVizinhos) {
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

int getSalaAdequada(Problema *p, Disciplina *disc) {
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

Individuo *geraVizinho3(Problema *p, Individuo *ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }
    
    float prob = ((float) rand()) / RAND_MAX;

    /*** MOVE EVENT ***/
    if (prob < 0.3) {
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

        //imprimeIndividuo2(p,novoInd);
        //printf("Sala: %d\n", sala);
        //printf("TimeSlot: %d\n", timeslot);
        //printf("TVazio: %d\n", timeSlotVazio);
        //scanf("%d\n",&p2);

        if (timeSlotVazio == -1) {
            goto timemove;
        }

        p2 = sala * (p->nDias * p->nPerDias) + timeSlotVazio;



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
    } else if (prob < 0.6) {
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
    } else if (prob < 0.9) {
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

                    printf("Trocando... => [%d,%d] (%d,%d)\n", getTimeSlotFromPos(pos,p),getTimeSlotFromPos(p2,p),pos,p2);

                    troca_par(novoInd, pos, p2);

                    if (somaViolacoesHardTroca(p, novoInd, pos, p2) > 0) {
                        printf("Oh my...\n");
                        printf("%f\n",funcaoObjetivo(p,novoInd));
                        exit(1);
                    }
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

                    dia = getDia(p, i);
                    horario = getPeriodo(p, i);

                    if (aulaIsolada(p, novoInd, i, dia, horario)) {
                        // desfaz pq na tirou violação
                        troca_par(novoInd, i, p1);
                    } else {
                        //somaViolacoesSoft(p, novoInd);
                        //printf("diminui compact %d %d\n", anterior, novoInd->soft3);
                        break;
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
