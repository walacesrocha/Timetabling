#include <stdio.h>
#include <stdlib.h>
#include "grasp.h"
#include "sa.h"
#include "problema.h"
#include "util.h"
#include "auxiliar.h"
#include "buscalocal.h"
#include "tabu.h"
#include "gd.h"
#include "gerador.h"
#include "io.h"

/**
 * Compara duas aulas em relação a dificuldade. 
 * O primeiro critério é indisponibilidade de horarios, seja por restrição inicial 
 * ou aulas do mesmo currículo/professor. Se empatar nesse critério verifica-se qual
 * disciplina está presente em mais currículo.
 * @param p
 * @param auxGrasp
 * @param a1
 * @param a2
 * @return 
 */
int comparaAulas(Problema *p, AuxGrasp *auxGrasp, int a1, int a2) {

    Disciplina *d1, *d2;
    int horariosDisp1, horariosDisp2;

    d1 = acessaDisciplina(p, a1);
    d2 = acessaDisciplina(p, a2);

    if (d1 == 0 || d2 == 0) {
        printf("ERRO: NULL, %d %d %d\n", a1, a2, p->nAulas);
        exit(1);
    }

    if (strcmp(d1->nomeDisciplina, d2->nomeDisciplina) == 0) {
        //printf("Disciplinas iguais: %d %d\n", a1, a2);
        return 0;
    }

    // calcula os horarios disponiveis para cada aula
    horariosDisp1 = getTotalHorariosViaveis(p, auxGrasp, a1);
    horariosDisp2 = getTotalHorariosViaveis(p, auxGrasp, a2);

    if (horariosDisp1 > horariosDisp2) {
        return -1;
    } else if (horariosDisp1 < horariosDisp2) {
        return 1;
    } else {
        if (d1->nCurriculos < d2->nCurriculos) {
            return -1;
        } else if (d1->nCurriculos > d2->nCurriculos) {
            return 1;
        } else {
            return 0;
        }
    }
}

void ordenaDisiciplinasPorDificuldade(Problema *p, AuxGrasp *auxGrasp) {
    //printf("ord GRASP\n");


    int i, j, chave;

    for (j = 1; j < auxGrasp->nCandidatos; j++) {
        chave = auxGrasp->candidatos[j];
        i = j - 1;
        while (i >= 0 && comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], chave) > 0) {
            auxGrasp->candidatos[i + 1] = auxGrasp->candidatos[i];
            i--;
        }
        auxGrasp->candidatos[i + 1] = chave;
    }

}

Individuo *geraTimetableVazio(Problema *p) {
    int i;
    Individuo *ind = alocaIndividuo();

    criaIndividuo(ind, p);

    for (i = 0; i < ind->n; i++) {
        ind->aula[i] = p->nAulas + 1; // marca todas as aulas como vazias
    }

    return ind;
}

void retiraCandidato(AuxGrasp *auxGrasp, int aula) {
    int i = 0;

    while (auxGrasp->candidatos[i] != aula) {
        i++;
    }
    //printf("%d/%d\n", i, auxGrasp->ind->n);

    for (; i < auxGrasp->nCandidatos - 1; i++) {
        auxGrasp->candidatos[i] = auxGrasp->candidatos[i + 1];
    }

    auxGrasp->nCandidatos--;

}

int estaEmConflito(Individuo *ind, int aula) {
    int i;

    for (i = 0; i < ind->nConflitos; i++) {
        if (ind->posConflitos[i] == aula) {
            return 1;
        }
    }

    return 0;
}

void resetAuxGrasp(Problema *p, AuxGrasp *auxGrasp, Individuo *indBase) {
    int i, j, k;


    auxGrasp->nCandidatos = p->nAulas;

    for (i = 0; i < p->nAulas; i++) {
        auxGrasp->explosao[i] = 0;
    }

    i = 0;
    for (j = 0; j < p->nDisciplinas; j++) {
        int nDisc = p->disciplinas[j].aulaInicial;
        for (k = 0; k < p->disciplinas[j].nAulas; k++) {
            auxGrasp->candidatos[i] = nDisc;
            nDisc++;
            i++;
        }
    }

    // cria a tabela-horario vazia 
    auxGrasp->ind = geraTimetableVazio(p);

    // cria vetor auxiliar para guardar as possibilidades de alocacao 
    for (i = 0; i < p->dimensao; i++) {
        auxGrasp->vetorPossibilidades[i]->id = i + 1;
    }
    auxGrasp->nrPossibilidades = 0;



    if (indBase != NULL) {
        somaViolacoesSoft2(p, indBase, 1);
        printf("Aula em conflitos: %d\n", indBase->nConflitos);
        for (i = 0; i < indBase->nConflitos; i++) {
            printf("%d ", indBase->posConflitos[i]);
        }
        printf("\n");
        int k;
        //printf("Aproveitando solucao anterior\n");
        for (k = 0; k < p->dimensao; k++) {

            if (ehAula(p, indBase->aula[k])) {
                if (((float) rand()) / RAND_MAX < p->pAproveitamento) {
                    auxGrasp->ind->aula[k] = indBase->aula[k];

                    //printf("Disc: %s (%d)\n", acessaDisciplina(p, indBase->aula[k]), indBase->aula[k]);

                    retiraCandidato(auxGrasp, indBase->aula[k]);

                    /*printf("Candidatos: ");
                    for (i = 0; i < auxGrasp->nCandidatos; i++) {
                        printf("%d ", auxGrasp->candidatos[i]);
                    }
                    printf("\n");

                    printf("Ind Parcial: ");
                    for (i = 0; i < p->dimensao; i++) {
                        printf("%d ", auxGrasp->ind->aula[i]);
                    }
                    printf("\n");*/
                }
            }
        }


    }

    //return auxGrasp;
}

AuxGrasp *geraAuxGrasp(Problema *p) {
    int i, j, k;
    AuxGrasp *auxGrasp;

    auxGrasp = (AuxGrasp*) malloc(sizeof (AuxGrasp));

    auxGrasp->candidatos = (int*) malloc(p->nAulas * sizeof (int));
    auxGrasp->explosao = (int*) malloc(p->nAulas * sizeof (int));
    auxGrasp->nCandidatos = p->nAulas;

    for (i = 0; i < p->nAulas; i++) {
        auxGrasp->explosao[i] = 0;
    }

    i = 0;
    for (j = 0; j < p->nDisciplinas; j++) {
        int nDisc = p->disciplinas[j].aulaInicial;
        for (k = 0; k < p->disciplinas[j].nAulas; k++) {
            auxGrasp->candidatos[i] = nDisc;
            nDisc++;
            i++;
        }
    }

    // cria a tabela-horario vazia 
    auxGrasp->ind = geraTimetableVazio(p);

    // cria vetor auxiliar para guardar as possibilidades de alocacao 
    auxGrasp->vetorPossibilidades = (AlocacaoAula**) malloc(p->dimensao * sizeof (AlocacaoAula*));
    for (i = 0; i < p->dimensao; i++) {
        auxGrasp->vetorPossibilidades[i] = (AlocacaoAula*) malloc(sizeof (AlocacaoAula));
        auxGrasp->vetorPossibilidades[i]->id = i + 1;
    }
    auxGrasp->nrPossibilidades = 0;

    // alocacao do pool

    auxGrasp->tPool = 20;
    auxGrasp->nElites = 0; // pool vazio

    auxGrasp->poolElite = (Individuo**) malloc(auxGrasp->tPool * sizeof (Individuo*));
    for (i = 0; i < auxGrasp->tPool; i++) {
        auxGrasp->poolElite[i] = alocaIndividuo();
        criaIndividuo(auxGrasp->poolElite[i], p);
    }

    return auxGrasp;
}

/**
 * Conta quantos horarios sao viaveis
 * @param p
 * @param horariosViaveis
 * @return 
 */
