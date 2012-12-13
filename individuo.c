#include <string.h>

#include "individuo.h"
#include "util.h"
#include "fitness.h"
//aulas comecam com 1

/* Estrutura:

typedef struct individuo{
                int *aula;
                float fitness;
                int n;
}Individuo;

 */

int numero_aleatorio(int maxrand) {
    int n;
    /////n =(rand()%MAXRAND)+1;
    n = rand() % maxrand;
    return n;
}

void troca_par(Individuo *a, int pos1, int pos2) {
    int aux;
    aux = a->aula[pos1];
    a->aula[pos1] = a->aula[pos2];
    a->aula[pos2] = aux;
}

void checaAulasIsoladasEmCurriculo(Problema *p, Individuo *ind, int c){
    
}

void troca_par_completo(Problema *p, Individuo *a, int pos1, int pos2) {
    int i, aux;
    int dia1, dia2, periodo1, periodo2, sala1, sala2;
    int idx1, idx2;
    Disciplina *disc1, *disc2;

    disc1 = acessaDisciplina(p, a->aula[pos1]);
    disc2 = acessaDisciplina(p, a->aula[pos2]);

    dia1 = getDiaFromPos(pos1, p);
    dia2 = getDiaFromPos(pos2, p);

    periodo1 = getPeriodoFromPos(pos1, p);
    periodo2 = getPeriodoFromPos(pos2, p);

    sala1 = getSalaFromPos(p, pos1);
    sala2 = getSalaFromPos(p, pos2);

    idx1 = disc1->pVetor;
    if (disc2) {
        idx2 = disc2->pVetor;
    }

    for (i = 0; i < disc1->nCurriculos; i++) {
        a->currDiasPeriodos[disc1->curriculos[i]->pVetor][dia1][periodo1]--;
        a->currDiasPeriodos[disc1->curriculos[i]->pVetor][dia2][periodo2]++;
    }
    if (disc2) {
        for (i = 0; i < disc2->nCurriculos; i++) {
            a->currDiasPeriodos[disc2->curriculos[i]->pVetor][dia1][periodo1]++;
            a->currDiasPeriodos[disc2->curriculos[i]->pVetor][dia2][periodo2]--;
        }
    }

    a->salasUsadas[idx1][sala1]--;
    a->salasUsadas[idx1][sala2]++;

    a->diasOcupados[idx1][dia1]--;
    a->diasOcupados[idx1][dia2]++;

    if (disc2) {
        a->salasUsadas[idx2][sala1]++;
        a->salasUsadas[idx2][sala2]--;

        a->diasOcupados[idx2][dia1]++;
        a->diasOcupados[idx2][dia2]--;
    }

    aux = a->aula[pos1];
    a->aula[pos1] = a->aula[pos2];
    a->aula[pos2] = aux;
}

Individuo *alocaIndividuo(void) {
    Individuo *ind = (Individuo*) malloc(sizeof (Individuo));
    return ind;
}

void criaIndividuo(Individuo *a, Problema *p) {
    int i, j, k;
    a->n = (p->nSalas) * (p->nDias) * (p->nPerDias);
    a->fitness = 0;
    a->aula = (int*) malloc(a->n * sizeof (int));

    // matriz 3-D para armazenar curriculos, dias e periodos
    a->currDiasPeriodos = (int***) malloc(p->nCurriculos * sizeof (int**));
    for (i = 0; i < p->nCurriculos; i++) {
        a->currDiasPeriodos[i] = (int**) malloc(p->nDias * sizeof (int*));
        for (j = 0; j < p->nDias; j++) {
            a->currDiasPeriodos[i][j] = (int*) malloc(p->nPerDias * sizeof (int));
        }
    }

    for (i = 0; i < p->nCurriculos; i++) {
        for (j = 0; j < p->nDias; j++) {
            for (k = 0; k < p->nPerDias; k++) {
                a->currDiasPeriodos[i][j][k] = 0;
            }
        }
    }

    // matriz disciplina x sala
    a->salasUsadas = (int**) malloc(p->nDisciplinas * sizeof (int*));
    for (i = 0; i < p->nDisciplinas; i++) {
        a->salasUsadas[i] = (int*) malloc(p->nSalas * sizeof (int));
        memset(a->salasUsadas[i], 0, p->nSalas * sizeof (int));
    }

    // matriz disciplina x dia
    a->diasOcupados = (int**) malloc(p->nDisciplinas * sizeof (int*));
    for (i = 0; i < p->nDisciplinas; i++) {
        a->diasOcupados[i] = (int*) malloc(p->nDias * sizeof (int));
        memset(a->diasOcupados[i], 0, p->nDias * sizeof (int));
    }

    // matriz curriculo x flag (flag=0/1, indica se curriculo tem penalidade IL) 
    a->curriculosComIL = (int*) malloc(p->nCurriculos * sizeof (int));
    memset(a->curriculosComIL, 0, p->nCurriculos * sizeof (int));

}

