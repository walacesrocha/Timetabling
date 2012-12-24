#include <math.h>
#include "sa.h"
#include "fitness.h"
#include "util.h"
#include "tabu.h"
#include "buscalocal.h"
#include "gerador.h"

float funcaoObjetivo(Problema *p, Individuo *ind, float pesoHard) {
    float vHard = somaViolacoesHard(p, ind);
    float vSoft = somaViolacoesSoft2(p, ind);

    //printf("(%f, %f)\n", vHard, vSoft);

    return pesoHard * vHard + vSoft;
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
    novoInd = copiaIndividuo(p, ind); //criaIndividuo(novoInd, p);
    //printf("Individuo criado\n");
    //for (i = 0; i < novoInd->n; i++) {
    //  novoInd->aula[i] = ind->aula[i];
    //}


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
            p->trocaNaoEfetiva++;
        }

        while (p1 == p2 || ehAula(p, novoInd->aula[p2])) {
            p2++;
            if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p2 = 0;
            }
            p->trocaNaoEfetiva++;
        }

        // faz a troca das posicoes
        aux = novoInd->aula[p1];
        novoInd->aula[p1] = novoInd->aula[p2];
        novoInd->aula[p2] = aux;

        /*/if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            //printf("voltando move\n");
            goto move;
        }*/
        p->trocaEfetiva++;
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
            p->trocaNaoEfetiva++;
        }

        while (p1 == p2 || !ehAula(p, novoInd->aula[p2]) ||
                aulasMesmaDisciplina3(p, novoInd->aula[p1], novoInd->aula[p2])) {
            p2++;
            if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p2 = 0;
            }
            p->trocaNaoEfetiva++;
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
        p->trocaEfetiva++;
    }


    return novoInd;
}

Individuo *geraVizinho2Tabu(Problema *p, Individuo *ind, Tabu *listaTabu) {
    int i, p1, p2, aux;
    long code;
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


        // faz a troca das posicoes
        troca_par(novoInd, p1, p2);
        code = getHashCode(novoInd);
        if (estaNoTabu(code, listaTabu)) {
            //printf("voltando move (tabu)\n");
            goto move;
        }

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            insereTabu(code, listaTabu);
            //printf("voltando move\n");
            goto move;
        }

        insereTabu(code, listaTabu);
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


        // faz a troca das posicoes
        troca_par(novoInd, p1, p2);
        code = getHashCode(novoInd);
        if (estaNoTabu(code, listaTabu)) {
            //printf("voltando swap (tabu)\n");
            goto swap;
        }

        if (somaViolacoesHardTroca(p, novoInd, p1, p2) > 0) {
            troca_par(novoInd, p1, p2);
            insereTabu(code, listaTabu);
            //printf("voltando swap\n");
            goto swap;
        }

        insereTabu(code, listaTabu);
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

            fo = funcaoObjetivo(p, ind, 10000);

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

    float pesoHard = 1;
    int aumentaPesoHard = 0;

    foAtual = funcaoObjetivo(p, indInicial, pesoHard);
    //printf("HARD: %f\n", somaViolacoesHard(p, indInicial));
    //printf("SOFT: %f\n", somaViolacoesSoft(p, indInicial));
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
    tMin = p->tf;
    beta = p->beta;

    N = -(I / (log10(t0 / tMin) / log10(beta)));

    //printf("N=%d\n", N);
    //printf("N=%f\n", N);

    N = 200;

    solucaoAtual = indInicial;
    //Gerador *gerador = getGeradorInicial(p->dimensao);