int contaHorariosViaveis(Problema *p, int *horariosViaveis) {
    int i, total = 0;
    int qtHorarios = p->nDias * p->nPerDias;

    for (i = 0; i < qtHorarios; i++) {
        if (horariosViaveis[i] == 1) {
            total++;
        }
    }
    return total;

}

int *getVetorTodosHorarios(Problema *p) {
    int *horariosViaveis;
    int i;
    int qtHorarios = p->nDias * p->nPerDias;

    horariosViaveis = (int*) malloc(qtHorarios * sizeof (int));
    if (horariosViaveis == 0) {
        printf("ERRO\n");
        exit(1);
    }
    // marca todos horarios inicialmente como viaveis
    for (i = 0; i < qtHorarios; i++) {
        horariosViaveis[i] = 1;
    }

    return horariosViaveis;

}

int *getHorariosViaveis(Problema*p, AuxGrasp*auxGrasp, int aula) {
    int *horariosViaveis;
    int i, j, qtHorarios;
    Disciplina *disc;
    //printf("começou\n");
    qtHorarios = p->nDias * p->nPerDias;

    //printf("a\n");
    horariosViaveis = (int*) malloc(qtHorarios * sizeof (int));
    if (horariosViaveis == 0) {
        printf("ERRO\n");
        exit(1);
    }
    //printf("b\n");
    // marca todos horarios inicialmente como viaveis
    for (i = 0; i < qtHorarios; i++) {
        horariosViaveis[i] = 1;
    }
    //printf("c\n");
    /*for (i = 0; i < qtHorarios; i++) {
        printf("%d ", horariosViaveis[i]);
    }
    printf("\n");*/
    // tira os horarios restritos
    disc = acessaDisciplina(p, aula);
    for (i = 0; i < disc->nIndisponibilidades; i++) {
        horariosViaveis[disc->indisponibilidades[i]] = 0;
    }
    //printf("d\n");
    /*/for (i = 0; i < qtHorarios; i++) {
        printf("%d ", horariosViaveis[i]);
    }
    printf("\n");*/

    // verifica os conflitos curriculo/professor
    for (i = 0; i < qtHorarios; i++) {
        int temHorarioVago = 0;
        int temConflito = 0;

        // percorre todas as salas do horario
        for (j = 0; j < p->nSalas; j++) {

            int pos = i + qtHorarios*j;
            //printf("checking ... %d\n", pos);

            if (ehAula(p, auxGrasp->ind->aula[pos])) {// tem uma disciplina na sala
                Disciplina *discPos = acessaDisciplina(p, auxGrasp->ind->aula[pos]);

                // conflito de professor
                if (strcmp(disc->professor, discPos->professor) == 0) {
                    temConflito = 1;
                    break;

                    // conflito de curriculo
                } else if (aulasMesmoCurriculo(p, aula, auxGrasp->ind->aula[pos])) {
                    temConflito = 1;
                    break;
                }
            } else {
                temHorarioVago = 1; // tem pelo menos uma sala vaga no horario
            }
        }

        // se nao tem sala vaga ou algum conflito marca como "inviavel"
        if (!temHorarioVago || temConflito) {
            horariosViaveis[i] = 0;
        }
    }
    //printf("e\n");
    //printf("terminou\n");

    return horariosViaveis;
}

int getTotalHorariosViaveis(Problema *p, AuxGrasp* auxGrasp, int aula) {
    int cont;
    int *horariosViaveis = getHorariosViaveis(p, auxGrasp, aula);

    cont = contaHorariosViaveis(p, horariosViaveis);

    free(horariosViaveis);

    return cont;
}

Disciplina* getDisciplinaPorNome(Problema *p, char * nomeDisciplina) {
    Disciplina *disc;
    int i, j;


    for (j = 0; j < p->nDisciplinas; j++) {

        if (strcmp((p->disciplinas + j)->nomeDisciplina, nomeDisciplina) == 0) {
            return p->disciplinas + j;
        }
    }

}

void avaliaCustoAlocacao(Problema *p, AuxGrasp *auxGrasp, int aula, int nrP) {

    AlocacaoAula *alocacao;
    Disciplina *disc;
    Sala *sala;
    int *diasOcupados, totalDiasOcupados;
    int *salasOcupadas, totalSalasOcupadas;
    int i, c, qtHorarios, pos;
    int aulasIsoladas = 0;

    //printf("avaliando\n");

    qtHorarios = p->nDias * p->nPerDias;

    // disciplina que esta sendo alocada
    disc = acessaDisciplina(p, aula);
    //printf("Disc: %s (%d)\n", disc->nomeDisciplina, disc->nAlunos);

    // acessa a possibilidade
    alocacao = auxGrasp->vetorPossibilidades[nrP];
    alocacao->custo = 0;

    sala = p->salas + alocacao->sala;

    // ROOM_CAPACITY
    if (disc->nAlunos > sala->capacidade) {
        // custo = quantidade de alunos excedentes
        alocacao->custo += (disc->nAlunos - sala->capacidade);
    }

    // vetor auxiliar para guardar os dias ocupados pela disciplina
    diasOcupados = (int*) malloc(p->nDias * sizeof (int));
    for (i = 0; i < p->nDias; i++) {
        diasOcupados[i] = 0;
    }

    // vetor auxiliar para guardar as salas ocupadas pela disciplina
    salasOcupadas = (int*) malloc(p->nSalas * sizeof (int));
    for (i = 0; i < p->nSalas; i++) {
        salasOcupadas[i] = 0;
    }


    for (i = 0; i < p->dimensao; i++) {
        if (ehAula(p, auxGrasp->ind->aula[i])) {
            if (aulasMesmaDisciplina3(p, aula, auxGrasp->ind->aula[i])) {
                // marca o dia como tendo aula
                diasOcupados[getDia(p, i)] = 1;

                // marca a sala como ocupada
                salasOcupadas[getRoomFromPos(i, p)] = 1;
            }
        }
    }

    // marca o dia em que sera alocado
    diasOcupados[getDia(p, alocacao->horario + qtHorarios * alocacao->sala)] = 1;

    // marca a sala em que sera ocupada
    salasOcupadas[alocacao->sala] = 1;

    // conta o total de dias ocupados
    totalDiasOcupados = 0;
    for (i = 0; i < p->nDias; i++) {
        if (diasOcupados[i]) {
            totalDiasOcupados++;
        }
    }

    // conta o total de salas ocupadas
    totalSalasOcupadas = 0;
    for (i = 0; i < p->nSalas; i++) {
        if (salasOcupadas[i]) {
            totalSalasOcupadas++;
        }
    }

    // MIN_WORKING_DAYS
    if (totalDiasOcupados < disc->minDiasAula) {
        // penalizacao MIN_WORKING_DAYS
        alocacao->custo += (disc->minDiasAula - totalDiasOcupados)* 5; // peso = 5
    }

    // ROOM_STABILITY
    alocacao->custo += (totalSalasOcupadas - 1);


    int periodo = getPeriodo(p, alocacao->horario);
    int temAdjacente = 0;

    for (c = 0; c < disc->nCurriculos; c++) {


        char *nomeDiscDoCurriculo = disc->curriculos[c]->disciplinas[0];
        Disciplina *discDoCurr = getDisciplinaPorNome(p, nomeDiscDoCurriculo);
        int aulaDoCurriculo = discDoCurr->aulaInicial;

        if (periodo > 0) {// caso nao seja o primeiro periodo...
            // ... verifica se tem aula do mesmo curriculo no periodo anterior

            for (i = 0; i < p->nSalas; i++) {
                pos = alocacao->horario - 1 + qtHorarios * i;

                if (ehAula(p, auxGrasp->ind->aula[pos])) {
                    if (aulasMesmoCurriculo(p, aulaDoCurriculo, auxGrasp->ind->aula[pos])) {
                        temAdjacente = 1;
                        break;
                    }
                }
            }
        }

        if (!temAdjacente && periodo < p->nPerDias - 1) {// caso nao tenha aula antes, 
            // e nao e o ultimo periodo do dia
            // ... verifica se tem aula do mesmo curriculo no periodo posterior

            for (i = 0; i < p->nSalas; i++) {
                pos = alocacao->horario + 1 + qtHorarios * i;

                if (ehAula(p, auxGrasp->ind->aula[pos])) {
                    if (aulasMesmoCurriculo(p, aulaDoCurriculo, auxGrasp->ind->aula[pos])) {
                        temAdjacente = 1;
                        break;
                    }
                }
            }
        }

        // ISOLATED_LECTURE
        //alocacao->custo = 0;
        if (!temAdjacente) {
            aulasIsoladas += 1;
        }
    }

    alocacao->custo += 2 * aulasIsoladas; // penalidade peso 2

    //printf("MIN=%d, ocupados: %d\n", disc->minDiasAula, totalDiasOcupados);


    free(diasOcupados);
    free(salasOcupadas);
}