Individuo *copiaIndividuo(Problema *p, Individuo *origem) {
    Individuo *destino;
    int i;

    destino = alocaIndividuo();
    criaIndividuo(destino, p);

    destino->fitness = origem->fitness;
    destino->n = origem->n;
    memcpy(destino->aula, origem->aula, origem->n * sizeof (int));
    /*for (i = 0; i < (origem->n); i++) {
        destino->aula[i] = origem->aula[i];

    }*/
    return destino;

}

void inicializaIndividuo2(Individuo *a) {
    int i;
    int valor_nulo = (2 * (a->n)) + 55;

    //int valor_nulo = -55;
    //printf("valor_nulo: %d\n", valor_nulo); exit(1);/////////
    for (i = 0; i < a->n; i++) {
        a->aula[i] = valor_nulo;
    }

}

/* preenche o vetor com as posicoes*/
void inicializaIndividuo(Individuo *a, int numero_total_aulas) {
    int i;
    for (i = 0; i < a->n; i++) {
        a->aula[i] = i + 1;
    }

}

void zeraMatCurrDiasPeriodos(Problema *p, Individuo*ind) {
    int i, j, periodo;

    for (i = 0; i < p->nCurriculos; i++) {
        for (j = 0; j < p->nDias; j++) {
            for (periodo = 0; periodo < p->nPerDias; periodo++) {
                ind->currDiasPeriodos[i][j][periodo] = 0;
            }
        }
    }
}

void inicializaMatrizesAuxiliares(Problema *p, Individuo*ind) {
    int i, j, dia, periodo, sala;
    for (i = 0; i < p->dimensao; i++) {

        if (ehAula(p, ind->aula[i])) {
            dia = getDiaFromPos(i, p);
            periodo = getPeriodoFromPos(i, p);
            sala = getSalaFromPos(p, i);

            // curr X dia X periodo
            Disciplina *disc = acessaDisciplina(p, ind->aula[i]);
            for (j = 0; j < disc->nCurriculos; j++) {
                ind->currDiasPeriodos[disc->curriculos[j]->pVetor][dia][periodo] += 1;
            }

            // disciplina X dia
            ind->diasOcupados[disc->pVetor][dia]++;

            // disciplina X sala
            ind->salasUsadas[disc->pVetor][sala]++;
        }
    }

    somaAulasIsoladas(p, ind);

}

void imprimeMatCurrDiasPeriodo(Problema *p, Individuo *ind) {
    int i, j, periodo;

    for (i = 0; i < p->nCurriculos; i++) {
        printf("Curr: %s\n", (p->curriculos + i)->nomeCurriculo);
        for (j = 0; j < p->nDias; j++) {
            for (periodo = 0; periodo < p->nPerDias; periodo++) {
                printf("%d ", ind->currDiasPeriodos[i][j][periodo]);
            }
            printf("\n");
        }
        printf("----------------------");
    }
}
//retorna 1 se houver troca, ou 0 caso contrario
//soh realiza troca se pelo menos 1 das posicoes possui aula, e se a troca nao viola restricoes hard

int troca_par_restrita(Problema *p, Individuo *indiv, int pos1, int pos2) {
    if (ehAula(p, indiv->aula[pos1]) || ehAula(p, indiv->aula[pos2])) {
        troca_par(indiv, pos1, pos2);

        if (somaViolacoesHardTroca(p, indiv, pos1, pos2) == 0) {
            //printf("trocou\n");////////
            return 1;
        } else {
            troca_par(indiv, pos1, pos2); //destroca		    
        }

    }
    //printf("NAO trocou\n");////////
    return 0; // nao houve troca       
}

//nao usada ////////

int violacoesHard1Local(Problema *p, Individuo *a, int pos) {
    int sala, i, incremento, soma, aula;

    soma = 0;
    incremento = p->nPerDias * p->nDias;

    for (i = pos + incremento; i < a->n; i += incremento) {

        aula = a->aula[i];
        if (ehAula(p, aula)) {
            if (aulasMesmaDisciplina3(p, a->aula[pos], aula)) {
                soma++;
            }
        }
    }

    return soma;
}





// verifica aulas da mesma disciplina no Mesmo Horario
//verifica somente disciplina da posicao pos com disciplinas abaixo. Deve ser chamada para todas as posicoes exceto a
// ultima sala

float violacoesHard1Local2(Problema *p, Individuo *a, int pos) {
    int sala, i, incremento, aula;
    float soma;

    soma = 0.0;
    incremento = p->nPerDias * p->nDias;

    for (i = pos + incremento; i < a->n; i += incremento) {

        aula = a->aula[i];
        if (ehAula(p, aula)) {
            if (aulasMesmaDisciplina3(p, a->aula[pos], aula)) {

                soma++;
            }
        }
    }
    //printf("hard 1: %f, operacao: %f\n", soma, soma*0.999);
    return soma;
}


//conflitos

