/* 
 * File:   tabu.c
 * Author: walace
 *
 * Created on 2 de Novembro de 2012, 22:17
 */

#include <stdio.h>
#include <stdlib.h>
#include "tabu.h"

/*
 * 
 */

void zeraListaTabu(int *listaTabu, int n) {

    int i, m;

    m = n*n;

    for (i = 0; i < m; i++) {
        listaTabu[i] = 0;
    }
}

Tabu *geraListaTabu(int n) {

    Tabu *listaTabu = (Tabu*) malloc(sizeof (Tabu));

    listaTabu->lista = (long*) malloc(n * sizeof (long));
    listaTabu->nElementos = 0;
    listaTabu->prox = 0;
    listaTabu->tamanho = n;

    return listaTabu;
}

void desalocaListaTabu(Tabu *listaTabu) {
    free(listaTabu->lista);
    free(listaTabu);
}

int estaNoTabu(long code, Tabu* listaTabu) {
    int i, n;

    n = listaTabu->tamanho;
    if (listaTabu->nElementos < listaTabu->tamanho) {
        n = listaTabu->nElementos;
    }

    for (i = 0; i < n; i++) {
        if (code == listaTabu->lista[i]) {
            return 1;
        }
    }
    return 0;
}

void insereTabu(long code, Tabu *listaTabu) {

    listaTabu->lista[listaTabu->prox] = code;
    listaTabu->prox++;

    if (listaTabu->nElementos < listaTabu->tamanho) {
        listaTabu->nElementos++;
    }

    if (listaTabu->prox == listaTabu->tamanho) {
        // volta ao inicio
        listaTabu->prox = 0;
    }
}

void imprimePercListaTabu(int *listaTabu, int n) {
    int m = n*n;
    float total = 0;
    int i;

    for (i = 0; i < m; i++) {
        if (listaTabu[i]) {
            total += 1;
        }
    }

    printf("Perc.: %f\n", total / m);
}