int minimo(int n, int m) {
    return (n < m) ? n : m;
}

/**
 * Ordena as possibilidades de alocacao para montar a LRC
 * @param auxGrasp
 */
void ordenaPossibilidades(AuxGrasp *auxGrasp) {
    int i, j;
    AlocacaoAula *chave;

    for (j = 1; j < auxGrasp->nrPossibilidades; j++) {
        chave = auxGrasp->vetorPossibilidades[j];
        i = j - 1;
        while (i >= 0 && auxGrasp->vetorPossibilidades[i]->custo > chave->custo) {
            //printf("custo=%d,%d\n", auxGrasp->vetorPossibilidades[i]->custo, chave->custo);
            auxGrasp->vetorPossibilidades[i + 1] = auxGrasp->vetorPossibilidades[i];
            i--;
        }
        auxGrasp->vetorPossibilidades[i + 1] = chave;
    }

}

/*int limiteExplosoes(Problema *p, AuxGrasp *auxGrasp){
    
    int i;
    
    for (i=0;i<auxGrasp->)
    
}*/

/**
 * Expolode uma aula para abrir espaço no timetable
 * @param p
 * @param auxGrasp
 * @param aula
 */
void explodeTimetable(Problema *p, AuxGrasp *auxGrasp, int aula) {
    Disciplina *disc;
    int *nrIndisp;
    int qtHorarios, i, j, pos;
    int minConflitos, posMin;

    printf("PM0 = %d\n", posMin);
    printf("C1: ");
    for (i = 0; i < auxGrasp->nCandidatos; i++) {
        printf("% d", auxGrasp->candidatos[i]);
    }
    printf("\n");

    //printf("a\n");
    qtHorarios = p->nDias * p->nPerDias;

    // numero de indisponibilidades em cada horario
    nrIndisp = (int*) malloc(qtHorarios * sizeof (int));
    //printf("b\n");
    // marca todos horarios com zero indisponibilidades
    for (i = 0; i < qtHorarios; i++) {
        nrIndisp[i] = 0;

        for (j = 0; j < p->nSalas; j++) {
            pos = i + qtHorarios*j;

            if (!ehAula(p, auxGrasp->ind->aula[pos])) {
                // horario vazio na coluna
                continue;
            }

            if (aulasConflitantes(p, aula, auxGrasp->ind->aula[pos])) {
                nrIndisp[i]++;
            }

            if (auxGrasp->explosao[auxGrasp->ind->aula[pos] - 1] > 2500) {
                nrIndisp[i] = p->nAulas + 2;
            }
        }
    }
    //printf("c\n");
    disc = acessaDisciplina(p, aula);
    for (i = 0; i < disc->nIndisponibilidades; i++) {
        nrIndisp[disc->indisponibilidades[i]] = p->nSalas + 2;
    }
    //printf("d\n");

    minConflitos = p->nSalas + 1;
    //posMin = -1;

    printf("QC: ");
    for (i = 0; i < qtHorarios; i++) {
        printf("%d ", nrIndisp[i]);
    }
    printf("\n");

    //printf("Conflitos: ");
    for (i = 0; i < qtHorarios; i++) {
        if (nrIndisp[i] > 0 && nrIndisp[i] < minConflitos) {
            posMin = i;
            minConflitos = nrIndisp[i];
            printf("passei aqui\n");
        }
        //printf("%d ", nrIndisp[i]);
    }

    printf("PosMin=%d\n", posMin);

    if (posMin == -1) {
        printf("Nao achei posiccao de explosao\n*\n*\n*\n");
    }
    //printf("\n");
    //printf("e\n");
    /*for (i = 0; i < auxGrasp->nCandidatos; i++) {
        //printf("%d ", auxGrasp->candidatos[i]);
        //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
    }
    //printf("\n");*/
    int explodiuPorConflito = 0;
    for (j = 0; j < p->nSalas; j++) {
        int pos = posMin + qtHorarios*j;

        if (!ehAula(p, auxGrasp->ind->aula[pos])) {
            // horario vazio na coluna
            continue;
        }

        if (aulasConflitantes(p, aula, auxGrasp->ind->aula[pos])) {
            int aulaConflitante = auxGrasp->ind->aula[pos];

            if (aulaConflitante == 0 || aulaConflitante > p->nAulas) {
                printf("ERRO AQUI\n");
                exit(1);
            }

            printf("explodiu %d\n", aulaConflitante);

            auxGrasp->explosao[aulaConflitante - 1]++; // conta a explosao
            auxGrasp->ind->aula[pos] = p->nAulas + 1; // coloca horario vago na posicao
            auxGrasp->candidatos[auxGrasp->nCandidatos] = aulaConflitante; // aula volta a ser candidato
            auxGrasp->nCandidatos++; // conflitante voltou a ser um candidato

            //printf("Explosoes[%d]: %d\n", aulaConflitante, auxGrasp->explosao[aulaConflitante - 1]);

            explodiuPorConflito = 1;

        } else {
            printf("nao conflitante\n");
        }
    }

    if (!explodiuPorConflito) {
        for (j = 0; j < p->nSalas; j++) {
            int pos, aulaConflitante;
            do {
                j = rand() % p->nSalas;
                pos = posMin + qtHorarios*j;
                printf("j=%d\n", j);
            } while (!ehAula(p, auxGrasp->ind->aula[pos]));

            aulaConflitante = auxGrasp->ind->aula[pos];

            printf("explodiu %d\n", aulaConflitante);

            auxGrasp->explosao[aulaConflitante - 1]++; // conta a explosao
            auxGrasp->ind->aula[pos] = p->nAulas + 1; // coloca horario vago na posicao
            auxGrasp->candidatos[auxGrasp->nCandidatos] = aulaConflitante; // aula volta a ser candidato
            auxGrasp->nCandidatos++; // conflitante voltou a ser um candidato

            //printf("Explosoes[%d]: %d\n", aulaConflitante, auxGrasp->explosao[aulaConflitante - 1]);

            explodiuPorConflito = 1;

            break;


        }

    }
    /*for (i = 0; i < auxGrasp->nCandidatos; i++) {
        //printf("%d ", auxGrasp->candidatos[i]);
        //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
    }
    //printf("\n");**/
    //printf("f\n");

    printf("C2: ");
    for (i = 0; i < auxGrasp->nCandidatos; i++) {
        printf("% d", auxGrasp->candidatos[i]);
    }
    printf("\n");
}

/*
 * Verifica se um horario (dia,hora) é inviavel para a disciplina
 */
int horarioInviavel(Problema *p, Disciplina *disc, int posMin) {
    int i;

    for (i = 0; i < disc->nIndisponibilidades; i++) {
        if (disc->indisponibilidades[i] == posMin) {
            return 1;
        }
    }

    return 0;
}

