/* 
 * File:   tabu.h
 * Author: walace
 *
 * Created on 2 de Novembro de 2012, 22:17
 */

#ifndef TABU_H
#define	TABU_H

typedef struct tabu {
    long *lista;
    int tamanho;
    int nElementos;
    int prox;
    
} Tabu;

void zeraListaTabu(int *listaTabu, int n);

Tabu *geraListaTabu(int n);

void desalocaListaTabu(Tabu *listaTabu) ;

void imprimePercListaTabu(int *listaTabu, int n);

#endif	/* TABU_H */

