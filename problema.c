#include <string.h>
#include "problema.h"

/*
 * Aloca espaço de memória para armazenar um vetor de n disciplinas
 */
Disciplina *alocaDisciplinas(int nDisciplinas) {

    Disciplina *d = (Disciplina*) malloc(nDisciplinas * sizeof (Disciplina));

    return d;
}

/*
 * Aloca espaço de memória para armazenar um vetor de n salas
 */
Sala *alocaSalas(int nSalas) {

    Sala *s = (Sala*) malloc(nSalas * sizeof (Sala));

    return s;
}

/*
 * Aloca espaço de memória para armazenar um vetor de n curriculos
 */
Curriculo *alocaCurriculos(int nCurriculos) {

    Curriculo *c = (Curriculo*) malloc(nCurriculos * sizeof (Curriculo));

    return c;
}

/*
 * Aloca espaço de memória para armazenar um vetor de n restricoes
 */
Restricao *alocaRestricoes(int nRestricoes) {

    Restricao *r = (Restricao*) malloc(nRestricoes * sizeof (Restricao));

    return r;
}

char **alocaVetorDisciplinas(int nDisciplinas) {
    char **vetDisc;
    int i;

    vetDisc = (char**) malloc(nDisciplinas * sizeof (char*));

    for (i = 0; i < nDisciplinas; i++) {
        vetDisc[i] = (char*) malloc(30 * sizeof (char));
    }

    return vetDisc;
}

/*
 * Aloca espaço de memoria para armazenar uma instancia de problema
 */
Problema *alocaProblema() {
    Problema *p = (Problema*) malloc(sizeof (Problema));
    p->nAulas = 0;
    p->aceitaPioraSA = 1;
    p->pesoRC = 1;
    p->pesoMW = 5;
    p->pesoIL = 2;
    p->pesoRS = 1;

    p->vHard = p->comMelhora = p->semEfeito = p->comPiora = 0;
    return p;
}

/*
 * Verifica se duas aulas são da mesma disciplina
 */
int aulasMesmaDisciplina(Problema *p, int a1, int a2) {
    int i;


    for (i = 0; i < p->nDisciplinas; i++) {
        if ((p->disciplinas + i)->aulaInicial <= a1
                && a1 < (p->disciplinas + i)->aulaInicial + (p->disciplinas + i)->nAulas) {
            if ((p->disciplinas + i)->aulaInicial <= a2
                    && a2 < (p->disciplinas + i)->aulaInicial + (p->disciplinas + i)->nAulas) {

                return 1; // a1 e a2 estão sao da mesma disciplina
            }
        }

    }

    return 0;
}

/*
 * Verifica se duas disciplinas são do mesmo curriculo
 */
int disciplinasMesmoCurriculo(Problema *p, char *d1, char*d2) {

    int i, j, t1, t2;

    for (i = 0; i < p->nCurriculos; i++) {
        t1 = t2 = 0; // flag para marcar se d1 e d2 estao no curriculo

        for (j = 0; j < (p->curriculos + i)->nDisciplinas; j++) {
            if (strcmp(d1, (p->curriculos + i)->disciplinas[j]) == 0) {
                t1 = 1; // d1 pertence ao curriculo
            }
            if (strcmp(d2, (p->curriculos + i)->disciplinas[j]) == 0) {
                t2 = 1; // d2 pertence ao curriculo
            }
        }

        if (t1 && t2) {// se tem as duas disciplinas
            //printf ("original: sim\n"); 
            return 1;
        }

    }
    //printf ("original: nao\n");
    return 0;

}

/*
 * Recupera o nome da disciplina pelo numero da aula
 */
char *getDisciplina(Problema *p, int aula) {
    int i;

    for (i = 0; i < p->nDisciplinas; i++) {
        if ((p->disciplinas + i)->aulaInicial <= aula
                && aula < (p->disciplinas + i)->aulaInicial + (p->disciplinas + i)->nAulas) {

            return (p->disciplinas + i)->nomeDisciplina;
        }
    }

    return 0;
}

/*
 * Verifica se duas aulas são do mesmo currículo, ou seja, se suas disciplinas
 * estao no mesmo curriculo.
 */