void explodeTimetable2(Problema *p, AuxGrasp *auxGrasp, int aula) {
    Disciplina *disc;
    int *nrIndisp;
    int qtHorarios, i, j, pos;
    int minConflitos, posMin;



    //printf("a\n");
    qtHorarios = p->nDias * p->nPerDias;

    posMin = rand() % qtHorarios;

    disc = acessaDisciplina(p, aula);
    while (horarioInviavel(p, disc, posMin)) {

        posMin++;

        if (posMin == qtHorarios) {
            // volta ao primeiro horario
            posMin = 0;
        }
    }

    int explodiuPorConflito = 0;
    int nConflitantes = 0;
    int nAulas = p->nSalas;
    for (j = 0; j < p->nSalas; j++) {
        pos = posMin + qtHorarios*j;

        if (ehAula(p, auxGrasp->ind->aula[pos])) {

            if (aulasConflitantes(p, aula, auxGrasp->ind->aula[pos])) {
                int aulaConflitante = auxGrasp->ind->aula[pos];

                //printf("explodiu %d\n", aulaConflitante);

                auxGrasp->explosao[aulaConflitante - 1]++; // conta a explosao
                auxGrasp->ind->aula[pos] = p->nAulas + 1; // coloca horario vago na posicao
                auxGrasp->candidatos[auxGrasp->nCandidatos] = aulaConflitante; // aula volta a ser candidato
                auxGrasp->nCandidatos++; // conflitante voltou a ser um candidato

                //printf("Explosoes[%d]: %d\n", aulaConflitante, auxGrasp->explosao[aulaConflitante - 1]);

                explodiuPorConflito++;

            } else {
                nConflitantes++;
                //printf("nao conflitante\n");
            }
        } else {
            nAulas--;
        }
    }

    if (!explodiuPorConflito) {
        // ninguem saiu ainda
        // escolhe uma aleatorio para sair
        j = rand() % p->nSalas;
        pos = posMin + qtHorarios*j;
        while (!ehAula(p, auxGrasp->ind->aula[pos])) {
            j++;
            if (j == p->nSalas) {
                j = 0;
            }
            pos = posMin + qtHorarios*j;
        }

        int aulaConflitante = auxGrasp->ind->aula[pos];

        //printf("explodiu %d\n", aulaConflitante);

        auxGrasp->explosao[aulaConflitante - 1]++; // conta a explosao
        auxGrasp->ind->aula[pos] = p->nAulas + 1; // coloca horario vago na posicao
        auxGrasp->candidatos[auxGrasp->nCandidatos] = aulaConflitante; // aula volta a ser candidato
        auxGrasp->nCandidatos++; // conflitante voltou a ser um candidato

        //printf("Explosoes[%d]: %d\n", aulaConflitante, auxGrasp->explosao[aulaConflitante - 1]);
    } else {
        //printf("Explosoes: %d\n", explodiuPorConflito);
        if (explodiuPorConflito > 1) {
            //printf("Problem\n");
            //exit(1);
        }
    }


}

void alocaAula(Problema *p, AuxGrasp* auxGrasp, int aula, int permiteInviabilidade) {
    int *horariosViaveis;
    int i, j, nrP, pos, qtHorarios; // quantidade de horarios: dias x periodos
    AlocacaoAula *alocacao;

    //printf("1\n");

    qtHorarios = p->nDias * p->nPerDias;
    //printf("1.1\n");

    if (permiteInviabilidade) {
        horariosViaveis = getVetorTodosHorarios(p);
    } else {
        horariosViaveis = getHorariosViaveis(p, auxGrasp, aula);
    }
    //printf("2\n");
    //printf("HV: %d\n", contaHorariosViaveis(p, horariosViaveis));

    if (contaHorariosViaveis(p, horariosViaveis) == 0) {
        int aux;
        int n1, n2, step;
        //printf("Quero alocar %d\n", aula);
        //printf("Candidatos[%d]: ", auxGrasp->nCandidatos);
        for (i = 0; i < auxGrasp->nCandidatos; i++) {
            //printf("%d ", auxGrasp->candidatos[i]);
            //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
        }
        //printf("\n");

        n1 = auxGrasp->nCandidatos;
        explodeTimetable2(p, auxGrasp, aula);
        n2 = auxGrasp->nCandidatos;

        step = n2 - n1 - 1;

        //printf("Explosoes contadas: %d\n", n2 - n1);


        aux = auxGrasp->candidatos[auxGrasp->nCandidatos - 1];
        auxGrasp->candidatos[auxGrasp->nCandidatos - 1] = auxGrasp->candidatos[auxGrasp->nCandidatos - 2 - step];
        auxGrasp->candidatos[auxGrasp->nCandidatos - 2 - step] = aux;

        //printf("Candidatos[%d]: ", auxGrasp->nCandidatos);
        for (i = 0; i < auxGrasp->nCandidatos; i++) {
            //printf("%d ", auxGrasp->candidatos[i]);
            //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
        }
        //printf("\n");

        if (n2 - n1 > 1) {
            //exit(1);
        }

        /*printf("T=%d ", p->nAulas);
        for (i = 0; i < p->nAulas; i++) {
            printf("%d, ", auxGrasp->explosao[i]);
        }
        printf("\n");*/
        //printf("\n");
        //exit(1);


        //printf("Explodiu...\n");
        return alocaAula(p, auxGrasp, aula, permiteInviabilidade);


        // recalcula para encontrar o horario viavel
        //horariosViaveis = getHorariosViaveis(p, auxGrasp, aula);

        //printf("XXX -> HV: %d\n", contaHorariosViaveis(p, horariosViaveis));
    }
    //printf("3\n");

    auxGrasp->nrPossibilidades = 0;

    // cria "AlocacaoAula" marcando (horario,sala) que estao disponiveis
    for (i = 0; i < qtHorarios; i++) {

        if (horariosViaveis[i]) {
            for (j = 0; j < p->nSalas; j++) {
                pos = i + qtHorarios*j;

                if (!ehAula(p, auxGrasp->ind->aula[pos])) {
                    // posicao nao está ocupada
                    alocacao = auxGrasp->vetorPossibilidades[auxGrasp->nrPossibilidades];

                    alocacao->horario = i;
                    alocacao->sala = j;
                    alocacao->nrAula = aula;

                    auxGrasp->nrPossibilidades++;
                }

            }
        }
    }
    //printf("4\n");
    // avalia todas as possibilidades
    for (nrP = 0; nrP < auxGrasp->nrPossibilidades; nrP++) {
        avaliaCustoAlocacao(p, auxGrasp, aula, nrP);
        // TODO: falta a restricao ISOLATED_LECTURES
    }
    //printf("5\n");

    /*for (nrP = 0; nrP < auxGrasp->nrPossibilidades; nrP++) {
        alocacao = auxGrasp->vetorPossibilidades[nrP];
        printf("[%d] H=%d, S=%d, C=%d\n", alocacao->id, alocacao->horario, alocacao->sala, alocacao->custo);
    }*/

    // ordena as possibilidades
    ordenaPossibilidades(auxGrasp);
    //printf("6\n");
    for (nrP = 0; nrP < auxGrasp->nrPossibilidades; nrP++) {
        //printf("%d ", auxGrasp->vetorPossibilidades[nrP]->custo);
    }
    //printf("\n");

    int cMin = auxGrasp->vetorPossibilidades[0]->custo;
    int cMax = auxGrasp->vetorPossibilidades[auxGrasp->nrPossibilidades - 1]->custo;
    float maxC = cMin + p->threshold * (cMax - cMin);
    //printf("7\n");
    //printf("%d <-> %d\n", cMin, cMax);
    //printf("Escolha: %d a %f\n", cMin, maxC);

    //printf("contagem\n");
    //printf("np=%d\n", auxGrasp->nrPossibilidades);
    //printf("min=%d, max=%d\n", cMin, cMax);
    i = 0;
    while (i < auxGrasp->nrPossibilidades && auxGrasp->vetorPossibilidades[i]->custo <= maxC) {
        i++;
    }
    //printf("8\n");
    //printf("calculado: %d/%d\n", i,auxGrasp->nrPossibilidades);

    //printf("NP=%d, I=%d\n", auxGrasp->nrPossibilidades, i);

    // escohe uma possibilidade e aloca a aula
    //int escolha = rand() % (minimo(tLRC, auxGrasp->nrPossibilidades));
    int escolha = rand() % i;
    //printf("Escolha: %d\n", escolha);

    alocacao = auxGrasp->vetorPossibilidades[escolha];
    //printf("9\n");
    // posicao a inserir
    pos = alocacao->horario + qtHorarios * alocacao->sala;

    // insere no timetabling
    auxGrasp->ind->aula[pos] = aula;
    auxGrasp->nCandidatos--;
    //printf("10\n");

    //printf("Nr.Possibilidades: %d\n", auxGrasp->nrPossibilidades);
    /*for (nrP = 0; nrP < auxGrasp->nrPossibilidades; nrP++) {
        alocacao = auxGrasp->vetorPossibilidades[nrP];
        printf("[%d] H=%d, S=%d, C=%d\n", alocacao->id, alocacao->horario, alocacao->sala, alocacao->custo);
    }
    printf("\n");*/

    free(horariosViaveis);
}

