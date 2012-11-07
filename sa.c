#include <math.h>
#include "sa.h"
#include "fitness.h"
#include "util.h"

float funcaoObjetivo(Problema *p, Individuo *ind) {
    float vHard = somaViolacoesHard(p, ind);
    float vSoft = somaViolacoesSoft(p, ind);

    //printf("(%f, %f)\n", vHard, vSoft);

    return 10000 * vHard + vSoft;
}

Individuo *geraVizinho(Problema *p, Individuo *ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

    /*** MOVE EVENT ***/
    if (((float) rand()) / RAND_MAX) {
move:

        p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
        p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

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
            goto move;
        }

    } else {
        /*** SWAP EVENT ***/

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

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando swap\n");
            goto swap;
        }
    }


    return novoInd;
}

Individuo *geraVizinho2(Problema *p, Individuo *ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }


    /*** MOVE EVENT ***/
    if (((float) rand()) / RAND_MAX < 0.5) {
        p->nMoves++;
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

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
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

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando swap\n");
            goto swap;
        }
    }


    return novoInd;
}

Individuo *geraVizinho2Tabu(Problema *p, Individuo *ind, int *listaTabu) {
    int i, p1, p2, aux, pTabu;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }


    /*** MOVE EVENT ***/
    if (((float) rand()) / RAND_MAX < 0.5) {
        p->nMoves++;
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

        while (p1 == p2 || ehAula(p, novoInd->aula[p2])) {
            p2++;
            if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p2 = 0;
            }
        }

        pTabu = p1 * p->dimensao + p2;

        if (listaTabu[pTabu]) {
            goto move;
        }

        // faz a troca das posicoes
        aux = novoInd->aula[p1];
        novoInd->aula[p1] = novoInd->aula[p2];
        novoInd->aula[p2] = aux;

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            listaTabu[pTabu] = 1;
            //printf("voltando move\n");
            goto move;
        }

        listaTabu[pTabu] = 1;
    } else {
        p->nSwaps++;
        /*** SWAP EVENT ***/
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

        while (p1 == p2 || !ehAula(p, novoInd->aula[p2])) {
            p2++;
            if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p2 = 0;
            }
        }

        pTabu = p1 * p->dimensao + p2;

        if (listaTabu[pTabu]) {
            goto move;
        }

        // faz a troca das posicoes
        aux = novoInd->aula[p1];
        novoInd->aula[p1] = novoInd->aula[p2];
        novoInd->aula[p2] = aux;

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            listaTabu[pTabu] = 1;
            //printf("voltando swap\n");
            goto swap;
        }

        listaTabu[pTabu] = 1;
    }


    return novoInd;
}

int getPosCruz(Problema *p, int p1, Individuo *ind, int pLivre) {

    int t1 = getTimeSlotFromPos(p1, p);
    int r1 = getSalaFromPos(p, p1);
    int r, t, k;
    int pos[p->nSalas + (p->nDias * p->nPerDias)];

    k = 0;
    // caminho vertical
    for (r = 0; r < p->nSalas; r++) {
        if (r == r1) continue;

        int p2 = r * (p->nDias * p->nPerDias) + t1;

        if (pLivre && !ehAula(p, ind->aula[p2])) {
            pos[k] = p2;
            k++;
        } else if (!pLivre && ehAula(p, ind->aula[p2])) {
            pos[k] = p2;
            k++;
        }

    }

    // caminho horizontal
    for (t = 0; t < p->nSalas; t++) {
        if (t == t1) continue;
        int p2 = r1 * (p->nDias * p->nPerDias) + t;

        if (pLivre && !ehAula(p, ind->aula[p2])) {
            pos[k] = p2;
            k++;
        } else if (!pLivre && ehAula(p, ind->aula[p2])) {
            pos[k] = p2;
            k++;
        }

    }

    //printf("k=%d\n", k);
    if (k > 0) {

        return pos[rand() % k];
        int j;
        int melhorP2;
        float melhorFo = 99999999;
        float fo;
        for (j = 0; j < k; j++) {
            int p2 = pos[j];
            int aux = ind->aula[p1];
            ind->aula[p1] = ind->aula[p2];
            ind->aula[p2] = aux;

            fo = funcaoObjetivo(p, ind);

            if (fo < melhorFo) {
                melhorP2 = p2;
                melhorFo = fo;
            }

            aux = ind->aula[p1];
            ind->aula[p1] = ind->aula[p2];
            ind->aula[p2] = aux;

        }

        return melhorP2;
    } else {
        return -1;
    }

}