float violacoesHard3(Problema *p, Individuo *a, int pos) {
    int sala, i, j, a1, a2;
    float nViolacoes = 0.0;

    for (i = 0; i < (p->nSalas) - 1; i++) {
        a1 = a->aula[posAulaSala(p, pos, i)];
        if (ehAula(p, a1)) {
            for (j = i + 1; j < p->nSalas; j++) {
                a2 = a->aula[posAulaSala(p, pos, j)];
                if (ehAula(p, a2)) {
                    if (aulasConflitantes(p, a1, a2)) {
                        nViolacoes = nViolacoes + 1;
                    }

                }
            }
        }
    }
    return nViolacoes;
}


//Disponibilidades
//periodo aqui significa horario (ex: 14-15 horas)
//para uma posicao do vetor: checa se eh aula, obtem codigo da disciplina, varre vetor restricoes verificando se existe violacao e soma as violacoes

float violacoesHard4(Problema *p, Individuo *a, int pos) {
    int aula;

    aula = a->aula[pos];
    if (ehAula(p, aula)) {
        return (float) horarioIndisponivel(p, aula, pos);
    }
    return 0.0;
}


// float somaViolacoesHardLocalBkp(Problema *p, Individuo *a, int pos) {
//     int i;
//     float violacoes1, violacoes4, violacoes3 ;
//     int ini = (getDiaFromPos (pos, p) * p->nPerDias) + getPeriodoFromPos(pos,p);    
//     int incremento = p->nPerDias * p->nDias;
//     
//     //printf ("somaViolacoesHardLocal: pos = %d, ini = %d\n", pos, ini); //////////
//     violacoes3 = violacoesHard3(p, a, ini);
//     violacoes1 = violacoes4 = 0.0;
//     for (i= ini; i< a->n; i+= incremento){
//         violacoes1+= violacoesHard1Local2(p, a, i);
//         violacoes4 += violacoesHard4(p, a, i);
//     }
//     //printf("v1: %f, v3: %f, v4: %f\n", violacoes1, violacoes3, violacoes4);/////////
//     return (float)(violacoes1 + violacoes3 + violacoes4);
// }


//conflitos de "aula"

float violacoesHard3_local_disp(Problema *p, Individuo *a, int ini, int aula) {
    int sala, i, j, a1, a2;
    int incremento = p->nPerDias * p->nDias;
    float nViolacoes = 0.0;

    for (i = ini; i < (p->dimensao); i = i + incremento) {
        a2 = a->aula[i];
        if (a2 != aula) {
            if (ehAula(p, a2)) {
                if (aulasConflitantes(p, aula, a2)) {
                    nViolacoes = nViolacoes + 1;
                }

            }

        }
    }
    return nViolacoes;
}

float somaViolacoesHardLocal(Problema *p, Individuo *a, int pos) {
    float violacoes1, violacoes4, violacoes3;
    int ini = (getDiaFromPos(pos, p) * p->nPerDias) + getPeriodoFromPos(pos, p);
    int incremento = p->nPerDias * p->nDias;
    int aula;


    aula = a->aula[pos];

    violacoes1 = 0.0;
    violacoes3 = 0.0;
    violacoes4 = 0.0;
    if (ehAula(p, aula)) {
        violacoes1 = (float) eliminaAulasMultiplasLocal(p, a, ini, aula);

        //apos eliminacao de aulas multiplas aula atual pode ter sido eliminada
        if (ehAula(p, a->aula[pos])) {
            violacoes3 = violacoesHard3_local_disp(p, a, ini, aula);
            violacoes4 = violacoesHard4(p, a, pos);
        }


        restauraAulasMultiplasLocal(p, a, ini);
    }

    //v1, v3, v4
    //printf("%f,%f,%f\n", violacoes1, violacoes3, violacoes4);/////////

    return (float) (violacoes1 + violacoes3 + violacoes4);
}

void restauraAulasMultiplasLocalInicializacao(Problema *p, Individuo *ind, int ini) {
    int i;
    int incremento = p->nPerDias * p->nDias;

    //for (i=0; i< (p->dimensao);i++){ //////
    for (i = ini; i < (p->dimensao); i += incremento) {
        //if ((ind->aula[i]) > (p->dimensao)){
        if (((ind->aula[i]) > (p->dimensao)) && ((ind->aula[i]) < (2 * (p->dimensao)))) {
            ind->aula[i] = (ind->aula[i]) - (p->dimensao) - 10;
        }
    }
}

float somaViolacoesHardLocalInicializacao(Problema *p, Individuo *a, int pos) {
    float violacoes1, violacoes4, violacoes3;
    int ini = (getDiaFromPos(pos, p) * p->nPerDias) + getPeriodoFromPos(pos, p);
    int incremento = p->nPerDias * p->nDias;
    int aula;


    aula = a->aula[pos];

    violacoes1 = 0.0;
    violacoes3 = 0.0;
    violacoes4 = 0.0;
    if (ehAula(p, aula)) {
        violacoes1 = (float) eliminaAulasMultiplasLocal(p, a, ini, aula);

        //apos eliminacao de aulas multiplas aula atual pode ter sido eliminada
        if (ehAula(p, a->aula[pos])) {
            violacoes3 = violacoesHard3_local_disp(p, a, ini, aula);
            violacoes4 = violacoesHard4(p, a, pos);
        }


        restauraAulasMultiplasLocalInicializacao(p, a, ini);
    }

    //v1, v3, v4
    //printf("%f,%f,%f\n", violacoes1, violacoes3, violacoes4);/////////

    return (float) (violacoes1 + violacoes3 + violacoes4);
}

