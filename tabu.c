/* 
 * File:   tabu.c
 * Author: walace
 *
 * Created on 2 de Novembro de 2012, 22:17
 */

#include <stdio.h>
#include <stdlib.h>

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

int *geraListaTabu(int n) {

    int *listaTabu = (int*) malloc(n * n * sizeof (int));

    zeraListaTabu(listaTabu, n);
    
    return listaTabu;
}

void imprimePercListaTabu(int *listaTabu, int n){
    int m = n*n;
    float total = 0;
    int i;
    
    for (i=0;i<m;i++){
        if(listaTabu[i]){
            total += 1;
        }
    }
    
    printf("Perc.: %f\n", total/m);
}