int aulasMesmoCurriculo(Problema *p, int a1, int a2) {
    int i;
    char *d1, *d2;

    // recupera as disciplinas das aulas
    d1 = getDisciplina(p, a1);
    d2 = getDisciplina(p, a2);

    return disciplinasMesmoCurriculo(p, d1, d2);

}

/* Liberação de memória */
void desalocaProblema(Problema *p) {
    int i;

    //printf("desalocando disciplinas...\n");

    // desaloca as disciplinas
    free(p->disciplinas);

    //printf("desalocando curriculos...\n");
    // desaloca curriculos
    for (i = 0; i < p->nCurriculos; i++) {
        desalocaCurriculo(p->curriculos + i);
    }
    free(p->curriculos);

    //printf("desalocando salas...\n");
    // desaloca salas
    for (i = 0; i < p->nSalas; i++) {
        desalocaSala(p->salas + i);
    }
    free(p->salas);

    //printf("desalocando restricoes...\n");
    // desaloca restrições
    for (i = 0; i < p->nRestricoes; i++) {
        desalocaRestricao(p->restricoes + i);
    }
    free(p->restricoes);


    /*Leandro inicio*/
    liberaMatrizAulaSala(p);
    liberaMatrizAulaAula(p);
    liberaMatrizAulaIndisponibilidades(p);
    liberaSlotsDisponiveisTodasDisciplinas(p);
    /*Leandro fim*/

    //printf("desalocando problema...\n");

    free(p);

    //printf("completo...\n");
}

void desalocaDisciplina(Disciplina *d) {
    free(d);
}

void desalocaCurriculo(Curriculo *c) {
    int i;

    for (i = 0; i < c->nDisciplinas; i++) {
        free(c->disciplinas[i]);
    }
    free(c->disciplinas);
}

void desalocaSala(Sala *s) {
    //free(s);
}

void desalocaRestricao(Restricao *r) {
    //free(r);
}

/*Leandro inicio*/

int nAulasDisciplina(Disciplina *d) {
    return d->nAulas;
}

void calculaNumeroTotalAulas(Problema *p) {
    int i;
    int soma = 0;
    for (i = 0; i < p->nDisciplinas; i++) {
        soma = soma + nAulasDisciplina(&(p->disciplinas[i]));
    }
    p->nAulas = soma;
}

Disciplina *acessaDisciplina(Problema *p, int aula) {
    int i;

    for (i = 0; i < p->nDisciplinas; i++) {
        if ((p->disciplinas + i)->aulaInicial <= aula
                && aula < (p->disciplinas + i)->aulaInicial + (p->disciplinas + i)->nAulas) {

            return (p->disciplinas + i);
        }
    }

    return NULL;

}

void copiaIndisponibilidades(Disciplina *d, int *aux, int ni) {
    int i;
    for (i = 0; i < ni; i++) {
        d->indisponibilidades[i] = aux[i];
    }
    d->nIndisponibilidades = ni;
}

void insereIndisponibilidadesNasDisciplinas(Problema *p) {
    int i, j, ni;
    int aux[p->nDias * p->nPerDias];
    Restricao *r;
    Disciplina *d;

    for (i = 0; i < p->nDisciplinas; i++) {
        d = &(p->disciplinas[i]);
        ni = 0;
        for (j = 0; j < p->nRestricoes; j++) {
            r = &(p->restricoes[j]);
            if (strcmp(d->nomeDisciplina, r->nomeDisciplina) == 0) {
                aux[ni] = (r->dia * p->nPerDias) + r->periodo;
                ni++;
            }
        }
        d->indisponibilidades = (int*) malloc(ni * sizeof (int));
        copiaIndisponibilidades(d, aux, ni);
    }
}

void insereCurriculosNasDisciplinas(Problema *p) {
    int i, j, k, nc;
    Disciplina *d;
    Curriculo *c;

    for (i = 0; i < p->nDisciplinas; i++) {
        d = &(p->disciplinas[i]);
        nc = 0;
        for (j = 0; j < p->nCurriculos; j++) {

            c = &(p->curriculos[j]);
            for (k = 0; k < c->nDisciplinas; k++) {
                if (strcmp(d->nomeDisciplina, c->disciplinas[k]) == 0) {
                    nc++;
                }
            }
        }
        d->curriculos = (Curriculo**) malloc(nc * sizeof (Curriculo*));
        d->nCurriculos = 0;

        for (j = 0; j < p->nCurriculos; j++) {

            c = &(p->curriculos[j]);
            for (k = 0; k < c->nDisciplinas; k++) {
                if (strcmp(d->nomeDisciplina, c->disciplinas[k]) == 0) {
                    d->curriculos[d->nCurriculos] = c;
                    d->nCurriculos = d->nCurriculos + 1;
                }
            }
        }

    }
}