float somaViolacoesHardLocalTeste(Problema *p, Individuo *a, int pos, float *violacoes1, float *violacoes3, float *violacoes4) {
    int ini = (getDiaFromPos(pos, p) * p->nPerDias) + getPeriodoFromPos(pos, p);
    int incremento = p->nPerDias * p->nDias;
    int aula;


    aula = a->aula[pos];

    *violacoes1 = 0.0;
    *violacoes3 = 0.0;
    *violacoes4 = 0.0;
    if (ehAula(p, aula)) {
        *violacoes1 = (float) eliminaAulasMultiplasLocal(p, a, ini, aula);

        //apos eliminacao de aulas multiplas aula atual pode ter sido eliminada
        if (ehAula(p, a->aula[pos])) {
            *violacoes3 = violacoesHard3_local_disp(p, a, ini, aula);
            *violacoes4 = violacoesHard4(p, a, pos);
        }


        restauraAulasMultiplasLocal(p, a, ini);
    }

    //v1, v3, v4
    //printf("%f,%f,%f\n", violacoes1, violacoes3, violacoes4);/////////

    return (float) (*violacoes1 + *violacoes3 + *violacoes4);
}

float somaConflitosLocaisTeste(Problema *p, Individuo *a, int pos) {
    float violacoes1, violacoes4, violacoes3;
    int ini = (getDiaFromPos(pos, p) * p->nPerDias) + getPeriodoFromPos(pos, p);
    int incremento = p->nPerDias * p->nDias;
    int aula;


    aula = a->aula[pos];

    violacoes1 = 0.0;
    violacoes3 = 0.0;
    violacoes4 = 0.0;
    if (ehAula(p, aula)) {
        violacoes1 = (float) eliminaAulasMultiplasLocal(p, a, ini, aula);

        //apos eliminacao de aulas multiplas aula atual pode ter sido eliminada
        if (ehAula(p, a->aula[pos])) {
            violacoes3 = violacoesHard3_local_disp(p, a, ini, aula);
            violacoes4 = violacoesHard4(p, a, pos);
        }


        restauraAulasMultiplasLocal(p, a, ini);
    }

    //v1, v3, v4
    //printf("%f,%f,%f\n", violacoes1, violacoes3, violacoes4);/////////

    return (float) (violacoes3);
}

float somaIndisponibilidadesLocaisTeste(Problema *p, Individuo *a, int pos) {
    float violacoes1, violacoes4, violacoes3;
    int ini = (getDiaFromPos(pos, p) * p->nPerDias) + getPeriodoFromPos(pos, p);
    int incremento = p->nPerDias * p->nDias;
    int aula;


    aula = a->aula[pos];

    violacoes1 = 0.0;
    violacoes3 = 0.0;
    violacoes4 = 0.0;
    if (ehAula(p, aula)) {
        violacoes1 = (float) eliminaAulasMultiplasLocal(p, a, ini, aula);

        //apos eliminacao de aulas multiplas aula atual pode ter sido eliminada
        if (ehAula(p, a->aula[pos])) {
            violacoes3 = violacoesHard3_local_disp(p, a, ini, aula);
            violacoes4 = violacoesHard4(p, a, pos);
        }


        restauraAulasMultiplasLocal(p, a, ini);
    }

    //v1, v3, v4
    //printf("%f,%f,%f\n", violacoes1, violacoes3, violacoes4);/////////

    return (float) (violacoes4);
}

int linhaColunaParaPos(Problema *p, int linha, int coluna) {
    int nColunas = (p->nPerDias) * (p->nDias);
    return (linha * nColunas) +coluna;
}