Individuo *buscaLocalGraspProfundidade(Problema*p, Individuo *indInicial) {
    Individuo *solucaoAtual, *aDesalocar;
    Individuo *vizinho;
    float foAtual;
    float deltaF;

    foAtual = funcaoObjetivo(p, indInicial, 10000);
    printf("Fo atual: %f\n",foAtual);

    solucaoAtual = indInicial;
    int iteracoes = 0;
    float fo;
    do {
        vizinho = kempeSwap(p, solucaoAtual);

        fo = funcaoObjetivo(p, vizinho,p->pesoHard);
        //fo = somaViolacoesSoft(p, vizinho);
        deltaF = fo - foAtual;

        //printf("Df=%f\n", deltaF);

        aDesalocar = 0;
        if (deltaF <= 0) {// função objetivo decresceu
            foAtual = fo;
            printf("Melhorou... %f [%d]\n", foAtual, iteracoes);
            aDesalocar = solucaoAtual;
            solucaoAtual = vizinho;
            //melhorInd = solucaoAtual;
            if (deltaF < 0) {
                iteracoes = 0; // continua buscando
            }
        } else {
            aDesalocar = vizinho;
        }

        //printf("ADesalocar: %p %p %p\n", aDesalocar, solucaoAtual, vizinho);
        liberaIndividuo(aDesalocar);

        iteracoes++;

        if (iteracoes % 100 == 0) {
            //printf("[%d]\n", iteracoes);
        }

        //printf("Iter: %d / FO: %f\n", iteracoes, foAtual);
    } while (iteracoes < p->nIterSemMelhoras);

    //printf("T=%f, Pioras=%d, FO=%f (%f, %f)\n", t0, nPioras, foAtual,
    //somaViolacoesHard(p, solucaoAtual), somaViolacoesSoft(p, solucaoAtual));


    return solucaoAtual;
}

void perturbaSolucao(Problema *p, Individuo *ind) {
    int i, k;
    int p1, p2;


    printf("FO: %f\n", funcaoObjetivo(p, ind, 10000));
    k = 0;
    while (k < 5) {
        p1 = rand() % p->dimensao;
        p2 = rand() % p->dimensao;

        troca_par(ind, p1, p2);

        if (somaViolacoesHardTroca(p, ind, p1, p2) > 0) {
            troca_par(ind, p1, p2);
            continue;
        }

        printf("FO: %f\n", funcaoObjetivo(p, ind, 10000));
        k++;
    }

    printf("FO: %f\n", funcaoObjetivo(p, ind, 10000));
}

void coletaEstatisticas(Problema *p, Individuo *ind) {
    int vHard, semEfeito, comMelhora, comPiora;
    int i, p1, p2, total;
    float f0, f1;
    Neighbour *neighbour;

    vHard = semEfeito = comMelhora = comPiora = 0;
    total = 1000;

    for (i = 0; i < 1000; i++) {
        if (((float) rand()) / RAND_MAX < 0.5) {
            neighbour = geraMove(p, ind);
        } else {
            neighbour = geraSwap(p, ind);
        }

        if (neighbour->deltaHard > 0) {
            vHard++;
        } else if (neighbour->deltaSoft > 0) {
            comPiora++;
        } else if (neighbour->deltaSoft < 0) {
            comMelhora++;
        } else {
            semEfeito++;
        }

    }

    printf("%d, %d, %d, %d\n", vHard, semEfeito, comPiora, comMelhora);
    /*printf("=: %f\n", 100*semEfeito/total);
    printf(">: %f\n", 100*comPiora/total);
    printf("<: %f\n", 100*comMelhora/total);**/

}

Individuo *buscaLocalGraspHibrida(Problema*p, Individuo *indInicial) {
    Neighbour **vizinhos;
    float foAtual;
    float deltaF;
    int i, nVizinhos;
    int melhorVizinho;
    long totalElementos = 0;
    float pViz;
    int count = 0;

    nVizinhos = p->k;

    vizinhos = (Neighbour**) malloc(nVizinhos * sizeof (Neighbour*));


    foAtual = funcaoObjetivo2(p, indInicial, p->pesoHard);
    //printf("FO atual: %f\n", foAtual);

    int iteracoesSemMelhora = 0;

    /*printf("I,%d,%d,%d,%d\n", indInicial->soft1, indInicial->soft2,
            indInicial->soft3, indInicial->soft4);*/
    do {

        //printf("FO Atual: %f\n", foAtual);
        for (i = 0; i < nVizinhos; i++) {

            pViz = (float) rand() / RAND_MAX;

            if (pViz < 0.5) {
                vizinhos[i] = geraSwap(p, indInicial);
            } else if (pViz < 1.4) {
                vizinhos[i] = geraMove(p, indInicial);
            } else if (pViz < 0.6) {
                vizinhos[i] = geraTimeMove(p, indInicial);
            } else if (pViz < 0.8) {
                vizinhos[i] = geraRoomMove(p, indInicial);
            } else if (pViz < 0.9) {
                vizinhos[i] = geraIsolatedLectureMove(p, indInicial);
            } else {
                vizinhos[i] = geraMinWorkingDaysMove(p, indInicial);
            }

            if (1) {
                if (vizinhos[i]->deltaHard > 0) {
                    p->vHard++;
                } else if (vizinhos[i]->deltaSoft > 0) {
                    p->comPiora++;
                } else if (vizinhos[i]->deltaSoft < 0) {
                    p->comMelhora++;
                } else {
                    p->semEfeito++;
                }
            }

            totalElementos += nVizinhos;
            //foVizinhos[i] = funcaoObjetivo(p, vizinhos[i]);

            //getHashCode(vizinhos[i]);
            if (i == 0) {
                melhorVizinho = 0;
            } else {
                if (vizinhos[i]->deltaHard * p->pesoHard + vizinhos[i]->deltaSoft <
                        vizinhos[melhorVizinho]->deltaHard * p->pesoHard + vizinhos[melhorVizinho]->deltaSoft) {
                    melhorVizinho = i;
                }
            }

            //printf("\tDeltaSoft[%d]: %f \n", i, vizinhos[i]->deltaSoft);
        }

        //printf(": Melhor: %d\n", melhorVizinho);

        //imprimePercListaTabu(listaTabu, p->dimensao);

        deltaF = vizinhos[melhorVizinho]->deltaHard * p->pesoHard + vizinhos[melhorVizinho]->deltaSoft;

        //printf("Df=%f\n", deltaF);

        //exit(0);

        if (deltaF <= 0) {// função objetivo decresceu
            //scanf("%d\n", &i);

            troca_par_completo(p, indInicial, vizinhos[melhorVizinho]->p1, vizinhos[melhorVizinho]->p2);

            /*if (foAtual + deltaF != funcaoObjetivo(p, indInicial, 10000)) {
                imprimeIndividuo3(p, indInicial);
                printf("F1: %f\n", foAtual + deltaF);
                printf("F2: %f\n", funcaoObjetivo(p, indInicial, 10000));
                exit(0);
            }*/
            foAtual += deltaF;
            //melhorInd = solucaoAtual;
            if (deltaF < 0) {
                if (p->info) {
                    printf("LH: %f\n", foAtual);
                }
                iteracoesSemMelhora = 0; // continua buscando
                //zeraListaTabu(listaTabu, p->dimensao);
            }

        } else {
            //printf("Nao melhorou\n");
        }

        //printf("FO(%d): %f\n", iteracoesSemMelhora, foAtual);

        //printf("ADesalocar: %p %p %p\n", aDesalocar, solucaoAtual, vizinho);
        for (i = 0; i < nVizinhos; i++) {
            free(vizinhos[i]);
        }


        //somaViolacoesSoft(p, solucaoAtual);

        /*if (iteracoesSemMelhora && iteracoesSemMelhora % 5000 == 0) {
            //printf("I[%d],%d,%d,%d,%d === %f\n", iteracoesSemMelhora, solucaoAtual->soft1, solucaoAtual->soft2,
            //      solucaoAtual->soft3, solucaoAtual->soft4, funcaoObjetivo(p, solucaoAtual));

            somaViolacoesSoft(p, solucaoAtual);
            printf("Conflitos: ");
            for (i = 0; i < solucaoAtual->nConflitos; i++) {
                printf("%d ", solucaoAtual->posConflitos[i]);
            }
            printf("\n");

        }*/


        iteracoesSemMelhora++;
        count++;

        /*if (iteracoesSemMelhora % 2000 == 0 && nPerturbacoes < 3) {
            printf("perturbando solucao\n");
            perturbaSolucao(p, solucaoAtual);
            foAtual = funcaoObjetivo(p, solucaoAtual);
            nPerturbacoes++;
        }*/

        //printf("Iter: %d / FO: %f\n", iteracoesSemMelhora, foAtual);
    } while (iteracoesSemMelhora < p->nIterSemMelhoras); // || iteracoesComMesmoFo < 200);

    /*printf("I,%d,%d,%d,%d\n", indInicial->soft1, indInicial->soft2,
            indInicial->soft3, indInicial->soft4);*/

    //printf("T=%f, Pioras=%d, FO=%f (%f, %f)\n", t0, nPioras, foAtual,
    //somaViolacoesHard(p, solucaoAtual), somaViolacoesSoft(p, solucaoAtual));

    free(vizinhos);

    return indInicial;
}