Individuo *geraVizinho4(Problema *p, Individuo *ind) {
    int i, p1, p2, aux;
    Individuo *novoInd = alocaIndividuo();
    criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    for (i = 0; i < novoInd->n; i++) {
        novoInd->aula[i] = ind->aula[i];
    }

    /*** MOVE EVENT ***/
    if (((float) rand()) / RAND_MAX < 0.5) {
        p->nMoves++;
move:

        p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
        p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio

        while (!ehAula(p, novoInd->aula[p1])) {
            p1++;
            if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p1 = 0;
            }
        }

        //printf("posicoes sorteadas\n");

        p2 = getPosCruz(p, p1, novoInd, 1);

        if (p2 == -1) {
            //printf("aconteceu ...\n");
            goto move;
        }

        /*while (ehAula(p, novoInd->aula[p2])) {
            p2++;
            if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p2 = 0;
            }
        }*/

        // faz a troca das posicoes
        aux = novoInd->aula[p1];
        novoInd->aula[p1] = novoInd->aula[p2];
        novoInd->aula[p2] = aux;

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando move\n");
            goto move;
        }
    } else {
        p->nSwaps++;
        /*** SWAP EVENT ***/
swap:

        p1 = rand() % p->dimensao; // posicao que ira apontar um horário de aula
        p2 = rand() % p->dimensao; // posicao que irá apontar outro horario de aula

        while (!ehAula(p, novoInd->aula[p1])) {
            p1++;
            if (p1 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p1 = 0;
            }
        }

        //printf("posicoes sorteadas\n");

        p2 = getPosCruz(p, p1, novoInd, 0);

        if (p2 == -1) {
            printf("aconteceu ...\n");
            goto swap;
        }


        /*while (!ehAula(p, novoInd->aula[p2])) {
            p2++;
            if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p2 = 0;
            }
        }*/

        // faz a troca das posicoes
        aux = novoInd->aula[p1];
        novoInd->aula[p1] = novoInd->aula[p2];
        novoInd->aula[p2] = aux;

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando swap\n");
            goto swap;
        }
    }


    return novoInd;
}

Individuo *simulatedAnnealing(Problema*p, Individuo *indInicial) {
    Individuo *solucaoAtual, *aDesalocar;
    Individuo *vizinho;
    float foAtual;
    float deltaF;
    float I = 114000000;
    long N;

    if (indInicial == NULL) {
        indInicial = geraIndividuoAleatorio(p, 5);
        //printf("gerando sol. inicial\n");
    }

    foAtual = funcaoObjetivo(p, indInicial);
    printf("HARD: %f\n", somaViolacoesHard(p, indInicial));
    printf("SOFT: %f\n", somaViolacoesSoft(p, indInicial));
    //exit(0);

    //printf("Implementando\n");
    //printf("%s\n", p->nome);

    //Gere uma solução inicial i
    //repita
    //repita
    //Gere uma solução j na vizinhança de i
    //se f(j) < f(i)
    //i=j;
    //fim se
    //se não
    //se random(0,1) < exp(
    // i = j;
    //fimm se
    ///m se
    //Até iterações = num_iteracoes
    //t = t;
    //Até <critério de parada>


    //Retorne i

    float t0, tMin, beta;

    t0 = p->t0;
    tMin = t0 / p->rho;
    beta = p->beta;

    N = -(I / (log10(t0 / tMin) / log10(beta)));

    //printf("N=%d\n", N);
    //printf("N=%f\n", N);

    N = 200;

    solucaoAtual = indInicial;
    do {
        int iteracoes = 0;
        float fo;
        int nPioras = 0;
        do {
            vizinho = geraVizinho2(p, solucaoAtual);

            fo = funcaoObjetivo(p, vizinho);
            deltaF = fo - foAtual;

            //printf("Df=%f\n", deltaF);

            aDesalocar = 0;
            if (deltaF < 0) {// função objetivo decresceu
                foAtual = fo;
                //printf("Melhorou... %f\n", foAtual);
                aDesalocar = solucaoAtual;
                solucaoAtual = vizinho;
                //melhorInd = solucaoAtual;
            } else {
                // calcula probabilidade de aceitação
                float prob = pow(M_E, -deltaF / t0);

                if (p->aceitaPioraSA && (float) rand() / RAND_MAX <= prob) {
                    //printf("aceitou piora\n");
                    aDesalocar = solucaoAtual;
                    solucaoAtual = vizinho;
                    //printf("aceitou piora\n");
                    foAtual = fo;
                    nPioras++;
                } else {
                    aDesalocar = vizinho;
                }

                //printf("prob=%f\n", prob);

            }

            //printf("ADesalocar: %p %p %p\n", aDesalocar, solucaoAtual, vizinho);
            liberaIndividuo(aDesalocar);

            iteracoes++;
        } while (iteracoes < N);

        printf("T=%f, Pioras=%d, FO=%f (%f, %f)\n", t0, nPioras, foAtual,
                somaViolacoesHard(p, solucaoAtual), somaViolacoesSoft(p, solucaoAtual));
        t0 *= beta;
    } while (t0 > tMin);

    return solucaoAtual;
}