float somaTodasHardLocalTeste(Problema *p, Individuo *ind, float *h1total, float *h3total, float *h4total, float *somaHardLocal) {
    int i, j, nColunas, pos;
    float soma;
    float h1, h3, h4;
    int dispsContadas[p->nSalas]; //armazena disciplinas onde ocorreram violacao1 em uma coluna
    int nDisps, cont;
    int casos[p->nSalas];



    nColunas = (p->nPerDias) * (p->nDias);

    //inicializa vertor de disciplinas contadas
    for (cont = 0; cont < (p->nSalas); cont++) {
        dispsContadas[cont] = (p->dimensao + 20);
        nDisps = 0;
    }


    *h4total = 0.0;
    *h3total = 0.0;
    *h1total = 0.0;
    soma = 0.0;
    for (j = 0; j < nColunas; j++) {
        for (cont = 0; cont < (p->nSalas); cont++) {
            casos[cont] = 0;
        }
        for (i = 0; i < (p->nSalas); i++) {
            pos = linhaColunaParaPos(p, i, j);
            //printf ("i: %d, j: %d, pos: %d, aula: %d\n", i, j, pos, ind->aula[pos]);////////
            h1 = 0.0;
            h3 = 0.0;
            h4 = 0.0;
            soma = soma + somaViolacoesHardLocalTeste(p, ind, pos, &h1, &h3, &h4);
            *h1total = *h1total + h1;
            *h3total = *h3total + h3;
            *h4total = *h4total + h4;

            //printf ("h1total: %f, h1: %f\n", *h1total, h1);
            if (ehAula(p, ind->aula[pos])) {
                //insere primeiro valor no vetor		    
                if (nDisps == 0) {
                    dispsContadas[nDisps] = ind->aula[pos];
                    nDisps = nDisps + 1;
                    //printf("**primeira aula\n") ;printf ("dispsContadas:[ %d, %d, %d], nDisps: %d\n", dispsContadas[0], dispsContadas[1], dispsContadas[2], nDisps);///////
                } else {
                    //printf("verifica aula\n") ;
                    for (cont = 0; cont < nDisps; cont++) {
                        //printf ("aula: %d, pos: %d\n", ind->aula[pos], pos); 
                        //printf ("Antes insersao, cont: %d, nDisps: %d, dispsContadas: %d, %d, %d\n", cont, nDisps, dispsContadas[0], dispsContadas[1], dispsContadas[2]);
                        if (!aulasMesmaDisciplina(p, dispsContadas[cont], ind->aula[pos])) {
                            //printf("disciplinas diferentes -> no vetor: %d, na tabela: %d\n",  dispsContadas[cont], ind->aula[pos]);								
                        } else {
                            //printf("dispVet: %d, dispTabela: %d\n", dispsContadas[cont], ind->aula[pos]);
                            //printf ("coluna: %d, linha: %d, dispsContadas0: %d, dispsContadas1: %d, dispsContadas2: %d, nDisps: %d, h1total antes atualiza: %f\n", j,i, dispsContadas[0], dispsContadas[1], dispsContadas[2], nDisps, *h1total);///////
                            //printf ("aula: %d, h1: %f, h1total: %f\n\n", ind->aula[pos], h1, *h1total);			
                            casos[cont] = 1;
                            cont = nDisps + 10; //aborta a verificacao no vetor
                        }
                    }
                    if (cont == nDisps) {
                        //printf ("Aula inserida -> dispsContadas antes: [%d, %d, %d], nDisps: %d\n", dispsContadas[0], dispsContadas[1], dispsContadas[2], nDisps);///////				
                        dispsContadas[nDisps] = ind->aula[pos]; //insere no vetor
                        nDisps = nDisps + 1;
                        //printf ("Aula inserida -> dispsContadas depois: [%d, %d, %d], nDisps: %d\n", dispsContadas[0], dispsContadas[1], dispsContadas[2], nDisps);///////				
                    } else {
                        //printf("aula repetida, cont: %d, h1total antes: %f, ",cont, *h1total);							
                        //*h1total = *h1total - 1; //remove aulas nao duplicadas
                        //printf("h1total depois: %f\n, ", *h1total);
                    }

                }
            }
            //printf ("\n\n\n");

        }
        for (cont = 0; cont < (p->nSalas); cont++) {
            *h1total = *h1total - casos[cont];
        }
        //printf ("coluna: %d, dispsContadas0: %d, dispsContadas1: %d, dispsContadas2: %d, nDisps: %d\n", j, dispsContadas[0], dispsContadas[1], dispsContadas[2], nDisps);///////
        for (cont = 0; cont < (p->nSalas); cont++) {
            dispsContadas[cont] = (p->dimensao + 20);
            nDisps = 0;
        }
        //reinicializa vertor de disciplinas contadas


    }
    *h3total = *h3total / 2;
    *somaHardLocal = soma;
    return (*h3total + *h4total + *h1total);

}

float somaTodosConflitosLocaisTeste(Problema *p, Individuo *ind) {
    int i;
    float soma;
    //printf ("v1,v3,v4\n");//////
    soma = 0.0;
    for (i = 0; i < (p->dimensao); i++) {
        soma = soma + somaConflitosLocaisTeste(p, ind, i);
    }
    return soma;
}

float somaTodasIndisponibilidadesLocaisTeste(Problema *p, Individuo *ind) {
    int i;
    float soma;
    //printf ("v1,v3,v4\n");//////
    soma = 0.0;
    for (i = 0; i < (p->dimensao); i++) {
        soma = soma + somaIndisponibilidadesLocaisTeste(p, ind, i);
    }
    return soma;
}

float somaTodasViolacoesHardLocal(Problema *p, Individuo *ind) {
    int i;
    float soma;
    //printf ("v1,v3,v4\n");//////
    soma = 0.0;
    for (i = 0; i < (p->dimensao); i++) {
        soma = soma + somaViolacoesHardLocal(p, ind, i);
    }
    return soma;
}