annealing:
    t0 = p->t0;
    foAtual = funcaoObjetivo(p, solucaoAtual, pesoHard);

    do {
        int iteracoes = 0;
        float fo;
        int nPioras = 0;
        float totalProb = 0;
        do {
            vizinho = geraVizinho2(p, solucaoAtual);

            fo = funcaoObjetivo(p, vizinho, pesoHard);
            deltaF = fo - foAtual;

            //printf("Df=%f\n", deltaF);

            aDesalocar = 0;
            if (deltaF <= 0) {// função objetivo decresceu
                foAtual = fo;
                printf("SA: %f [%f]\n", foAtual, t0);
                aDesalocar = solucaoAtual;
                solucaoAtual = vizinho;
                //melhorInd = solucaoAtual;
            } else {
                // calcula probabilidade de aceitação
                float prob = pow(M_E, -deltaF / t0);

                //printf("%f <-> %f\n", prob, exp(-deltaF / t0));

                totalProb += prob;

                if (p->aceitaPioraSA && (((float) rand() / RAND_MAX) <= prob)) {
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

        //printf("T=%f/%f, Pioras=%d, FO=%f (%f, %f) [%.3f] Peso: %f\n", t0, tMin, nPioras, foAtual,
        //somaViolacoesHard(p, solucaoAtual), somaViolacoesSoft(p, solucaoAtual), totalProb / N, pesoHard);
        t0 *= beta;

        pesoHard += 0.01;
        foAtual = funcaoObjetivo(p, solucaoAtual, pesoHard);
        if (t0 < 0.01 * p->t0) {
            //pesoHard *= 1.01;
            foAtual = funcaoObjetivo(p, solucaoAtual, pesoHard);
        }
    } while (t0 > tMin);

    /*if (somaViolacoesHard(p, solucaoAtual) > 0) {
        aumentaPesoHard = 1;
        goto annealing;
    }*/

    return solucaoAtual;
}

Individuo *simulatedAnnealingVNS(Problema*p, Individuo *indInicial) {
    Individuo *solucaoAtual, *aDesalocar;
    Individuo *vizinho;
    float foAtual;
    float deltaF;
    long N;

    float pesoHard = 1;

    foAtual = funcaoObjetivo(p, indInicial, pesoHard);


    float t0, tMin, beta;

    t0 = p->t0;
    tMin = p->tf;
    beta = p->beta;

    N = 200;

    solucaoAtual = indInicial;
    //Gerador *gerador = getGeradorInicial(p->dimensao);
annealing:
    t0 = p->t0;
    foAtual = funcaoObjetivo(p, solucaoAtual, pesoHard);
    do {
        int iteracoes = 0;
        float fo;
        int nPioras = 0;
        float totalProb = 0;
        do {
            float pv = (float) rand() / RAND_MAX;

            if (pv < 0.15) {
                vizinho = move(p, solucaoAtual);
            } else if (pv < 0.3) {
                vizinho = swap(p, solucaoAtual);
            } else if (pv < 0.45) {
                vizinho = lectureMove(p, solucaoAtual);
            } else if (pv < 0.6) {
                vizinho = timeMove(p, solucaoAtual);
            } else if (pv < 0.75) {
                vizinho = roomMove(p, solucaoAtual);
            } else if (pv < 0.9) {
                vizinho = rooms(p, solucaoAtual);
            } else {
                vizinho = compact(p, solucaoAtual);
            }

            fo = funcaoObjetivo(p, vizinho, pesoHard);
            deltaF = fo - foAtual;

            //printf("Df=%f\n", deltaF);

            aDesalocar = 0;
            if (deltaF <= 0) {// função objetivo decresceu
                foAtual = fo;
                //printf("SA: %f [%f]\n", foAtual,t0);
                aDesalocar = solucaoAtual;
                solucaoAtual = vizinho;
                //melhorInd = solucaoAtual;
            } else {
                // calcula probabilidade de aceitação
                float prob = pow(M_E, -deltaF / t0);

                //printf("%f <-> %f\n", prob, exp(-deltaF / t0));

                totalProb += prob;

                if (p->aceitaPioraSA && (((float) rand() / RAND_MAX) <= prob)) {
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

        printf("T=%f/%f, Pioras=%d, FO=%f (%f, %f) [%.3f] Peso: %f\n", t0, tMin, nPioras, foAtual,
                somaViolacoesHard(p, solucaoAtual), somaViolacoesSoft(p, solucaoAtual), totalProb / N, pesoHard);
        t0 *= beta;

        pesoHard += 0.01;
        foAtual = funcaoObjetivo(p, solucaoAtual, pesoHard);
        if (t0 < 0.01 * p->t0) {
            //pesoHard *= 1.01;
            foAtual = funcaoObjetivo(p, solucaoAtual, pesoHard);
        }
    } while (t0 > tMin);

    /*if (somaViolacoesHard(p, solucaoAtual) > 0) {
        aumentaPesoHard = 1;
        goto annealing;
    }*/

    return solucaoAtual;
}

Individuo *simulatedAnnealing2(Problema*p, Individuo *indInicial) {
    Individuo *solucaoAtual;
    float foAtual;
    int nPioras;
    float deltaF;
    float prob;
    float totalProb;
    float melhorFo;
    long N;
    Neighbour *movimento;
    float pViz;
    long iteracoesSemMelhora = 0;

    float pesoHard = 10000;

    foAtual = funcaoObjetivo(p, indInicial, pesoHard);


    double t0, tMin, beta;

    t0 = p->t0;
    tMin = p->tf;
    beta = p->beta;


    solucaoAtual = indInicial;
    //Gerador *gerador = getGeradorInicial(p->dimensao);
    t0 = p->t0;
    foAtual = funcaoObjetivo(p, solucaoAtual, pesoHard);
    melhorFo = foAtual;
    do {
        N = 500;
        nPioras = 0;
        totalProb = 0;
        do {

            pViz = (float) rand() / RAND_MAX;

            if (pViz < 0.5) {
                movimento = geraSwap(p, solucaoAtual);
            } else if (pViz < 10.6) {
                movimento = geraMove(p, solucaoAtual);
            } else if (pViz < 0.8) {
                movimento = geraTimeMove(p, solucaoAtual);
            } else if (pViz < 1.72) {
                movimento = geraRoomMove(p, solucaoAtual);
            } else if (pViz < 0.9) {
                movimento = geraMinWorkingDaysMove(p, solucaoAtual);
            } else {
                movimento = geraIsolatedLectureMove(p, solucaoAtual);
            }

            deltaF = movimento->deltaHard * pesoHard + movimento->deltaSoft;

            //printf("Df=%f\n", deltaF);

            if (deltaF <= 0) {// função objetivo decresceu
                foAtual += deltaF;
                //printf("SA: %f [%f]\n", foAtual,t0);
                troca_par_completo(p, solucaoAtual, movimento->p1, movimento->p2);
                //melhorInd = solucaoAtual;
                iteracoesSemMelhora = 0;
                if (foAtual < melhorFo) {
                    melhorFo = foAtual;                    
                }
            } else {
                // calcula probabilidade de aceitação
                prob = pow(M_E, -deltaF / t0);

                //printf("%f <-> %f\n", prob, exp(-deltaF / t0));

                totalProb += prob;

                if (p->aceitaPioraSA && (((float) rand() / RAND_MAX) <= prob)) {
                    //printf("aceitou piora\n");
                    troca_par_completo(p, solucaoAtual, movimento->p1, movimento->p2);
                    //printf("aceitou piora\n");
                    foAtual += deltaF;
                    nPioras++;
                }

                //printf("prob=%f\n", prob);

            }

            //printf("ADesalocar: %p %p %p\n", aDesalocar, solucaoAtual, vizinho);
            free(movimento);

            N--;
            iteracoesSemMelhora++;
        } while (N > 0);

        /*printf("T=%.8f/%.8f, Pioras=%d, FO=%.1f / %.1f (%f, %f) Peso: %f RC: %d MW: %d IL: %d RS: %d F*=%.0f\n",
                t0, tMin, nPioras, foAtual,
                funcaoObjetivo(p, solucaoAtual, pesoHard),
                somaViolacoesHard(p, solucaoAtual), somaViolacoesSoft2(p, solucaoAtual), pesoHard,
                solucaoAtual->soft1,
                solucaoAtual->soft2,
                solucaoAtual->soft3,
                solucaoAtual->soft4,
                melhorFo);*/
        t0 *= beta;

        foAtual = funcaoObjetivo(p, solucaoAtual, pesoHard);

        /*if (iteracoes - ultimaMelhora > p->nIterSemMelhoras*5 && nReaquecimentos < 3) {
            // reaquece
            //t0 = t0 * pow(7, -1.7 * 7);
            t0 = p->t0;
            nReaquecimentos++;
        }
        if (nReaquecimentos == 3) {
            return solucaoAtual;
        }*/
    } while (t0 > tMin && iteracoesSemMelhora < 10 * p->nIterSemMelhoras);

    /*printf("T=%f/%f, Pioras=%d, FO=%.1f / %.1f (%f, %f) [%.3f] It=%ld UM:%ld: %f F*=%.0f\n", t0, tMin, nPioras, foAtual,
            funcaoObjetivo(p, solucaoAtual, pesoHard),
            somaViolacoesHard(p, solucaoAtual), somaViolacoesSoft2(p, solucaoAtual), totalProb / N, pesoHard);*/

    /*if (somaViolacoesHard(p, solucaoAtual) > 0) {
        aumentaPesoHard = 1;
        goto annealing;
    }*/

    return solucaoAtual;
}