Individuo *buscaLocalGraspMista(Problema*p, Individuo *indInicial) {
    Individuo *solucaoAtual, *aDesalocar;
    //Individuo **vizinhos;
    float foAtual;
    float deltaF;
    float *foVizinhos;
    int i;
    int melhorVizinho;
    ListaVizinhos *listaVizinhos;

    listaVizinhos = alocaListaVizinhos(p->k);

    //vizinhos = (Individuo**) malloc(nVizinhos * sizeof (Individuo*));
    foVizinhos = (float*) malloc(p->k * sizeof (float));

    foAtual = funcaoObjetivo(p, indInicial, 10000);

    solucaoAtual = indInicial;
    int iteracoesSemMelhora = 0;
    int iteracoesComMesmoFo = 0;

    do {

        //printf("FO Atual: %f\n", foAtual);

        geraListaVizinhos(p, solucaoAtual, listaVizinhos);

        for (i = 0; i < p->k; i++) {
            int p1 = listaVizinhos->pos1[i];
            int p2 = listaVizinhos->pos2[i];

            troca_par(solucaoAtual, p1, p2);
            //vizinhos[i] = geraVizinho2(p, solucaoAtual);
            //foVizinhos[i] = funcaoObjetivo(p, vizinhos[i]);
            foVizinhos[i] = somaViolacoesSoft(p, solucaoAtual);
            if (i == 0) {
                melhorVizinho = 0;
            } else {
                if (foVizinhos[i] < foVizinhos[melhorVizinho]) {
                    melhorVizinho = i;
                }
            }

            troca_par(solucaoAtual, p1, p2);

            //printf("%f ", foVizinhos[i]);
        }

        //printf(": Melhor: %d\n", melhorVizinho);



        deltaF = foVizinhos[melhorVizinho] - foAtual;

        //printf("Df=%f\n", deltaF);

        aDesalocar = 0;
        if (deltaF <= 0) {// função objetivo decresceu
            //scanf("%d\n", &i);
            foAtual = foVizinhos[melhorVizinho];
            //printf("Melhorou... %f\n", foAtual);
            //aDesalocar = solucaoAtual;
            //solucaoAtual = vizinhos[melhorVizinho];
            //vizinhos[melhorVizinho] = aDesalocar;
            troca_par(solucaoAtual,
                    listaVizinhos->pos1[melhorVizinho],
                    listaVizinhos->pos2[melhorVizinho]);
            //melhorInd = solucaoAtual;
            if (deltaF < 0) {
                iteracoesSemMelhora = 0; // continua buscando
                iteracoesComMesmoFo = 0;
            } else {
                iteracoesComMesmoFo++;
            }

        } else {
            /////////////////aDesalocar = vizinho;
        }

        printf("FO(%d,%d): %f\n", iteracoesSemMelhora, iteracoesComMesmoFo, foAtual);

        //printf("ADesalocar: %p %p %p\n", aDesalocar, solucaoAtual, vizinho);
        /*for (i = 0; i < nVizinhos; i++) {
            liberaIndividuo(vizinhos[i]);
        }*/

        iteracoesSemMelhora++;

        //printf("Iter: %d / FO: %f\n", iteracoes, foAtual);
    } while (iteracoesSemMelhora < p->nIterSemMelhoras); // || iteracoesComMesmoFo < 200);

    //printf("T=%f, Pioras=%d, FO=%f (%f, %f)\n", t0, nPioras, foAtual,
    //somaViolacoesHard(p, solucaoAtual), somaViolacoesSoft(p, solucaoAtual));

    //free(vizinhos);
    free(foVizinhos);
    desalocaListaVizinhos(listaVizinhos);

    return solucaoAtual;
}

void geraSolucaoInicialGrasp(Problema *p, AuxGrasp *auxGrasp, Individuo *indBase) {
    int aula, i;

    resetAuxGrasp(p, auxGrasp, indBase);

    /*for (i = 0; i < auxGrasp->nCandidatos; i++) {
        //printf("%d ", auxGrasp->candidatos[i]);
        //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
    }
    //printf("\n");*/

    ordenaDisiciplinasPorDificuldade(p, auxGrasp);
    //TODO: talvez pode sair. Tem no WHILE

    //printf("==============================================================================\n");

    for (i = 0; i < auxGrasp->nCandidatos; i++) {
        //printf("%d ", auxGrasp->candidatos[i]);
        //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
    }
    //printf("\n");


    while (auxGrasp->nCandidatos > 0) {// enquanto ha candidatos a alocar

        //printf("Candidatos restantes: %d\n", auxGrasp->nCandidatos);

        //ordenaDisiciplinasPorDificuldade(p, auxGrasp);

        // aula "mais dificil"
        aula = auxGrasp->candidatos[auxGrasp->nCandidatos - 1];
        //aula = 11;
        //printf("Escolheu aula %d\n", aula);

        //auxGrasp->ind->aula[4] = 7;
        //auxGrasp->ind->aula[1] = 10;
        //auxGrasp->ind->aula[2] = 9;
        alocaAula(p, auxGrasp, aula, 0);

        //getchar();
        //break;
    }


    /*for (i = 0; i < p->nDias * p->nPerDias; i++) {
        //printf("%d: %d/%d\n", i, getPeriodo(p, i), p->nPerDias);
    }*/

    //return auxGrasp;
}

void desalocaAuxGrasp(Problema *p, AuxGrasp *auxGrasp) {
    int i;
    free(auxGrasp->candidatos);
    free(auxGrasp->explosao);

    for (i = 0; i < p->dimensao; i++) {
        free(auxGrasp->vetorPossibilidades[i]);
    }

    free(auxGrasp->vetorPossibilidades);

    for (i = 0; i < auxGrasp->tPool; i++) {
        liberaIndividuo(auxGrasp->poolElite[i]);
    }

    free(auxGrasp->poolElite);
}