//Modificado
//verificacao de slot disponivel checando todas as restricoes hard
/*
int slotDisponivelModificado (Problema *p, Individuo *ind, Disciplina *d, int aula, int pos){
    int i, timeSlot, valorOriginal;
    timeSlot = getTimeSlotFromPos (pos, p);
    
    valorOriginal = ind->aula[pos];
    //verifica se slot jah estah preenchido com uma aula
    if (	ehAula (p, valorOriginal)	){
        return 0;
    }
    //if(valorOriginal == 92){printf ("aula: %d, aula da posicao: %d, pos: %d\n", aula, valorOriginal, pos);imprimeIndividuo2(p, ind); exit(1);}////////
    ind->aula[pos] = aula;
    //verifica se nao sera violada nenhuma restricao hard
   if (         somaViolacoesHardLocalInicializacao (p, ind, pos) > 0){
   
       ind->aula[pos] = valorOriginal;
       return 0;
   }   
    ind->aula[pos] = valorOriginal; //otimizar: nao retornar valor original. Deixar armazenada a aula    
        
    return 1;
}
//*/

//artigo 13
//verificacao de slot disponivel original

int slotDisponivel(Problema *p, Individuo *ind, Disciplina *d, int aula, int pos) {
    int i, timeSlot, valorOriginal;
    timeSlot = getTimeSlotFromPos(pos, p);

    valorOriginal = ind->aula[pos];
    //verifica se slot jah estah preenchido com uma aula
    if (ehAula(p, valorOriginal)) {
        return 0;
    }
    //if(valorOriginal == 92){printf ("aula: %d, aula da posicao: %d, pos: %d\n", aula, valorOriginal, pos);imprimeIndividuo2(p, ind); exit(1);}////////
    ind->aula[pos] = aula;
    //verifica se nao sera violada a indisponibilidade    
    if (violacoesHard4(p, ind, pos) > 0) {
        //if (         somaViolacoesHardLocal (p, ind, pos) > 0){
        ind->aula[pos] = valorOriginal;
        return 0;
    }
    ind->aula[pos] = valorOriginal; //otimizar: nao retornar valor original. Deixar armazenada a aula    

    return 1;
}

int proximaPosicaoVazia(Problema *p, Individuo *ind, int pos) {
    int posIni = pos;
    do {
        pos++;
        if (pos >= ind->n) {
            pos = 0;
        }
    } while (ehAula(p, ind->aula[pos]) && pos != posIni);
    //if (pos==posIni) printf("nenhum slot vazio encontrado !!!!!!!\n");/////////////    
    return pos;
}

int proximaPosicao(Problema *p, Individuo *ind, Disciplina *d, int aula, int pos) {
    int posIni = pos;
    do {
        pos++;
        if (pos >= ind->n) {
            pos = 0;
        }
    } while (!slotDisponivel(p, ind, d, aula, pos) && pos != posIni);
    if (pos == posIni) {
        //printf("nenhuma posicao disponivel encontrada !\n");

        pos = proximaPosicaoVazia(p, ind, pos);

    }
    return pos;
}


//sorteia 50 vezes antes de buscar proxima posicao

void insereDisciplina(Problema *p, Individuo *ind, Disciplina *d) {
    int aula, pos, nTentativas;
    for (aula = d->aulaInicial; aula < (d->aulaInicial + d->nAulas); aula++) {
        nTentativas = 50;
        pos = numero_aleatorio(ind->n);
        while (nTentativas > 0) {
            if (slotDisponivel(p, ind, d, aula, pos)) {
                nTentativas = -55;
            } else {
                pos = numero_aleatorio(ind->n);
                nTentativas = nTentativas - 1;

            } //printf("pos: %d\n", pos);/////////

        }
        if (nTentativas == 0) {
            // printf("prox pos\n");///////
            pos = proximaPosicao(p, ind, d, aula, pos);
        }
        ind->aula[pos] = aula;
    }
    //printf ("\n");//////////
}

void preenchePosicoesVazias(Problema *p, Individuo *ind) {
    int i, val;
    //val = (ind->n)+1;
    val = (p->nAulas) + 1;
    for (i = 0; i < ind->n; i++) {
        if (!ehAula(p, ind->aula[i])) {
            //if ( ind->aula[i] < 0	){	
            ind->aula[i] = val;
            val++;
        }
    }
}


/* realiza nTrocas (se possivel) sem violar restricoes hard*/
// void embaralhaIndividuo2(Problema *p, Individuo *a, int nTrocas) {
//     int i, pos1, pos2, nTentativas;
//     printf ("Embaralhando individuo\n");///////
//     for (i = 0; i < nTrocas; i++) {
//         pos1 = numero_aleatorio(a->n);
//         pos2 = numero_aleatorio(a->n);
// 	for (nTentativas = 0; nTentativas < MAXTENTATIVAS; nTentativas++){
// 	    if (troca_par_restrita(p, a, pos1, pos2)){
// 		nTentativas = MAXTENTATIVAS + 33;
// 	    }
// 	    //printf ("nTentativa: %d\n", nTentativas);///////
// 	}	
//     }
// 
// }

void imprimeVetorSlotsDisponiveis(int *v, int tamVet) {
    int i;

    for (i = 0; i < tamVet; i++) {
        printf("%d  ", v[i]);
    }
    printf("\ntamVet: %d\n\n", tamVet);

}

