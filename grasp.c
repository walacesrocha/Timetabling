#include <stdio.h>
#include <stdlib.h>
#include "grasp.h"
#include "problema.h"
#include "util.h"

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
    printf("ord GRASP\n");


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

    return auxGrasp;
}

int *getHorariosViaveis(Problema*p, AuxGrasp*auxGrasp, int aula) {
    int *horariosViaveis;
    int i, j, qtHorarios;
    Disciplina *disc;

    qtHorarios = p->nDias * p->nPerDias;

    horariosViaveis = (int*) malloc(qtHorarios * sizeof (int));

    // marca todos horarios inicialmente como viaveis
    for (i = 0; i < qtHorarios; i++) {
        horariosViaveis[i] = 1;
    }

    /*for (i = 0; i < qtHorarios; i++) {
        printf("%d ", horariosViaveis[i]);
    }
    printf("\n");*/
    // tira os horarios restritos
    disc = acessaDisciplina(p, aula);
    for (i = 0; i < disc->nIndisponibilidades; i++) {
        horariosViaveis[disc->indisponibilidades[i]] = 0;
    }

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

    return horariosViaveis;
}

int getTotalHorariosViaveis(Problema *p, AuxGrasp* auxGrasp, int aula) {
    int total, i, qtHorarios;
    int *horariosViaveis = getHorariosViaveis(p, auxGrasp, aula);

    total = 0;
    qtHorarios = p->nDias * p->nPerDias;

    for (i = 0; i < qtHorarios; i++) {
        if (horariosViaveis[i] == 1) {
            total++;
        }
    }
    return total;
}

void avaliaCustoAlocacao(Problema *p, AuxGrasp *auxGrasp, int aula, int nrP) {

    AlocacaoAula *alocacao;
    Disciplina *disc;
    Sala *sala;
    int *diasOcupados, totalDiasOcupados;
    int *salasOcupadas, totalSalasOcupadas;
    int i, qtHorarios;

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
            if (aulasMesmaDisciplina(p, aula, auxGrasp->ind->aula[i])) {
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



    //printf("MIN=%d, ocupados: %d\n", disc->minDiasAula, totalDiasOcupados);


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

void alocaAula(Problema *p, AuxGrasp* auxGrasp, int aula) {
    int *horariosViaveis;
    int i, j, nrP, pos, qtHorarios; // quantidade de horarios: dias x periodos
    AlocacaoAula *alocacao;
    int tLRC = 3; // tamanho da lista restrita de candidatos

    qtHorarios = p->nDias * p->nPerDias;

    horariosViaveis = getHorariosViaveis(p, auxGrasp, aula);

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

    // avalia todas as possibilidades
    for (nrP = 0; nrP < auxGrasp->nrPossibilidades; nrP++) {
        avaliaCustoAlocacao(p, auxGrasp, aula, nrP);
        // TODO: falta a restricao ISOLATED_LECTURES
    }

    /*for (nrP = 0; nrP < auxGrasp->nrPossibilidades; nrP++) {
        alocacao = auxGrasp->vetorPossibilidades[nrP];
        printf("[%d] H=%d, S=%d, C=%d\n", alocacao->id, alocacao->horario, alocacao->sala, alocacao->custo);
    }*/

    // ordena as possibilidades
    ordenaPossibilidades(auxGrasp);

    // escohe uma possibilidade e aloca a aula
    int escolha = rand() % (minimo(tLRC, auxGrasp->nrPossibilidades));
    printf("Escolha: %d\n", escolha);
    
    alocacao = auxGrasp->vetorPossibilidades[escolha];
    
    // posicao a inserir
    pos = alocacao->horario + qtHorarios*alocacao->sala;
    
    // insere no timetabling
    auxGrasp->ind->aula[pos] = aula;
    auxGrasp->nCandidatos--;

    printf("Nr.Possibilidades: %d\n", auxGrasp->nrPossibilidades);
    /*for (nrP = 0; nrP < auxGrasp->nrPossibilidades; nrP++) {
        alocacao = auxGrasp->vetorPossibilidades[nrP];
        printf("[%d] H=%d, S=%d, C=%d\n", alocacao->id, alocacao->horario, alocacao->sala, alocacao->custo);
    }
    printf("\n");*/

}

Individuo *geraSolucaoInicialGrasp(Problema *p) {
    int i, j, k;
    int aula;
    AuxGrasp *auxGrasp;

    auxGrasp = geraAuxGrasp(p);

    for (i = 0; i < auxGrasp->nCandidatos; i++) {
        printf("%d ", auxGrasp->candidatos[i]);
        //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
    }
    printf("\n");

    ordenaDisiciplinasPorDificuldade(p, auxGrasp);

    printf("==============================================================================\n");

    for (i = 0; i < auxGrasp->nCandidatos; i++) {
        printf("%d ", auxGrasp->candidatos[i]);
        //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
    }
    printf("\n");


    while (auxGrasp->nCandidatos > 0) {// enquanto ha candidatos a alocar
        
        printf("Candidatos restantes: %d\n",auxGrasp->nCandidatos);

        ordenaDisiciplinasPorDificuldade(p, auxGrasp);

        // aula "mais dificil"
        aula = auxGrasp->candidatos[auxGrasp->nCandidatos - 1];
        //aula = 11;
        printf("Escolheu aula %d\n", aula);

        //auxGrasp->ind->aula[0] = 7;
        //auxGrasp->ind->aula[1] = 10;
        //auxGrasp->ind->aula[2] = 9;
        alocaAula(p, auxGrasp, aula);

        //getchar();
        //break;
    }


    for (i = 1; i <= p->nAulas; i++) {
        //printf("%s: %d\n", acessaDisciplina(p, i)->nomeDisciplina, getTotalHorariosViaveis(p, auxGrasp, i));
    }

    return auxGrasp->ind;
}
// 1 2 3 12 13 14 15 16 4 5 6 7 8 9 10 11 
