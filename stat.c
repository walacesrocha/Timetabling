/* 
 * File:   stat.c
 * Author: iris
 *
 * Created on 17 de Agosto de 2011, 20:50
 */

#include <stdio.h>
#include <stdlib.h>

#include "problema.h"
#include "io.h"

float salasAdequadas(Problema *p) {
    int i, j;
    float porcentagem = 0;

    for (i = 0; i < p->nDisciplinas; i++) {

        int estudantes = p->disciplinas[i].nAlunos;
        int nSalasAdequadas = 0;

        for (j = 0; j < p->nSalas; j++) {
            if (estudantes < p->salas[j].capacidade) {
                nSalasAdequadas += 1;
            }
        }

        //printf("Salas adequdas: %d/%d\n", nSalasAdequadas, p->nSalas);

        porcentagem += (float) nSalasAdequadas / p->nSalas;

        //printf("P=%f\n", porcentagem);

    }


    //printf("P=%f\n", porcentagem / p->nDisciplinas);

    return (porcentagem / p->nDisciplinas);
}

float horariosIndisponiveis(Problema *p) {

    int i;
    int totalHorarios = p->nDias * p->nPerDias;
    float porcentagem = 0;

    for (i = 0; i < p->nDisciplinas; i++) {

        porcentagem += (float) p->disciplinas[i].nIndisponibilidades / totalHorarios;
        //printf("Indisp: %f\n", porcentagem);
    }

    return porcentagem / p->nDisciplinas;
}

/*
 * 
 */
int main(int argc, char** argv) {


    Problema *p = lerInstancia(argv[1]);

    printf("Disciplinas: %d\n", p->nDisciplinas);
    printf("Salas: %d\n", p->nSalas);
    printf("Periodos: %d\n", p->nPerDias);
    printf("Dias: %d\n", p->nDias);
    printf("Curriculos: %d\n", p->nCurriculos);


    printf("Salas Adequadas: %f\n", salasAdequadas(p));
    printf("Horarios Indisponiveis: %f\n", horariosIndisponiveis(p));



    return (EXIT_SUCCESS);
}