/* cria um Individuo semi aleatorio respeitando as disponibilidades das disciplinas*/
Individuo *geraIndividuoAleatorio(Problema *p, int nTrocas) {
    int i;
    Individuo *ind = alocaIndividuo();

    criaIndividuo(ind, p);
    inicializaIndividuo2(ind);


    for (i = 0; i < p->nDisciplinas; i++) {
        insereDisciplina(p, ind, &(p->disciplinas[i]));
    }

    //printf("Antes de preencher posicoes, numero de aulas = %d\n", p->nAulas);imprimeIndividuo2(p, ind); printf("\n\n Ordenado:\n"); ordenaIndividuo (ind); imprimeIndividuo2(p, ind);exit(1);/////////////
    preenchePosicoesVazias(p, ind);
    //embaralhaIndividuo2 (p, ind, nTrocas);
    //embaralhaIndividuo2(p, ind, 25);//////
    //printf("\n\n\nInd:\n");imprimeIndividuo2(p, ind); printf("\n Ordenado:\n"); ordenaIndividuo (ind); imprimeIndividuo2(p, ind);
    return ind;
}

void ordenaIndividuo(Individuo *ind) {
    int i, j;
    int chave;

    for (j = 1; j < ind->n; j++) {
        chave = ind->aula[j];
        i = j - 1;
        while (i >= 0 && ind->aula[i] > chave) {
            ind->aula[i + 1] = ind->aula[i];
            i--;
        }
        ind->aula[i + 1] = chave;
    }
}

void embaralhaIndividuo(Individuo *a, int nTrocas) {
    int i, pos1, pos2;
    for (i = 0; i < nTrocas; i++) {
        pos1 = numero_aleatorio(a->n);
        pos2 = numero_aleatorio(a->n);
        troca_par(a, pos1, pos2);
    }

}

void liberaIndividuo(Individuo *a) {
    free(a->aula);
    free(a);
}

void preenche_individuo_teste(Individuo *a, int tam) {
    int i;
    for (i = 0; i < tam; i++) {
        a->aula[i] = numero_aleatorio(MAXRAND);
    }
}

void imprimeIndividuo(Individuo *a) {
    int i;
    for (i = 0; i < a->n; i++) {
        printf("%d  ", a->aula[i]);
    }
    printf("\n");
}

/*imprime com espacos separando dias e salta linha a cada sala*/
void imprimeIndividuo2(Problema *p, Individuo *a) {
    int i, dia, sala, periodo;
    i = 0;
    printf("Fitness: %f\n", a->fitness);
    printf("Tam: %d\n", a->n);
    for (sala = 0; sala < p->nSalas; sala++) {
        for (dia = 0; dia < p->nDias; dia++) {
            for (periodo = 0; periodo < p->nPerDias; periodo++) {
                printf("%d ", a->aula[i]);
                i++;
            }
            printf("  ");
        }
        printf("\n");
    }
}

//verifica se valor eh uma aula

int ehAula(Problema *p, int valor) {
    return (valor <= (p->nAulas));
}

/*imprime disciplinas e nao as aulas, com espacos separando dias e salta linha a cada sala*/
void imprimeIndividuo3(Problema *p, Individuo *a) {
    int i, dia, sala, periodo;
    Disciplina *d;

    i = 0;
    printf("Fitness: %f\n", a->fitness);
    printf("Tam: %d\n", a->n);
    for (sala = 0; sala < p->nSalas; sala++) {
        for (dia = 0; dia < p->nDias; dia++) {
            for (periodo = 0; periodo < p->nPerDias; periodo++) {
                if (ehAula(p, a->aula[i])) {
                    d = acessaDisciplina(p, a->aula[i]);
                    //printf ("%s ", d->professor);
                    printf("%s ", d->nomeDisciplina);
                } else {
                    printf("_____ ");
                }
                i++;
            }
            printf("        ");
        }
        printf("\n");
    }
}

////consertar

void getDiaPeriodo(Problema *p, int pos, int *dia, int *periodo) {
    int sala, resto1;
    *periodo = pos % (p->nPerDias);
    resto1 = pos % (p->nDias * p->nPerDias);
    *dia = resto1 / (p->nPerDias);

}

int getPeriodo(Problema *p, int pos) {
    return pos % (p->nPerDias);
}

int getDia(Problema *p, int pos) {
    int resto1;

    resto1 = pos % (p->nDias * p->nPerDias);

    return resto1 / (p->nPerDias);

}

int getSalaFromPos(Problema *p, int pos) {
    return pos / (p->nDias * p->nPerDias);
}




//dada uma posicao base = pos, retorna a posicao que possui mesmo periodo, ou seja na mesma coluna, na sala = sala. Apenas para salas da propria posicao pos ou abaixo da mesma.

int posAulaSala(Problema *p, int pos, int sala) {
    return pos + (sala * (p->nDias * p->nPerDias));
}

int salaDiaHorarioParaPos(Problema *p, int sala, int dia, int horario) {
    return ((p->nDias * p->nPerDias) * sala)
            + (dia * (p->nPerDias))
            + horario;
}