void copiaIndividuo2(Individuo* indFonte, Individuo* indDestino) {
    int i;

    for (i = 0; i < indFonte->n; i++) {
        indDestino->aula[i] = indFonte->aula[i];
    }
    indDestino->fitness = indFonte->fitness;
}

Individuo *pathRelinking(Problema *p, Individuo *solucaoAtual, AuxGrasp *auxGrasp) {

    int i, pos;
    Individuo *target, *inter, *initial;
    int *posicoesTarget;
    float melhorFoGlobal = 999999;
    Individuo *bestGlobal;

    bestGlobal = alocaIndividuo();
    criaIndividuo(bestGlobal, p);
    copiaIndividuo2(solucaoAtual, bestGlobal);

    // inividuo intermediario, usado para percorrer o caminho (link)
    inter = alocaIndividuo();
    criaIndividuo(inter, p);
    copiaIndividuo2(solucaoAtual, inter);

    // escolhe uma solucao elite
    pos = rand() % auxGrasp->nElites;
    target = auxGrasp->poolElite[pos];

    posicoesTarget = (int*) malloc(p->nAulas * sizeof (int));

    // marca a posicao de cada aula
    for (i = 0; i < target->n; i++) {
        if (ehAula(p, target->aula[i])) {
            posicoesTarget[target->aula[i] - 1] = i;
        }
    }

    /*imprimeIndividuo(target);
    //printf("Posicoes: ");
    for (i = 0; i < p->nAulas; i++) {
        printf("%d ", posicoesTarget[i]);
    }
    printf("\n");
    imprimeIndividuo(solucaoAtual);*/

    float fo = funcaoObjetivo(p, inter, 10000);
    int move;

    do {
        move = 0; // indica se houve movimento
        float melhorFo = 999999, foAux;
        int t1, t2; // posicoes da melhor troca
        // verifica as aulas que estao em posicoes diferentes
        for (i = 0; i < inter->n; i++) {
            if (ehAula(p, inter->aula[i])) {
                int aula = inter->aula[i];
                pos = posicoesTarget[aula - 1];

                if (i != pos) {
                    //printf("Aula %d != (%d,%d)", aula, i, pos);
                    troca_par(inter, pos, i);
                    foAux = funcaoObjetivo(p, inter, 10000);

                    if (foAux < melhorFo) {
                        melhorFo = foAux;
                        t1 = i;
                        t2 = pos;
                    }
                    //printf(", FO= %f => %f\n", fo, foAux);
                    troca_par(inter, pos, i);

                    move = 1;
                } else {
                    //printf("Aula %d == (%d,%d)\n", aula, i, pos);
                }
            }
        }

        // realiza a melhor troca
        if (move) {
            //printf("%d <=> %d, fo=%f\n", t1, t2, melhorFo);
            troca_par(inter, t1, t2);
            fo = melhorFo;

            if (melhorFo < melhorFoGlobal) {
                copiaIndividuo2(inter, bestGlobal);
                melhorFoGlobal = melhorFo;
            }
        }

        //scanf("%d", &i);
    } while (move);

    //printf("Source: %f\n", funcaoObjetivo(p, solucaoAtual));
    //printf("Path-Relinking: %f\n", funcaoObjetivo(p, bestGlobal));
    //printf("Target: %f\n", funcaoObjetivo(p, target));

    free(posicoesTarget);
    return bestGlobal;

}

Individuo *pathRelinking2(Problema *p, Individuo *solucaoAtual, AuxGrasp *auxGrasp) {

    int i, pos;
    Individuo *target, *inter, *initial;
    int *posicoesTarget;
    float melhorFoGlobal = 999999;
    Individuo *bestGlobal;
    Neighbour *mov;
    float deltaF, pesoHard;

    pesoHard = 10000;

    bestGlobal = alocaIndividuo();
    criaIndividuo(bestGlobal, p);
    copiaIndividuo2(solucaoAtual, bestGlobal);

    mov = (Neighbour*) malloc(sizeof (Neighbour));


    // escolhe uma solucao elite
    pos = rand() % auxGrasp->nElites;
    initial = auxGrasp->poolElite[pos];

    // inividuo intermediario, usado para percorrer o caminho (link)
    inter = alocaIndividuo();
    criaIndividuo(inter, p);
    copiaIndividuo2(initial, inter);
    zeraMatCurrDiasPeriodos(p, inter);
    inicializaMatrizesAuxiliares(p, inter);

    target = solucaoAtual;
    posicoesTarget = (int*) malloc(p->nAulas * sizeof (int));

    // marca a posicao de cada aula
    for (i = 0; i < target->n; i++) {
        if (ehAula(p, target->aula[i])) {
            posicoesTarget[target->aula[i] - 1] = i;
        }
    }

    /*imprimeIndividuo(target);
    //printf("Posicoes: ");
    for (i = 0; i < p->nAulas; i++) {
        printf("%d ", posicoesTarget[i]);
    }
    printf("\n");
    imprimeIndividuo(solucaoAtual);*/

    float fo = funcaoObjetivo2(p, inter, pesoHard);
    int move;

    do {
        move = 0; // indica se houve movimento
        float melhorFo = 999999, foAux;
        int t1, t2; // posicoes da melhor troca
        // verifica as aulas que estao em posicoes diferentes
        for (i = 0; i < inter->n; i++) {
            if (ehAula(p, inter->aula[i])) {
                int aula = inter->aula[i];
                pos = posicoesTarget[aula - 1];

                if (i != pos) {
                    //printf("Aula %d != (%d,%d)", aula, i, pos);
                    //troca_par(inter, pos, i);
                    mov->p1 = i;
                    mov->p2 = pos;
                    avaliaNeighbour(p, inter, mov);

                    //foAux = funcaoObjetivo(p, inter, 10000);
                    deltaF = mov->deltaHard * pesoHard + mov->deltaSoft;

                    if (fo + deltaF < melhorFo) {
                        melhorFo = fo + deltaF;
                        t1 = i;
                        t2 = pos;
                    }
                    //printf(", FO= %f => %f\n", fo, foAux);
                    //troca_par(inter, pos, i);

                    move = 1;
                } else {
                    //printf("Aula %d == (%d,%d)\n", aula, i, pos);
                }
            }
        }

        // realiza a melhor troca
        if (move) {
            //printf("%d <=> %d, fo=%f\n", t1, t2, melhorFo);
            troca_par_completo(p, inter, t1, t2);
            fo = melhorFo;

            if (melhorFo < melhorFoGlobal) {
                copiaIndividuo2(inter, bestGlobal);
                melhorFoGlobal = melhorFo;
            }
        }

        //scanf("%d", &i);
    } while (move);

    //printf("Source: %f\n", funcaoObjetivo(p, initial));
    //printf("Path-Relinking: %f\n", funcaoObjetivo(p, bestGlobal));
    //printf("Target: %f\n", funcaoObjetivo(p, target));

    free(posicoesTarget);
    free(mov);

    return bestGlobal;

}

/**
 * Atualiza informações do pool de solucoes elite
 * @param 
 * @return 
 */
void atualizaInfoPool(AuxGrasp *auxGrasp) {
    int i;

    // marca o primeiro como sendo o pior do pool
    auxGrasp->piorSolucao = auxGrasp->poolElite[0]->fitness;
    auxGrasp->posicaoPior = 0;

    // verifica no restante do pool se tem algum pior
    for (i = 1; i < auxGrasp->nElites; i++) {
        if (auxGrasp->poolElite[i]->fitness > auxGrasp->piorSolucao) {
            auxGrasp->piorSolucao = auxGrasp->poolElite[i]->fitness;
            auxGrasp->posicaoPior = i;
        }
    }
}

/**
 * Verifica se um individuo identico ja esta presente no pool
 */
