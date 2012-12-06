/* 
 * File:   gerador.h
 * Author: walace
 *
 * Created on 6 de Dezembro de 2012, 12:02
 */

#ifndef GERADOR_H
#define	GERADOR_H

typedef struct gerador {
    int p1, p2;
    int n;
} Gerador;

Gerador *getGeradorInicial(int n);
Individuo *getProxVizinho(Problema*p,Individuo *ind, Gerador *gerador);
void incrementaPosGerador(Gerador *gerador) ;
void imprimeGerador(Gerador *gerador);

#endif	/* GERADOR_H */