Individuo *leIndividuo(Problema *p) {
    Individuo *a;
    int i;

    a = alocaIndividuo();
    criaIndividuo(a, p);
    inicializaIndividuo2(a);
    printf("digite os valores da tabela:\n");

    for (i = 0; i < p->dimensao; i++) {
        scanf("%d", &(a->aula[i]));
    }
    a->fitness = fitness(p, a);
    a->n = p->dimensao;
    return a;
}

/*imprime com espacos separando dias e salta linha a cada sala*/
void imprimePosicoesTabela(Problema *p, Individuo *a) {
    int i, dia, sala, periodo;
    i = 0;
    //printf("Fitness: %f\n", a->fitness);
    //printf("Tam: %d\n", a->n);
    for (sala = 0; sala < p->nSalas; sala++) {
        for (dia = 0; dia < p->nDias; dia++) {
            for (periodo = 0; periodo < p->nPerDias; periodo++) {
                printf("%d ", i);
                i++;
            }
            printf("  ");
        }
        printf("\n");
    }
}

int aulaRepetidaNaColuna(Problema *p, Individuo *a, int aula, int pos) {
    int i, a2;
    int ini = (getDiaFromPos(pos, p) * p->nPerDias) + getPeriodoFromPos(pos, p);
    int incremento = p->nPerDias * p->nDias;


    for (i = ini; i < a->n; i += incremento) {
        a2 = a->aula[i];
        if (ehAula(p, a2)) {
            //evita verificar aula com ela mesma. 
            if (aula != a2) {
                if (aulasMesmaDisciplina(p, aula, a2)) {
                    printf("aula repetida encontrada na coluna\n");
                    return 1;
                }
            }
        }
    }
    return 0;
}

void realocaAulaRepetida(Problema *p, Individuo *ind, int aula, int p1) {
    int p2, aux, nPosLivres, nTentativas;

    // MOVE EVENT
    p2 = rand() % p->dimensao; // posicao que irá apontar um horario vazio
    //printf("posicoes sorteadas\n");

    nPosLivres = (p->dimensao) - (p->nAulas);
    nTentativas = 0;
    while (nTentativas < nPosLivres) {
        while (ehAula(p, ind->aula[p2])) {
            p2++;
            if (p2 == p->dimensao) {// volta ao inicio do vetor 'aula'
                p2 = 0;
            }
        }
        nTentativas = nTentativas + 1;
        //se troca viola restrição desfaz e busca nova posição vazia sequencialmente
        if (aulaRepetidaNaColuna(p, ind, aula, p2)) {
            //busca outra posicao
            p2++;
        } else {
            //faz troca
            printf("Realocando aulas repetidas da disp %s. Aula %d na posicao %d e aula %d na posicao %d\n", getDisciplina(p, ind->aula[p1]), ind->aula[p1], p1, ind->aula[p2], p2);
            printf("\n");
            imprimeIndividuo3(p, ind);
            printf("\n\n");
            imprimeIndividuo2(p, ind);
            printf("\n");
            imprimePosicoesTabela(p, ind); ////////
            troca_par(ind, p1, p2);
            printf("Apos troca: disp %s. Aula %d na posicao %d e aula %d na posicao %d\n", getDisciplina(p, ind->aula[p1]), ind->aula[p1], p1, ind->aula[p2], p2);
            printf("\n");
            imprimeIndividuo3(p, ind);
            printf("\n\n");
            imprimeIndividuo2(p, ind);
            printf("\n\n\n"); ////////

            nTentativas = (p->dimensao); //para sair do loop
        }
    }

    if (nTentativas == nPosLivres) {
        printf("nao encontrou posicao livre disponivel !!!\n");
        exit(1);
    } ////////////////

}

void realocaAulasRepetidasAbaixo(Problema *p, Individuo *ind, int a1, int pos) {
    int i, a2, nColunas;

    nColunas = ((p->nPerDias) * (p->nDias)); //incremento para a posicao abaixo da posicao atual (mesma coluna)

    for (i = pos + nColunas; i < (p->dimensao); i = i + nColunas) {
        //printf ("eliminando aulas abaixo -------- pos: %d, pos2: %d\n", pos, i); imprimePosicoesTabela(p, ind); ///////////////
        a2 = ind->aula[i];

        //a1 eh verificado se eh aula antes de chamar esta funcao (eliminaAulasRepetidasAbaixo)
        if (ehAula(p, a2)) {
            if (aulasMesmaDisciplina(p, a1, a2)) {
                //printf ("aula repetida encontrada\n"); 
                realocaAulaRepetida(p, ind, a2, i);
            }
        }

    }

}

void realocaAulasMultiplas(Problema *p, Individuo *ind) {
    int i, posf, a1;

    posf = (p->dimensao) - ((p->nDias) * (p->nPerDias));
    //printf ("Realocando ...  dimensao: %d, posf: %d\n", p->dimensao, posf);///////////////
    for (i = 0; i < posf; i++) {
        a1 = ind->aula[i];

        if (ehAula(p, a1)) {
            realocaAulasRepetidasAbaixo(p, ind, a1, i);
        }
    }


}






