#include "individuo.h"
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

Individuo *alocaIndividuo(void) {
    Individuo *ind = (Individuo*) malloc(sizeof (Individuo));
    return ind;
}

void criaIndividuo(Individuo *a, Problema *p) {
    a->n = (p->nSalas) * (p->nDias) * (p->nPerDias);
    a->fitness = 0;
    a->aula = (int*) malloc(a->n * sizeof (int));
}

void inicializaIndividuo2(Individuo *a) {
    int i;
    int valor_nulo = (a->n) + 55;
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

/* varre vetor restricoes verificando se existe violacao e soma as violacoes*/
int horarioIndisponivel(Problema *p, Disciplina *disp, int pos) {
    int i, timeSlot;
    timeSlot = getTimeSlotFromPos(pos, p);
    for (i = 0; i < disp->nIndisponibilidades; i++) {
        if (disp->indisponibilidades[i] == timeSlot) {
            //printf ("horarioIndisponivel 2: sim; ");
            return 1;
        }
    }
    //printf ("horarioIndisponivel 2: nao; ");
    return 0;
}



//otimizar: juntar com violacoes hard 3
// verifica aulas no Mesmo Horario

float violacoesHard1(Problema *p, Individuo *a, int pos) {
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


//periodo aqui significa horario (ex: 14-15 horas)
//para uma posicao do vetor: checa se eh aula, obtem codigo da disciplina, varre vetor restricoes verificando se existe violacao e soma as violacoes

float violacoesHard4(Problema *p, Individuo *a, int pos) {
    int aula;
    Disciplina* pdisp;

    aula = a->aula[pos];
    if (ehAula(p, aula)) {
        pdisp = acessaDisciplina(p, aula);
        return (float) horarioIndisponivel(p, pdisp, pos);
    }
    return 0.0;
}

float somaViolacoesHardLocal(Problema *p, Individuo *a, int pos) {
    int i;
    float violacoes1, violacoes4, violacoes3;
    int ini = (getDiaFromPos(pos, p) * p->nPerDias) + getPeriodoFromPos(pos, p);
    int incremento = p->nPerDias * p->nDias;

    violacoes3 = violacoesHard3(p, a, ini);
    violacoes1 = violacoes4 = 0.0;
    for (i = ini; i < a->n; i += incremento) {
        violacoes1 += violacoesHard1(p, a, i);
        violacoes4 += violacoesHard4(p, a, i);
    }
    return (float) (violacoes1 + violacoes3 + violacoes4);
}

int slotDisponivel(Problema *p, Individuo *ind, Disciplina *d, int aula, int pos) {
    int i, timeSlot, valorOriginal;
    timeSlot = getTimeSlotFromPos(pos, p);

    valorOriginal = ind->aula[pos];
    //verifica se slot jah estah preenchido com uma aula
    if (ehAula(p, valorOriginal)) {
        return 0;
    }

    ind->aula[pos] = aula;
    //verifica se nao sera violada nenhuma restricao hard
    if (somaViolacoesHardLocal(p, ind, pos) > 0) {
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
    if (pos == posIni) printf("nenhum slot vazio encontrado !!!!!!!\n"); /////////////
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

void insereDisciplina(Problema *p, Individuo *ind, Disciplina *d) {
    int aula, pos;
    for (aula = d->aulaInicial; aula < (d->aulaInicial + d->nAulas); aula++) {
        pos = numero_aleatorio(ind->n);
        if (slotDisponivel(p, ind, d, aula, pos)) {
            ind->aula[pos] = aula;
        } else {
            pos = proximaPosicao(p, ind, d, aula, pos);
            ind->aula[pos] = aula;
        }
    }
}

void preenchePosicoesVazias(Problema *p, Individuo *ind) {
    int i, val;
    val = (ind->n) + 1;
    for (i = 0; i < ind->n; i++) {
        if (!ehAula(p, ind->aula[i])) {
            ind->aula[i] = val;
            val++;
        }
    }
}

/* realiza nTrocas (se possivel) sem violar restricoes hard*/
void embaralhaIndividuo2(Problema *p, Individuo *a, int nTrocas) {
    int i, pos1, pos2, nTentativas;
    printf("Embaralhando individuo\n"); ///////
    for (i = 0; i < nTrocas; i++) {
        pos1 = numero_aleatorio(a->n);
        pos2 = numero_aleatorio(a->n);
        for (nTentativas = 0; nTentativas < MAXTENTATIVAS; nTentativas++) {
            if (troca_par_restrita(p, a, pos1, pos2)) {
                nTentativas = MAXTENTATIVAS + 33;
            }
            //printf ("nTentativa: %d\n", nTentativas);///////
        }
    }

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
    preenchePosicoesVazias(p, ind);
    //embaralhaIndividuo2 (p, ind, nTrocas);
    //embaralhaIndividuo2(p, ind, 25);//////
    return ind;
}

Individuo *geraSolucaoInicial(Problema *p) {
    int posVazias[p->dimensao];
    int i;
    Individuo *ind = alocaIndividuo();

    criaIndividuo(ind, p);
    inicializaIndividuo2(ind);

    for (i = 0; i < p->nDisciplinas; i++) {
        ////continuar
    }
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



//dada uma posicao base = pos, retorna a posicao que possui mesmo periodo, ou seja na mesma coluna, na sala = sala. Apenas para salas da propria posicao pos ou abaixo da mesma.

int posAulaSala(Problema *p, int pos, int sala) {
    return pos + (sala * (p->nDias * p->nPerDias));
}

int salaDiaHorarioParaPos(Problema *p, int sala, int dia, int horario) {
    return ((p->nDias * p->nPerDias) * sala)
            + (dia * (p->nPerDias))
            + horario;
}