int jaTemNoPool(AuxGrasp *auxGrasp, Individuo *ind) {
    int i, j;
    float fo = ind->fitness;

    //printf("FO: %f\n", fo);

    for (i = 0; i < auxGrasp->nElites; i++) {
        //printf("=> %f\n, ", auxGrasp->poolElite[i]->fitness);
        // se fo eh diferente eles sao diferentes
        if (auxGrasp->poolElite[i]->fitness != fo) {
            continue;
        }

        for (j = 0; j < ind->n; j++) {
            if (ind->aula[j] != auxGrasp->poolElite[i]->aula[j]) {
                continue;
            }
        }

        // => fo igual e elementos iguais
        return 1;
    }

    return 0;

}

/**
 * Atualiza informações do pool de solucoes elite
 * @param 
 * @return 
 */
void atualizaPool(AuxGrasp *auxGrasp, Individuo *ind) {
    int i;

    // nao coloca no pool uma solucao ja existente
    if (jaTemNoPool(auxGrasp, ind)) {
        return;
    }

    if (auxGrasp->nElites < auxGrasp->tPool) {
        // ha espaco no pool
        copiaIndividuo2(ind, auxGrasp->poolElite[auxGrasp->nElites]);
        auxGrasp->nElites++;
        atualizaInfoPool(auxGrasp);
    } else {
        // pool cheio: verifica se nova solucao pode entrar

        if (ind->fitness < auxGrasp->piorSolucao) {
            copiaIndividuo2(ind, auxGrasp->poolElite[auxGrasp->posicaoPior]);
            atualizaInfoPool(auxGrasp);
        }
    }

}

Individuo *grasp(Problema *p) {
    int i, j, fezPR;
    float fo, foIter, foPR, melhor = 9999999;
    Individuo *ind, *bestInd, *indPr, *bestIter;
    AuxGrasp *auxGrasp;
    int iteracoesExecutadas = 0;

    auxGrasp = geraAuxGrasp(p);

    bestInd = alocaIndividuo();
    criaIndividuo(bestInd, p);

    p->mediaSolucoes = 0;
    p->soft1 = p->soft2 = p->soft3 = p->soft4 = 0;
    p->f1 = p->f2 = p->f3 = 0;

    for (i = 0; i < p->maxIterGrasp; i++) {

        p->pAproveitamento = (float) rand() / RAND_MAX;
        //printf("Iter:%d\n", i + 1);
        if (i == 0) {
            geraSolucaoInicialGrasp(p, auxGrasp, NULL);
        } else {
            geraSolucaoInicialGrasp(p, auxGrasp, bestInd);
        }
        ind = auxGrasp->ind;

        zeraMatCurrDiasPeriodos(p, ind);
        inicializaMatrizesAuxiliares(p, ind);
        if (p->info) {
            printf("F1: %f\n", funcaoObjetivo2(p, ind, 10000));
        }
        //printf("HARD: %f\n", somaViolacoesHard(p, ind));
        //printf("SOFT: %f\n", somaViolacoesSoft(p, ind));
        //if (i==4)exit(0);else continue;
        p->f1 += funcaoObjetivo2(p, ind, 10000);

        fezPR = 0; // flag: fez Path-Relinking

        foIter = funcaoObjetivo2(p, ind, 10000);
        p->pesoHard = 10000;


        if (p->buscaLocalGrasp == 1) {
            ind = buscaLocalGraspProfundidade(p, ind);
        } else if (p->buscaLocalGrasp == 2) {
            ind = buscaLocalGraspHibrida(p, ind);
        } else if (p->buscaLocalGrasp == 3) {
            ind = buscaLocal2Etapas(p, ind, 2);
        } else if (p->buscaLocalGrasp == 4) {
            ind = buscaLocalGraspMista(p, ind);
        } else if (p->buscaLocalGrasp == 5) {
            ind = buscaLocalGraspVNS(p, ind);
        } else if (p->buscaLocalGrasp == 6) {
            //ind = buscaLocalGraspHibrida(p, ind);
            ind = simulatedAnnealing2(p, ind);
        } else if (p->buscaLocalGrasp == 7) {
            ind = greatDeluge(p, ind);
        } else {
            //printf("Tipo de busca local nao foi informada!\n");
            //exit(1);
        }

        bestIter = ind;

        fo = funcaoObjetivo2(p, ind, 10000);

        p->f2 += fo;


        ind->fitness = fo;
        //printf("----------------------------------\n", fo);
        if (p->info) {
            printf("F2: %f\n", fo);
        }

        //printf("%.2f: %f\n", p->pAproveitamento, fo);
        //printf("HARD: %f\n", somaViolacoesHard(p, ind));
        //printf("SOFT: %f\n", somaViolacoesSoft(p, ind));
        //printf("\n\n\n", fo);



        p->soft1 += ind->soft1;
        p->soft2 += ind->soft2;
        p->soft3 += ind->soft3;
        p->soft4 += ind->soft4;

        if (i > 1/*auxGrasp->tPool*/ && p->buscaLocalGrasp <= 7) {// se pool de elites ja esta cheio
            indPr = pathRelinking2(p, ind, auxGrasp);
            zeraMatCurrDiasPeriodos(p, indPr);
            inicializaMatrizesAuxiliares(p, indPr);
            foPR = funcaoObjetivo2(p, indPr, 10000);
            indPr->fitness = foPR;
            fezPR = 1; // fez Path-Relinking
        }

        if (fezPR) {// se fez Path-Relinking
            if (p->info) {
                printf("F3: %f\n", foPR);
            }
            if (foPR < fo) {
                bestIter = indPr;
                fo = foPR;
            } else {
                bestIter = ind;
            }
        } /*else {
            bestIter = ind;
        }*/

        p->f3 += funcaoObjetivo2(p, bestIter, 10000);

        /*printf("Fo=%.0f\n",fo);


        printf("POOL(1): [");
        for (j = 0; j < auxGrasp->nElites; j++) {
            printf("%.0f ", auxGrasp->poolElite[j]->fitness);
        }

        printf("]\n");

        atualizaPool(auxGrasp, bestIter);

        printf("]\n");*/

        atualizaPool(auxGrasp, bestIter);

        /*if (auxGrasp->nElites < auxGrasp->tPool) {
            // ha espaco no pool
            copiaIndividuo2(bestIter, auxGrasp->poolElite[auxGrasp->nElites]);
            auxGrasp->nElites++;
            atualizaInfoPool(auxGrasp);
        } else {
            // pool cheio: verifica se nova solucao pode entrar

            if (fo < auxGrasp->piorSolucao) {
                copiaIndividuo2(bestIter, auxGrasp->poolElite[auxGrasp->posicaoPior]);
                atualizaInfoPool(auxGrasp);
            }
        }*/


        /*printf("POOL(2): [");
        for (j = 0; j < auxGrasp->nElites; j++) {
            printf("%.0f ", auxGrasp->poolElite[j]->fitness);
        }
        printf("]\n");*/

        //scanf("%d", &j);

        p->mediaSolucoes += fo;

        if (fo < melhor) {
            melhor = fo;
            copiaIndividuo2(bestIter, bestInd);
            zeraMatCurrDiasPeriodos(p, bestInd);
            inicializaMatrizesAuxiliares(p, bestInd);

        }
        liberaIndividuo(ind);
        if (fezPR) {// libera individuo do Path-Relinking
            liberaIndividuo(indPr);
        }

        if (p->info) {
            printf("Iter:%d,FO=%f\n", i + 1, melhor);
        }
        //printf("%f %f %f\n", p->f1, p->f2, p->f3);
        fflush(stdout);

        iteracoesExecutadas++;

        if (esgotouTempoLimite(p))break;

    }

    desalocaAuxGrasp(p, auxGrasp);

    p->mediaSolucoes /= iteracoesExecutadas;

    p->soft1 /= iteracoesExecutadas;
    p->soft2 /= iteracoesExecutadas;
    p->soft3 /= iteracoesExecutadas;
    p->soft4 /= iteracoesExecutadas;

    p->f1 /= iteracoesExecutadas;
    p->f2 /= iteracoesExecutadas;
    p->f3 /= iteracoesExecutadas;

    return bestInd;

}