void liberaDisciplina(Disciplina *d) {
    free(d->curriculos);
    free(d->indisponibilidades);
    free(d->nomeDisciplina);
    free(d->professor);
    free(d);
}

int getTimeSlotFromPos(int pos, Problema *p) {
    return pos % (p->nDias * p->nPerDias);
}

int getDiaFromPos2(int pos, Problema *p) {
    int q = pos % (p->nDias * p->nPerDias);
    q = q / p->nPerDias;

    return q;
}

int getPeriodoFromPos2(int pos, Problema *p) {
    int q = pos % (p->nDias * p->nPerDias);
    q = q % p->nPerDias;

    return q;
}

void imprimeDisciplina(Problema *p, Disciplina *d) {
    int i, r;

    printf("Nome Disciplina: %s\n", d->nomeDisciplina);
    printf("Professor: %s\n", d->professor);
    printf("Numero de aulas: %d\n", d->nAulas);
    printf("Aula inicial: %d\n", d->aulaInicial);
    printf("Minimo de dias: %d\n", d->minDiasAula);
    printf("Numero de alunos: %d\n", d->nAulas);

    printf("Curriculos (%d):	", d->nCurriculos);
    for (i = 0; i < d->nCurriculos; i++) {
        printf("%s,  ", (d->curriculos[i])->nomeCurriculo);
    }
    printf("\n");

    printf("Indisponibilidades (%d):	", d->nIndisponibilidades);
    for (i = 0; i < d->nIndisponibilidades; i++) {
        r = d->indisponibilidades[i];
        printf("%d, dia: %d, horario: %d;  ", r, getDiaFromPos2(r, p), getPeriodoFromPos2(r, p));
    }

    printf("\n\n");
}

void ordenaDisciplinasPorRestricoes(Problema *p) {
    int i, j;
    Disciplina chave;

    for (j = 1; j < p->nDisciplinas; j++) {
        chave = p->disciplinas[j];
        i = j - 1;
        while (i >= 0 && p->disciplinas[i].nIndisponibilidades < chave.nIndisponibilidades) {
            p->disciplinas[i + 1] = p->disciplinas[i];
            i--;
        }
        p->disciplinas[i + 1] = chave;
    }
}

void imprimeTodasDisciplinas(Problema *p) {
    int i;
    Disciplina *d;

    for (i = 0; i < p->nDisciplinas; i++) {
        d = &(p->disciplinas[i]);
        imprimeDisciplina(p, d);
        printf("\n");
    }
}

int disciplinasMesmoCurriculo2(Problema *p, Disciplina *d1, Disciplina *d2) {
    int i1, i2;
    Curriculo *c1;

    for (i1 = 0; i1 < d1->nCurriculos; i1++) {
        c1 = d1->curriculos[i1];
        for (i2 = 0; i2 < d2->nCurriculos; i2++) {
            if (c1 == d2->curriculos[i2]) {
                //printf ("Disps mesm curric  3: sim;   "); 
                return 1;
            }
        }
    }
    //printf ("Disps mesm curric  3: nao;   "); 
    return 0;
}

int aulasMesmoCurriculo2(Problema *p, int a1, int a2) {
    int i;
    Disciplina *pd1, *pd2;


    //if (a1 > (p->nAulas) || a2 > (p->nAulas) )printf ("aula vazia 222222222222222\n");//////////
    // recupera as disciplinas das aulas     
    pd1 = acessaDisciplina(p, a1);
    pd2 = acessaDisciplina(p, a2);

    return disciplinasMesmoCurriculo2(p, pd1, pd2);

}

int aulasMesmoProfessor(Problema *p, int aula1, int aula2) {
    Disciplina *disp1 = acessaDisciplina(p, aula1);
    Disciplina *disp2 = acessaDisciplina(p, aula2);

    if (strcmp(disp1->professor, disp2->professor) == 0) {
        return 1;
    }
    return 0;
}

/*Leandro fim*/
